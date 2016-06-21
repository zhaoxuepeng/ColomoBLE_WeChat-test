
/*******************************************************************************
  Filename:       Wechatservice.h
  Revised:        $Date: 2015-12-29 18:30:29 $
  Revision:       $Revision: 001 $
  Author  :       Allenguo
  Description:    This file contains Wechatservice .


  Copyright 2015 - 20xx AllenGuo Incorporated. All rights reserved.


*******************************************************************************/


#ifndef WECHATSERVICE_H
#define WECHATSERVICE_H

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

// Profile Parameters
/*#define PP_LINK_LOSS_ALERT_LEVEL         0  // RW uint8 - Profile Attribute value
#define PP_IM_ALERT_LEVEL                1  // RW uint8 - Profile Attribute value
#define PP_PASSCODE_LEVEL                2  // RW int8 - Profile Attribute value
#define PP_CONN_LEVEL                    3  // RW int8 - Profile Attribute value
#define PP_BUZZ_SET_LEVEL                4  // RW int8 - Profile Attribute value*/

#define WECHAT_SERV_UUID                0xFEE7


#define WECHAT_WRITE_UUID               0xFEC7
#define WECHAT_INDICATE_UUID            0xFEC8
#define WECHAT_READ_UUID                0xFEC9

#define WECHAT_WRITE_LEN                20
#define WECHAT_INDICATE_LEN             20
#define WECHAT_READ_LEN                 6



#define WECHAT_WRITE                0
#define WECHAT_INDICATE             1
#define WECHAT_READ                 2   

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * Profile Callbacks
 */

// Callback when the device has been started.  Callback event to 
// the Notify of an attribute wcAttrChange_t.
typedef void (*wcAttrChange_t)( uint8 attrParamID );

typedef struct
{
  wcAttrChange_t        wcnAttrChange;  // Whenever the Link Loss Alert attribute changes
} wechatCBs_t;

/*********************************************************************
 * API FUNCTIONS 
 */
 
/*
 * ProxReporter_InitService- Initializes the Proximity Reporter service by
 *          registering GATT attributes with the GATT server. Only call
 *          this function once.

 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 */
extern bStatus_t Wechat_AddService( uint32 services );

/*
 * ProxReporter_RegisterAppCBs - Registers the application callback function.
 *                    Only call this function once.
 *
 *    appCallbacks - pointer to application callbacks.
 */
extern bStatus_t Wechat_RegisterAppCBs( wechatCBs_t *appCallbacks );


/*
 * ProxReporter_SetParameter - Set a Proximity Reporter parameter.
 *
 *    param - Profile parameter ID
 *    len - length of data to right
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate 
 *          data type (example: data type of uint16 will be cast to 
 *          uint16 pointer).
 */
extern bStatus_t Wechat_SetParameter( uint8 param, uint8 len, void *value );
  
/*
 * ProxReporter_GetParameter - Get a Proximity Reporter parameter.
 *
 *    param - Profile parameter ID
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate 
 *          data type (example: data type of uint16 will be cast to 
 *          uint16 pointer).
 */
extern bStatus_t Wechat_GetParameter( uint8 param, void *value );

extern bStatus_t Wechat_Indicate( uint16 connHandle, uint8 *pValue, uint8 len, uint8 taskId); 


/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif


#endif /* Wechatservice_H */
