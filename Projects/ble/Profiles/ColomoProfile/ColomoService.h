/**************************************************************************************************
  Filename:       ColomoService.h
  Revised:        $2015/7/14 $
  Revision:       $v1.0 $

  Description:    This file contains the COLOMO service definitions and
                  prototypes.

**************************************************************************************************/

#ifndef COLOMOSERVICE_H
#define COLOMOSERVICE_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
 * CONSTANTS
 */
// Colomo Service Parameters
#define COLOMO_MEAS                      0
#define COLOMO_CTRL                      1
#define COLOMO_CNFM                      2
  
#ifdef BLEDEBUG
#define COLOMO_TEST                      3
#endif

// Colomo Service UUIDs
#define COLOMO_SERV_UUID                 0xFFF0
#define COLOMO_MEAS_UUID                 0xFFF1
#define COLOMO_CTRL_UUID                 0xFFF2
#define COLOMO_CNFM_UUID                 0xFFF3

// WeChat Service UUIDs
#define WECHAT_SERV_UUID                 0xFEE7
#define WECHAT_READ_UUID                 0xFEC9
#define WECHAT_INDICATE_UUID             0xFEC8
#define WECHAT_WRITE_UUID                0xFEC7
  
#ifdef BLEDEBUG
#define COLOMO_TEST_UUID                 0xFFF4
#endif

//Colomo Measure Data Size
#define COLOMO_MEAS_LENGTH               14
//Colomo Control Data Size
#define COLOMO_CTL_LENGTH                3
//Colomo Measure value table position
#define COLOMO_MEAS_VALUE_POS            2  
#define COLOMO_CNFM_VALUE_POS            8
  
// Value for command characteristic
#define COLOMO_COMMAND_ENERGY_EXP        0x01

// ATT Error code
// Control point value not supported
#define COLOMO_ERR_NOT_SUP               0x80

// COLOMO Service bit fields
#define COLOMO_SERVICE                   0x00000001

// Callback events
#define COLOMO_MEAS_NOTI_ENABLED         1
#define COLOMO_MEAS_NOTI_DISABLED        2
#define COLOMO_COMMAND_SET               3

/*********************************************************************
 * TYPEDEFS
 */
// Colomo Service callback function
typedef NULL_OK void (*colomoProfileChange_t)( uint8 paramID );

typedef struct
{
  colomoProfileChange_t        pfnColomoProfileChange;  // Called when characteristic value changes
} colomoProfileCBs_t;

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * Profile Callbacks
 */

/*********************************************************************
 * API FUNCTIONS 
 */

/*
 * COLOMO_AddService- Initializes the COLOMO service by registering
 *          GATT attributes with the GATT server.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 */
extern bStatus_t colomo_AddService( uint32 services );

/*
 * COLOMO_Register - Register a callback function with the
 *          COLOMO Service
 *
 * @param   pfnServiceCB - Callback function.
 */

bStatus_t colomo_Register( colomoProfileCBs_t *appCallbacks );

/*
 * COLOMO_SetParameter - Set a COLOMO parameter.
 *
 *    param - Profile parameter ID
 *    len - length of data to right
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate 
 *          data type (example: data type of uint16 will be cast to 
 *          uint16 pointer).
 */
extern bStatus_t colomo_SetParameter( uint8 param, uint8 len, void *value );
  
/*
 * COLOMO_GetParameter - Get a COLOMO parameter.
 *
 *    param - Profile parameter ID
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate 
 *          data type (example: data type of uint16 will be cast to 
 *          uint16 pointer).
 */
extern bStatus_t colomo_GetParameter( uint8 param, void *value );

/*********************************************************************
 * @fn          COLOMO_MeasNotify
 *
 * @brief       Send a notification containing a COLOMO
 *              measurement.
 *
 * @param       connHandle - connection handle
 * @param       pNoti - pointer to notification structure
 *
 * @return      Success or Failure
 */
extern bStatus_t colomo_MeasNotify( uint16 connHandle, attHandleValueNoti_t *pNoti );

/*********************************************************************
 * @fn          COLOMO_HandleConnStatusCB
 *
 * @brief       COLOMO Service link status change handler function.
 *
 * @param       connHandle - connection handle
 * @param       changeType - type of change
 *
 * @return      none
 */
extern void colomo_HandleConnStatusCB( uint16 connHandle, uint8 changeType );

/*********************************************************************
*********************************************************************/
extern void ColomoSendNotify(uint16 gapConnHandle, uint8 pos, uint8 * pBuf, uint8 len);
extern void ColomoSendIndication(uint16 gapConnHandle, uint8 pos, uint8 * pBuf, uint8 len, uint8 taskId);
#ifdef __cplusplus
}
#endif

#endif /* COLOMOSERVICE_H */
