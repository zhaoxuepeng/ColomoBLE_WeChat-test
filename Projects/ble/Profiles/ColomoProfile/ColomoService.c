/**************************************************************************************************
  Filename:       ColomoService.c
  Revised:        $2015/7/14 $
  Revision:       $v1.0 $

  Description:    This file contains the COLOMO service definitions and
                  prototypes.

**************************************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "bcomdef.h"
#include "OSAL.h"
#include "linkdb.h"
#include "att.h"
#include "gatt.h"
#include "gatt_uuid.h"
#include "gattservapp.h"

#include "ColomoService.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
// colomo service
CONST uint8 colomoServUUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(COLOMO_SERV_UUID), HI_UINT16(COLOMO_SERV_UUID)
};

// colomo measurement characteristic
CONST uint8 colomoMeasUUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(COLOMO_MEAS_UUID), HI_UINT16(COLOMO_MEAS_UUID)
};

// colomo control characteristic
CONST uint8 colomoCtrlUUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(COLOMO_CTRL_UUID), HI_UINT16(COLOMO_CTRL_UUID)
};

// colomo confirm characteristic
CONST uint8 colomoCnfmUUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(COLOMO_CNFM_UUID), HI_UINT16(COLOMO_CNFM_UUID)
};


#ifdef BLEDEBUG
CONST uint8 colomoTestUUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(COLOMO_TEST_UUID), HI_UINT16(COLOMO_TEST_UUID)
};
#endif
/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

static colomoProfileCBs_t *colomoProfileCBs=NULL;

/*********************************************************************
 * Profile Attributes - variables
 */

// Colomo Service attribute
static CONST gattAttrType_t colomoService = { ATT_BT_UUID_SIZE, colomoServUUID };

// Colomo Measurement Characteristic
static uint8 colomoMeasProps = GATT_PROP_NOTIFY|GATT_PROP_READ|GATT_PROP_WRITE;
static uint8 colomoMeasValue[COLOMO_MEAS_LENGTH] = {0};
static gattCharCfg_t colomoMeasClientCharCfg[GATT_MAX_NUM_CONN];

// Colomo Control Characteristic
static uint8 colomoCtrlProps = GATT_PROP_READ|GATT_PROP_WRITE;
static uint8 colomoCtrlValue[COLOMO_CTL_LENGTH] = {0};
static uint8 colomoCtrlDesp[15] = "Colomo Control\0";

// Colomo Confirm Characteristic
static uint8 colomoCnfmProps = GATT_PROP_NOTIFY;
static uint8 colomoCnfmValue = 0x00;
static uint8 colomoCnfmDesp[15] = "Colomo Confirm\0";

#ifdef BLEDEBUG
// colomo test Characteristic
static uint8 colomoTestProps = GATT_PROP_READ|GATT_PROP_WRITE;
static uint8 colomoTestValue = 0xab;
static uint8 colomoTestDesp[11] = "colomoTest\0";
#endif
/*********************************************************************
 * Profile Attributes - Table
 */

