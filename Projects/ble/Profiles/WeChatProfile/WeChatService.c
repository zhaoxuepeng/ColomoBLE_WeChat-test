
/*******************************************************************************
  Filename:       Wechatservice.c
  Revised:        $Date: 2015-12-29 18:30:29 $
  Revision:       $Revision: 001 $
  Author  :       Allenguo
  Description:    This file contains Wechatservice .


  Copyright 2015 - 20xx AllenGuo Incorporated. All rights reserved.


*******************************************************************************/


/*********************************************************************
 * INCLUDES
 */
#include "bcomdef.h"
#include "OSAL.h"
#include "linkdb.h"
#include "att.h"
#include "gatt.h"
#include "gatt_uuid.h"
//#include "gatt_profile_uuid.h"
#include "gattservapp.h"
#include "gapbondmgr.h"

#include "wechatservice.h"


/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */
//#define PP_DEFAULT_TX_POWER               0
//#define PP_DEFAULT_PATH_LOSS              0x7F
#define SERVAPP_NUM_ATTR_SUPPORTED        5


/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */


//PAIR Service UUID
CONST uint8 wechatServUUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16( WECHAT_SERV_UUID ), HI_UINT16( WECHAT_SERV_UUID )
};




/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
static wechatCBs_t *WC_AppCBs = NULL;


/*********************************************************************
 * Profile Attributes - variables
 */
//Pair Service
static CONST gattAttrType_t wechatService = { ATT_BT_UUID_SIZE, wechatServUUID };


//characteristic prop define

static uint8 wechatWirteCharProps = GATT_PROP_WRITE;
static uint8 wechatIndicateCharProps = GATT_PROP_INDICATE;
static uint8 wechatReadCharProps = GATT_PROP_READ;


//characteristic UUID define
static CONST uint8 wechatWirteCharUUID[ATT_BT_UUID_SIZE] = {LO_UINT16( WECHAT_WRITE_UUID ), HI_UINT16( WECHAT_WRITE_UUID )};
static CONST uint8 wechatIndicateCharUUID[ATT_BT_UUID_SIZE] = {LO_UINT16( WECHAT_INDICATE_UUID ), HI_UINT16( WECHAT_INDICATE_UUID )};
static CONST uint8 wechatReadCharUUID[ATT_BT_UUID_SIZE] = {LO_UINT16( WECHAT_READ_UUID ), HI_UINT16( WECHAT_READ_UUID )};

//characteristic value define
static uint8 wechatWirte[WECHAT_WRITE_LEN] = {0};
static uint8 wechatIndicate[WECHAT_INDICATE_LEN] = {0};
static uint8 wechatRead[WECHAT_READ_LEN] = {0xaa, 0xbb, 0xcc, 0xaa, 0xbb, 0xcc};


//characteristic configure pointer
static gattCharCfg_t wechatWirteConfig[GATT_MAX_NUM_CONN];;
static gattCharCfg_t wechatIndicateConfig[GATT_MAX_NUM_CONN];;
static gattCharCfg_t wechatReadConfig[GATT_MAX_NUM_CONN];;



//wechat Service Atttribute Table
static gattAttribute_t wechatAttrTbl[] = 
	{
	  // wechat service
	  { 
		{ ATT_BT_UUID_SIZE, primaryServiceUUID }, /* type */
		GATT_PERMIT_READ,						  /* permissions */
		0,										  /* handle */
		(uint8 *)&wechatService 				 /* pValue */
	  },
	  
	  // Characteristic Declaration
	  { 
		{ ATT_BT_UUID_SIZE, characterUUID },
		GATT_PERMIT_READ, 
		0,
		&wechatWirteCharProps 
	  },
	  // wechat write  attribute
	  { 
		{ ATT_BT_UUID_SIZE, wechatWirteCharUUID },
		GATT_PERMIT_WRITE | GATT_PERMIT_READ  , 
		0, 
		wechatWirte
	  },
		// Characteristic Declaration
	  { 
		{ ATT_BT_UUID_SIZE, characterUUID },
		GATT_PERMIT_READ, 
		0,
		&wechatIndicateCharProps 
	  },
	  // wechat  indicate attribute
	  { 
		{ ATT_BT_UUID_SIZE, wechatIndicateCharUUID },
		0,//GATT_PERMIT_READ, 
		0, 
		wechatIndicate 
	  },
	  // 3.Characteristic Configuration  
	  {  
		{ ATT_BT_UUID_SIZE, clientCharCfgUUID },  
		GATT_PERMIT_READ | GATT_PERMIT_WRITE,  
		0,	
		(uint8 *)&wechatIndicateConfig	
	  },	
	
	  // Characteristic Declaration
	  { 
	   { ATT_BT_UUID_SIZE, characterUUID },
	   GATT_PERMIT_READ, 
	   0,
	   &wechatReadCharProps 
	  },
	
		// wechat read Functiom attribute
	  { 
		{ ATT_BT_UUID_SIZE, wechatReadCharUUID },
		GATT_PERMIT_READ, 
		0, 
		wechatRead 
	  },
	 
	}

;


