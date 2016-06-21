
#ifndef WECHAT_H
#define WECHAT_H

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

/*********************************************************************
 * MACROS
 */
#define CLM_POFF_TOUT_REFILL                             st(ClmPoffTimeOut = 30;)

/*********************************************************************
 * FUNCTIONS
 */
extern void WechatDisconnect();
extern void getTaskId(uint8 taskId);
extern void AuthRequest_Init();
extern int wechat_data_consume_func(uint8 *data, uint32 len);
extern int ble_wechat_indicate_data_chunk();
extern int wechat_data_consume_func(uint8 *data, uint32 len);
extern void Wechat_Message();
extern uint8 device_auth();
extern uint8 device_init();
extern uint8 device_SendData(char *data, uint8 data_len);
extern uint8* headTail(uint8 *src);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* ColomoBLE_H */