static gattAttribute_t colomoAttrTbl[] = 
{
  //--------------------------------------------------------------------------//
  // Colomo Service
  //--------------------------------------------------------------------------//
  { 
    { ATT_BT_UUID_SIZE, primaryServiceUUID }, /* type */
    GATT_PERMIT_READ,                         /* permissions */
    0,                                        /* handle */
    (uint8 *)&colomoService                   /* pValue */
  },
  
  //--------------------------------------------------------------------------//
  // Colomo Measurement
  //--------------------------------------------------------------------------//
  // Declaration
  { 
    { ATT_BT_UUID_SIZE, characterUUID },
    GATT_PERMIT_READ, 
    0,
    &colomoMeasProps 
  },
  // Value
  { 
    { ATT_BT_UUID_SIZE, colomoMeasUUID },
    GATT_PERMIT_READ, 
    0, 
    colomoMeasValue
  },
  {
    { ATT_BT_UUID_SIZE, charUserDescUUID },
    GATT_PERMIT_READ, 
    0, 
    &colomoMeasProps 
  },
  
  //--------------------------------------------------------------------------//
  // Colomo Control
  //--------------------------------------------------------------------------//
  // Declaration
  { 
    { ATT_BT_UUID_SIZE, characterUUID },
    GATT_PERMIT_READ, 
    0,
    &colomoCtrlProps 
  },
  // Value
  { 
    { ATT_BT_UUID_SIZE, colomoCtrlUUID },
    GATT_PERMIT_WRITE | GATT_PERMIT_READ,  
    0, 
    colomoCtrlValue
  },
  {
    { ATT_BT_UUID_SIZE, charUserDescUUID },
    GATT_PERMIT_READ, 
    0, 
    colomoCtrlDesp 
  },
  
  //--------------------------------------------------------------------------//
  // Colomo Confirm
  //--------------------------------------------------------------------------//
  // Declaration
  { 
    { ATT_BT_UUID_SIZE, characterUUID },
    GATT_PERMIT_READ, 
    0,
    &colomoCnfmProps 
  },
  // Value
  { 
    { ATT_BT_UUID_SIZE, colomoCnfmUUID },
    GATT_PERMIT_READ, 
    0, 
    &colomoCnfmValue
  },
  {
    { ATT_BT_UUID_SIZE, charUserDescUUID },
    GATT_PERMIT_READ, 
    0, 
    colomoCnfmDesp 
  },
  
#ifdef BLEDEBUG   
    //colomo Test Declaration
    { 
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ, 
      0,
      &colomoTestProps
    },
    { 
      { ATT_BT_UUID_SIZE, colomoTestUUID },
      GATT_PERMIT_READ | GATT_PERMIT_WRITE, 
      0, 
      &colomoTestValue //
    },
    {
      { ATT_BT_UUID_SIZE, charUserDescUUID },
      GATT_PERMIT_READ, 
      0, 
      colomoTestDesp 
    }
#endif
};


/*********************************************************************
 * LOCAL FUNCTIONS
 */
static uint8 colomo_ReadAttrCB( uint16 connHandle, gattAttribute_t *pAttr, 
                            uint8 *pValue, uint8 *pLen, uint16 offset, uint8 maxLen );
static bStatus_t colomo_WriteAttrCB( uint16 connHandle, gattAttribute_t *pAttr,
                                 uint8 *pValue, uint8 len, uint16 offset );

/*********************************************************************
 * PROFILE CALLBACKS
 */
// Colomo Service Callbacks
CONST gattServiceCBs_t colomoCBs =
{
  colomo_ReadAttrCB,  // Read callback function pointer
  colomo_WriteAttrCB, // Write callback function pointer
  NULL                   // Authorization callback function pointer
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      colomo_AddService
 *
 * @brief   Initializes the Heart Rate service by registering
 *          GATT attributes with the GATT server.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 *
 * @return  Success or Failure
 */
bStatus_t colomo_AddService( uint32 services )
{
  uint8 status = SUCCESS;

  // Initialize Client Characteristic Configuration attributes
  GATTServApp_InitCharCfg( INVALID_CONNHANDLE, colomoMeasClientCharCfg );

  if ( services & COLOMO_SERVICE )
  {
    // Register GATT attribute list and CBs with GATT Server App
    status = GATTServApp_RegisterService( colomoAttrTbl, 
                                          GATT_NUM_ATTRS( colomoAttrTbl ),
                                          &colomoCBs );
  }

  return ( status );
}

/*********************************************************************
 * @fn     colomo_Register
 *
 * @brief   Register a callback function with the colomo Service.
 *
 * @param   pfnServiceCB - Callback function.
 *
 * @return  None.
 */

bStatus_t colomo_Register( colomoProfileCBs_t *appCallbacks )
{
  if ( appCallbacks )
  {
    colomoProfileCBs = appCallbacks;
    
    return ( SUCCESS );
  }
  else
  {
    return ( bleAlreadyInRequestedMode );
  }
}

/*********************************************************************
 * @fn      colomo_SetParameter
 *
 * @brief   Set a colomo parameter.
 *
 * @param   param - Profile parameter ID
 * @param   len - length of data to right
 * @param   value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate 
 *          data type (example: data type of uint16 will be cast to 
 *          uint16 pointer).
 *
 * @return  bStatus_t
 */
bStatus_t colomo_SetParameter( uint8 param, uint8 len, void *value )
{
  bStatus_t ret = SUCCESS;

  switch ( param )
  {
#ifdef BLEDEBUG
    case COLOMO_TEST:
      colomoTestValue = *((uint8*)value);
      break;
#endif
      
    case COLOMO_CTRL:
        osal_memcpy(colomoCtrlValue, value, COLOMO_CTL_LENGTH);
        break; 
        
    case COLOMO_MEAS:
      osal_memcpy( colomoMeasValue, value, COLOMO_MEAS_LENGTH );
      break;
      default:
        ret = INVALIDPARAMETER;
        break;
  }

  
  return ( ret );
}

/*********************************************************************
 * @fn      colomo_GetParameter
 *
 * @brief   Get a Heart Rate parameter.
 *
 * @param   param - Profile parameter ID
 * @param   value - pointer to data to get.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate 
 *          data type (example: data type of uint16 will be cast to 
 *          uint16 pointer).
 *
 * @return  bStatus_t
 */
bStatus_t colomo_GetParameter( uint8 param, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
 #ifdef BLEDEBUG
    case COLOMO_TEST:
      *((uint8*)value) = colomoTestValue;
      break;  
 #endif
    case COLOMO_CTRL:  
      osal_memcpy(value,colomoCtrlValue,COLOMO_CTL_LENGTH);
      break;
      
    case COLOMO_MEAS:
      osal_memcpy( value, colomoMeasValue, COLOMO_MEAS_LENGTH );
      break;
      default:
        ret = INVALIDPARAMETER;
        break;
  }

  return ( ret );
}
                               