/*********************************************************************
 * LOCAL FUNCTIONS
 */
static uint8 Wechat_ReadAttrCB( uint16 connHandle, gattAttribute_t *pAttr, 
                                    uint8 *pValue, uint8 *pLen, uint16 offset, uint8 maxLen );
static bStatus_t Wechat_WriteAttrCB( uint16 connHandle, gattAttribute_t *pAttr,
                                         uint8 *pValue, uint8 len, uint16 offset );

static void Wechat_HandleConnStatusCB( uint16 connHandle, uint8 changeType );


/*********************************************************************
 * PROFILE CALLBACKS
 */
// Prox Reporter Service Callbacks
CONST gattServiceCBs_t wechatCBs =
{
  Wechat_ReadAttrCB,  // Read callback function pointer
  Wechat_WriteAttrCB, // Write callback function pointer
  NULL                      // Authorization callback function pointer
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      proxReporter_AddService
 *
 * @brief   Initializes the Proximity Reporter service by
 *          registering GATT attributes with the GATT server.
 *          Only call this function once.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 *
 * @return   Success or Failure
 */
bStatus_t Wechat_AddService( uint32 services )
{
  uint8 status = SUCCESS;

//  if ( services &  WECHAT_SERV_UUID)
  {
    // Initialize Client Characteristic Configuration attributes
	GATTServApp_InitCharCfg(INVALID_CONNHANDLE, wechatWirteConfig);
	GATTServApp_InitCharCfg(INVALID_CONNHANDLE, wechatIndicateConfig);
	GATTServApp_InitCharCfg(INVALID_CONNHANDLE, wechatReadConfig);

  
    // Register with Link DB to receive link status change callback
    VOID linkDB_Register( Wechat_HandleConnStatusCB );
    
    // Register Link Loss attribute list and CBs with GATT Server App  
    status = GATTServApp_RegisterService( wechatAttrTbl, 
                                          GATT_NUM_ATTRS( wechatAttrTbl ),
                                          &wechatCBs );
  }

  

  return ( status );
}


/*********************************************************************
 * @fn      proxReporter_RegisterAppCBs
 *
 * @brief   Registers the application callback function. Only call 
 *          this function once.
 *
 * @param   callbacks - pointer to application callbacks.
 *
 * @return  SUCCESS or bleAlreadyInRequestedMode
 */
bStatus_t Wechat_RegisterAppCBs( wechatCBs_t *appCallbacks )
{
  if ( appCallbacks )
  {
    WC_AppCBs = appCallbacks;
    
    return ( SUCCESS );
  }
  else
  {
    return ( bleAlreadyInRequestedMode );
  }
}

/*********************************************************************
 * @fn      proxReporter_SetParameter
 *
 * @brief   Set a Proximity Reporter parameter.
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
bStatus_t Wechat_SetParameter( uint8 param, uint8 len, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
  	case WECHAT_INDICATE:
 	   if (len == WECHAT_INDICATE_LEN)
 	   {
			VOID osal_memcpy(wechatIndicate, value, WECHAT_INDICATE_LEN);
 	   }
 	   else
 	   {
 		   ret = bleInvalidRange;
 	   }
 	   break;
	case WECHAT_READ:
 	   if (len == WECHAT_READ_LEN)
 	   {
			VOID osal_memcpy(wechatRead, value, WECHAT_READ_LEN);
 	   }
 	   break;	
    default:
        ret = INVALIDPARAMETER;
        break;        

  }
  
  return ( ret );
}

/*********************************************************************
 * @fn      ProxReporter_GetParameter
 *
 * @brief   Get a Proximity Reporter parameter.
 *
 * @param   param - Profile parameter ID
 * @param   value - pointer to data to put.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate 
 *          data type (example: data type of uint16 will be cast to 
 *          uint16 pointer).
 *
 * @return  bStatus_t
 */
bStatus_t Wechat_GetParameter( uint8 param, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
	case WECHAT_WRITE:
	  VOID osal_memcpy(value, wechatWirte, WECHAT_WRITE_LEN);
      break;
	case WECHAT_INDICATE:
	  VOID osal_memcpy(value, wechatIndicate, WECHAT_INDICATE_LEN);
	  break;
    default:
      ret = INVALIDPARAMETER;
      break;
  }
  
  return ( ret );
}

/*********************************************************************
 * @fn          proxReporter_ReadAttrCB
 *
 * @brief       Read an attribute.
 *
 * @param       
 *
 * @return      Success or Failure
 */
 
static uint8 Wechat_ReadAttrCB( uint16 connHandle, gattAttribute_t *pAttr, 
                                    uint8 *pValue, uint8 *pLen, uint16 offset, uint8 maxLen )
{
  uint16 uuid;
  bStatus_t status = SUCCESS;

  // Make sure it's not a blob operation
  if ( offset > 0 )
  {
    return ( ATT_ERR_ATTR_NOT_LONG );
  }  

  if ( pAttr->type.len == ATT_BT_UUID_SIZE )
  {
    // 16-bit UUID
    uuid = BUILD_UINT16( pAttr->type.uuid[0], pAttr->type.uuid[1]);
    
    switch (uuid)
    {
      // No need for "GATT_SERVICE_UUID" or "GATT_CLIENT_CHAR_CFG_UUID" cases;
      // gattserverapp handles those types for reads.
  	case WECHAT_READ_UUID:		  
  		*pLen = WECHAT_READ_LEN;
  	    VOID osal_memcpy(pValue, wechatRead, WECHAT_READ_LEN);
  	  break;  
    case WECHAT_INDICATE_UUID:		
		*pLen = WECHAT_INDICATE_LEN;
		VOID osal_memcpy(pValue, pAttr->pValue, WECHAT_INDICATE_LEN);
		break;	

      default:
        // Should never get here!
        *pLen = 0;
        status = ATT_ERR_ATTR_NOT_FOUND;
        break;
    }
  }
  else
  {
    //128-bit UUID
    *pLen = 0;
    status = ATT_ERR_INVALID_HANDLE;
  }

  return ( status );
}
/*********************************************************************
 * @fn      proxReporter_WriteAttrCB
 *
 * @brief   Validate attribute data prior to a write operation
 *
 * @param   connHandle ?connection message was received on
 * @param   pReq - pointer to request
 *
 * @return  Success or Failure
 */
static bStatus_t Wechat_WriteAttrCB( uint16 connHandle, gattAttribute_t *pAttr,
                                                 uint8 *pValue, uint8 len, uint16 offset )
{
  bStatus_t status = SUCCESS;
  uint8 notify = 0xFF;

  if ( pAttr->type.len == ATT_BT_UUID_SIZE )
  { 
    // 16-bit UUID
    uint16 uuid = BUILD_UINT16( pAttr->type.uuid[0], pAttr->type.uuid[1]);
    switch ( uuid )
    { 
	  case WECHAT_WRITE_UUID:
	  	osal_memcpy(pAttr->pValue, pValue, len);
        notify = WECHAT_WRITE;
        break;
      case GATT_CLIENT_CHAR_CFG_UUID:
       
		if ( pAttr->handle == wechatAttrTbl[5].handle )
    	{  
    	    status = GATTServApp_ProcessCCCWriteReq( connHandle, pAttr, pValue, len,  
    	                                             offset, GATT_CLIENT_CFG_INDICATE );  	
		notify = WECHAT_INDICATE;

		}  
		else
		{
		 	status = GATTServApp_ProcessCCCWriteReq(connHandle, pAttr, pValue, len,
                                                 	 offset, GATT_CLIENT_CFG_NOTIFY);
		}
        break;
        
      default:
        // Should never get here!
        status = ATT_ERR_ATTR_NOT_FOUND;
        break;
    }
  }
  else
  {
    // 128-bit UUID
    status = ATT_ERR_INVALID_HANDLE;
  }    
  
  // If an attribute changed then callback function to notify application of change
  if ( (notify != 0xFF) && WC_AppCBs && WC_AppCBs->wcnAttrChange )
    WC_AppCBs->wcnAttrChange( notify );

  return ( status );
}

/*********************************************************************
 * @fn          proxReporter_HandleConnStatusCB
 *
 * @brief       Tx Power Level Service link status change handler function.
 *
 * @param       connHandle - connection handle
 * @param       changeType - type of change
 *
 * @return      none
 */
static void Wechat_HandleConnStatusCB(uint16 connHandle,uint8 changeType)
{ 
  // Make sure this is not loopback connection
  if ( connHandle != LOOPBACK_CONNHANDLE )
  {
    // Reset Client Char Config if connection has dropped
    if ( ( changeType == LINKDB_STATUS_UPDATE_REMOVED )      ||
         ( ( changeType == LINKDB_STATUS_UPDATE_STATEFLAGS ) && 
           ( !linkDB_Up( connHandle ) ) ) )
    { 
      GATTServApp_InitCharCfg( connHandle, wechatWirteConfig );
      GATTServApp_InitCharCfg( connHandle, wechatIndicateConfig );
      GATTServApp_InitCharCfg( connHandle, wechatReadConfig );
    }
  }
}

//******************************************************************************    
//name:         Wechat_Indicate    
//introduce:    指示len长度的数据   
//parameter:    connHandle:连接句柄    
//              pValue:要通知的数据，最多20个字节    
//              len:要通知的数据的长度   
//              taskId:应答时要返回到的那个任务的id  
//return:       none    
//******************************************************************************   
bStatus_t Wechat_Indicate( uint16 connHandle, uint8 *pValue, uint8 len, uint8 taskId)  
{  
  attHandleValueInd_t  indi;
  uint16 value=5;
  
  value  = GATTServApp_ReadCharCfg( connHandle, wechatIndicateConfig );////读出CCC的值 
  if ( value & GATT_CLIENT_CFG_INDICATE ) //判断是否打开通知开关，打开了则发送数据 
  {  
    indi.handle = wechatAttrTbl[4].handle;  
    indi.len = 20;  
    osal_memcpy( indi.value, pValue, len);
    return(GATT_Indication( connHandle, &indi, FALSE, taskId ));  
  }  
  return(FAILURE);  
}   


/*********************************************************************
*********************************************************************/