/*********************************************************************
 * @fn          colomo_ReadAttrCB
 *
 * @brief       Read an attribute.
 *
 * @param       connHandle - connection message was received on
 * @param       pAttr - pointer to attribute
 * @param       pValue - pointer to data to be read
 * @param       pLen - length of data to be read
 * @param       offset - offset of the first octet to be read
 * @param       maxLen - maximum length of data to be read
 *
 * @return      Success or Failure
 */
static uint8 colomo_ReadAttrCB( uint16 connHandle, gattAttribute_t *pAttr, 
                            uint8 *pValue, uint8 *pLen, uint16 offset, uint8 maxLen )
{
  bStatus_t status = SUCCESS;

  // Make sure it's not a blob operation (no attributes in the profile are long)
  if ( offset > 0 )
  {
    return ( ATT_ERR_ATTR_NOT_LONG );
  }
 
  uint16 uuid = BUILD_UINT16( pAttr->type.uuid[0], pAttr->type.uuid[1]);

  if (uuid == COLOMO_MEAS_UUID)
  {
    *pLen = COLOMO_MEAS_LENGTH;
    osal_memcpy( pValue, pAttr->pValue, COLOMO_MEAS_LENGTH );
  }
  else if (uuid == COLOMO_CTRL_UUID)
  {
    *pLen = COLOMO_CTL_LENGTH;
    osal_memcpy(pValue, pAttr->pValue, COLOMO_CTL_LENGTH );
  } 
#ifdef BLEDEBUG
  else if (uuid == COLOMO_TEST_UUID)
  {
    *pLen = 1;
    pValue[0] = *pAttr->pValue;
  } 
#endif  
  else
  {
    status = ATT_ERR_ATTR_NOT_FOUND;
  }

  return ( status );
}

/*********************************************************************
 * @fn      colomo_WriteAttrCB
 *
 * @brief   Validate attribute data prior to a write operation
 *
 * @param   connHandle - connection message was received on
 * @param   pAttr - pointer to attribute
 * @param   pValue - pointer to data to be written
 * @param   len - length of data
 * @param   offset - offset of the first octet to be written
 *
 * @return  Success or Failure
 */
static bStatus_t colomo_WriteAttrCB( uint16 connHandle, gattAttribute_t *pAttr,
                                 uint8 *pValue, uint8 len, uint16 offset )
{
  bStatus_t status = SUCCESS;
  uint8 notifyApp = 0xFF;
  
  // If attribute permissions require authorization to write, return error
  if ( gattPermitAuthorWrite( pAttr->permissions ) )
  {
    // Insufficient authorization
    return ( ATT_ERR_INSUFFICIENT_AUTHOR );
  }
  
  if ( pAttr->type.len == ATT_BT_UUID_SIZE )
  {
    // 16-bit UUID
    uint16 uuid = BUILD_UINT16( pAttr->type.uuid[0], pAttr->type.uuid[1]);
    switch ( uuid )
    { 
#if (defined BLEDEBUG)
      case COLOMO_TEST_UUID:
#endif
      case COLOMO_CTRL_UUID:
        //Validate the value
        // Make sure it's not a blob oper
        if ( offset == 0 )
        {
          if ( len > 3 )
          {
            status = ATT_ERR_INVALID_VALUE_SIZE;
          }
        }
        else
        {
          status = ATT_ERR_ATTR_NOT_LONG;
        }
        //Write the value
        if ( status == SUCCESS )
        {
          uint8 *pCurValue = (uint8 *)pAttr->pValue;        
          //*pCurValue = pValue[0];
          osal_memcpy(pCurValue, pValue, COLOMO_CTL_LENGTH );
          // if( pAttr->pValue == &colomoCtrlValue )
          if( pAttr->pValue == colomoCtrlValue )
          {
            notifyApp = COLOMO_CTRL;        
          }
#if (defined BLEDEBUG)
          else
          {
            notifyApp = COLOMO_TEST;
          }
#endif
        }    
        break;
      
      case GATT_CLIENT_CHAR_CFG_UUID:
        status = GATTServApp_ProcessCCCWriteReq( connHandle, pAttr, pValue, len,
                                                 offset, GATT_CLIENT_CFG_NOTIFY );
        break;   
        
      default:
        // Should never get here! (characteristics 2 and 4 do not have write permissions)
        status = ATT_ERR_ATTR_NOT_FOUND;
        break;
    }
  }
  else
  {
    // 128-bit UUID
    status = ATT_ERR_INVALID_HANDLE;
  }

  // If a charactersitic value changed then callback function to notify application of change
  if ( (notifyApp != 0xFF ) && colomoProfileCBs && colomoProfileCBs->pfnColomoProfileChange )
  {
    colomoProfileCBs->pfnColomoProfileChange( notifyApp );  
  }
  
  return ( status );
  
}

/*********************************************************************
 * @fn          colomo_HandleConnStatusCB
 *
 * @brief       Heart Rate Service link status change handler function.
 *
 * @param       connHandle - connection handle
 * @param       changeType - type of change
 *
 * @return      none
 */
void colomo_HandleConnStatusCB( uint16 connHandle, uint8 changeType )
{ 
  // Make sure this is not loopback connection
  if ( connHandle != LOOPBACK_CONNHANDLE )
  {
    // Reset Client Char Config if connection has dropped
    if ( ( changeType == LINKDB_STATUS_UPDATE_REMOVED )      ||
         ( ( changeType == LINKDB_STATUS_UPDATE_STATEFLAGS ) && 
           ( !linkDB_Up( connHandle ) ) ) )
    { 
      GATTServApp_InitCharCfg( connHandle, colomoMeasClientCharCfg );
    }
  }
}

/*********************************************************************
*********************************************************************/
void ColomoSendNotify(uint16 gapConnHandle, uint8 pos, uint8 * pBuf, uint8 len)
{
  attHandleValueNoti_t notifyHandler;
  
  // Set the value handle
  notifyHandler.handle = colomoAttrTbl[pos].handle;
  // Set the length
  notifyHandler.len =  len;
  // Set the value
  osal_memcpy(notifyHandler.value, pBuf, len);
  // Send the notification
  VOID GATT_Notification( gapConnHandle, &notifyHandler, FALSE );
}
//qsun
void ColomoSendIndication(uint16 gapConnHandle, uint8 pos, uint8 * pBuf, uint8 len, uint8 taskId)
{
  attHandleValueInd_t indicationHandler;
  
  // Set the value handle
  indicationHandler.handle = colomoAttrTbl[pos].handle;
  // Set the length
  indicationHandler.len =  len;
  // Set the value
  osal_memcpy(indicationHandler.value, pBuf, len);
  // Send the notification
  VOID GATT_Indication( gapConnHandle, &indicationHandler, FALSE, taskId);
  
}
