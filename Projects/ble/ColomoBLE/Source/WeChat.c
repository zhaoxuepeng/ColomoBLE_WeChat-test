
/*******************************************************************************
  Filename:       Wechat.c
  Revised:        $Date: 2016-01-25 19:00:00 $
  Revision:       $Revision: 002 $
  Author  :       Allenguo
  Description:    This file contains Wechatservice .


  Copyright 2015 - 20xx AllenGuo Incorporated. All rights reserved.


*******************************************************************************/
#include "string.h"
#include "bcomdef.h"

#if defined ( PLUS_BROADCASTER )
 #include "peripheralBroadcaster.h"
#else
  #include "peripheral.h"
#endif

#include "epb_MmBp.h"
#include "ble_wechat_util.h"

#include "wechat.h"
#include "wechatservice.h"
#include "OSAL_Memory.h"
#include "stdio.h"

uint8 WechatData[20]={0x00};

uint8 wechatIndicate[WECHAT_INDICATE_LEN] = {0};

typedef struct{
	uint8 *data;
	int   len;
	uint8 step;
	uint8 status;
}wechat_Auth_t;

typedef struct{
	uint8 *data;
	uint32   len;
	uint8 step;
	uint8 status;	
	uint8 reqCmd;
}wechat_Req_t;

wechat_Auth_t auth_t;
wechat_Req_t wechatReq;


#define DEVICE_TYPE "gh_cb0aac41d4fd"
#define DEVICE_ID "dev2"
#define PROTO_VERSION 0x010002
#define AUTH_PROTO 1
#define AUTH_METHOD 2  //mac or md5
//uint8 mac_addr[6] = {0xB0, 0xB4, 0x48, 0xDC, 0xD5, 0xB6};

#define EAM_macNoEncrypt


#define MD5_TYPE_AND_ID 0x79A3B59E726AAC941559CCBB1DE42FDD

uint8 md5_type_and_id[16] = {0x79, 0xA3, 0xB5, 0x9E, 0x72, 0x6A, 0xAC, 0x94, 0x15, 0x59, 0xCC, 0xBB, 0x1D, 0xE4, 0x2F, 0xDD};//0x79A3B59E726AAC941559CCBB1DE42FDD

BaseRequest basReq = {NULL};;

//AuthRequest authReq = {&basReq, false, {NULL, 0}, PROTO_VERSION, AUTH_PROTO, AUTH_METHOD, false, {NULL, 0},  true, {mac_addr, 6}, false, {NULL, 0}, false, {NULL, 0}};


/***********************init*************************/
wechat_Auth_t init_t;

#define CHALLENAGE_LENGTH 4

uint8 challeange[CHALLENAGE_LENGTH] = {0x11,0x22,0x33,0x44}; 
//has challeange
InitRequest initReq = {&basReq,false, {NULL, 0},true, {challeange, CHALLENAGE_LENGTH}};

char aaaa[9] = { 1,2,3,4,5,6,7,8,9};	
/***********************send data*************************/
typedef struct
{
	uint8 m_magicCode[2];
	uint16 m_version;
	uint16 m_totalLength;
	uint16 m_cmdid;
	uint16 m_seq;
	uint16 m_errorCode;
}BlueDemoHead;

typedef struct
{
	uint8 m_magicCode[2];
	uint16 m_version;
	uint16 m_totalLength;
	uint16 m_cmdid;
	uint16 m_seq;
	uint16 m_errorCode;
}AllenHead;

typedef struct
{
	uint8 *data;
	uint16 len;
}AllenBody;


typedef struct 
{
	int cmd;
	epb_CString send_msg;
} message_info;

typedef enum
{
	sendTextReq = 0x01,
	sendTextResp = 0x1001,
	openLightPush = 0x2001,
	closeLightPush = 0x2002,
}BleDemo2CmdID;

 typedef struct 
{
	bool wechats_switch_state; //
	bool indication_state;
	bool auth_state;
	bool init_state;
	bool auth_send;
	bool init_send;
	unsigned short send_data_seq;
	unsigned short push_data_seq;
	unsigned short seq; 
}wechat_state;


wechat_Auth_t send_t;
wechat_state wechatState = {false, false, false, false, false, false,0,0,0};


#define MPBLEDEMO2_MAGICCODE_H 0xAA
#define MPBLEDEMO2_MAGICCODE_L 0xBB
#define MPBLEDEMO2_VERSION 0x06
//#define SEND_HELLO_WECHAT "Hello, WeChat!"
#define SEND_HELLO_WECHAT "Hello, WeChat!"


typedef struct
{
		uint8 *data;
		uint16 len;
		uint16 offset;
} data_info;

data_info g_rcv_data = {NULL, 0, 0};
typedef enum
{
	errorCodeUnpackAuthResp = 0x9990,
	errorCodeUnpackInitResp = 0x9991,
	errorCodeUnpackSendDataResp = 0x9992,
	errorCodeUnpackCtlCmdResp = 0x9993,
	errorCodeUnpackRecvDataPush = 0x9994,
	errorCodeUnpackSwitchViewPush = 0x9995,
	errorCodeUnpackSwitchBackgroundPush = 0x9996,
	errorCodeUnpackErrorDecode = 0x9997,
}mpbledemo2UnpackErrorCode;

data_handler *m_mpbledemo2_handler = NULL;

#define CMD_NULL 0
#define CMD_AUTH 1
#define CMD_INIT 2
#define CMD_SENDDAT 3

static uint8 selfEntity;


uint8 device_init();
uint8 device_SendData(char *data, uint8 data_len);
uint8* headTail(uint8 *src);


//#define CATCH_LOG



void getTaskId(uint8 taskId)
{
	selfEntity = taskId;
}

void WechatDisconnect()
{
	wechatState.auth_state = false;
	wechatState.init_state = false;
}

uint16 notify_Handle;  
extern uint16 otify_Handle;


int ble_wechat_indicate_data_chunk()
{
	
	GAPRole_GetParameter( GAPROLE_CONNHANDLE, &notify_Handle);	        //获得当前连接的句柄，Indicate发送数据使用 


	switch (wechatReq.reqCmd){

	case CMD_INIT:
		case CMD_AUTH:
		case CMD_SENDDAT:
		
			{
                            if ((wechatReq.len-wechatReq.step*20)>19)  //剩下发送的数据大于20个字节
                            {
                            
                                    memset(WechatData, 0x00, 20);   //清除数据
                                    memcpy(WechatData, (wechatReq.data+20*wechatReq.step), 20);   //复制下一段20个数据
                                    if (Wechat_Indicate( notify_Handle, WechatData, 20, selfEntity) == SUCCESS) //Indicate数据 	
                                            wechatReq.step++;       //发送20个字节成功，指针偏移
                                    else
                                    {
                                        wechatReq.reqCmd = CMD_NULL;    //发送失败，改变status
                                        osal_mem_free(wechatReq.data);  //osal_mem_free 释放分配的数组空间，注意只能释放osal_mem_alloc分配的空间
                                    }						
                            }
			    else if ((wechatReq.len%20) != 0)	//剩下发送的数据小于20个字节	
			    {
					wechatReq.reqCmd = CMD_NULL;      //改变status
					memset(WechatData, 0x00, 20);
					memcpy(WechatData, (wechatReq.data+20*wechatReq.step), (wechatReq.len%20));  //复制剩下不足20个字节
					Wechat_Indicate( notify_Handle, WechatData, 20, selfEntity); //Indicate数据
					osal_mem_free(wechatReq.data);    //释放内存空间
			    }
				else
				{
					wechatReq.reqCmd = CMD_NULL;      //改变status        
					osal_mem_free(wechatReq.data);    //释放内存空间
				}
				
			}
			break;
		
		default:
			break;
		
		}
	return 0;

}

void Wechat_Message()
{
	int error_code;
	//ble_gatts_evt_write_t * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
	int chunk_size = 0;
	if (1) 
	{
		if (g_rcv_data.len == 0)        //g_rcv_data.len=0表示上一次数据接收完成，这是一次新的数据
		{
			BpFixHead *fix_head = (BpFixHead *) wechatIndicate;     //获取新数据的包头
			g_rcv_data.len = ntohs(fix_head->nLength);              //获取新数据的包长
			g_rcv_data.offset = 0;          //数据偏移指针清零 
			g_rcv_data.data = (uint8 *)osal_mem_alloc(g_rcv_data.len);//分配数据内存
		}	
		chunk_size = g_rcv_data.len - g_rcv_data.offset;                //获得未接受数据的长度
		chunk_size = chunk_size < 20 ? chunk_size : 20;                 //判断未接收数据长度是否大于20        
		memcpy(g_rcv_data.data+g_rcv_data.offset, wechatIndicate, chunk_size);  //复制数据到data中
		g_rcv_data.offset += chunk_size;        //数据偏移指针偏移
		if (g_rcv_data.len <= g_rcv_data.offset)   //判断数据是否全部接受完成 
		{
			error_code = wechat_data_consume_func(g_rcv_data.data, g_rcv_data.len); //接受数据解包
			osal_mem_free(g_rcv_data.data);    //释放data的内存
			//wechat_error_chack(p_wcs, p_data_handler, error_code);
			g_rcv_data.len = 0;
			g_rcv_data.offset = 0;
			
		}	
	}
}





int wechat_data_consume_func(uint8 *data, uint32 len)
{

	BpFixHead *fix_head = (BpFixHead *)data;
	uint8 fix_head_len = sizeof(BpFixHead);
#ifdef CATCH_LOG
	printS("\r\n##Received data: ");
	uint8 *d = data;
	for(uint8 i=0;i<len;++i){
	printV(" ,",d[i], 16);}
	printV("\r\n CMDID: ", ntohs(fix_head->nCmdId), 10);
	printV("\r\n len: ", ntohs(fix_head->nLength), 10);
	printV("\r\n Seq: ",ntohs(fix_head->nSeq), 10);
#endif
	switch(ntohs(fix_head->nCmdId))
	{
		case ECI_none:
			{
			}
			break;
		case ECI_resp_auth:
			{
                        #ifdef DBG_UART
                          printf("resp_auth\r\n");
                        #endif
				AuthResponse* authResp;
				authResp = epb_unpack_auth_response(data+fix_head_len,len-fix_head_len);
                                #ifdef DBG_UART
                                printf("auth err_code:%d\r\n", (uint16)authResp->base_response->err_code);
                                #endif
			#ifdef CATCH_LOG
				printS("\r\n@@Received 'authResp'\r\n");
			#endif
				if(!authResp){return errorCodeUnpackAuthResp;}
			#ifdef CATCH_LOG
				printS("\r\n unpack 'authResp' success!\r\n");
			
			#endif
					//add by allen
						device_init();
				if(authResp->base_response)
				{
					if(authResp->base_response->err_code == 0)
					{
						wechatState.auth_state = true;

					}
					else
					{
					#ifdef CATCH_LOG
						printV("\r\n error code:", authResp->base_response->err_code, 16);
					#endif
						#ifdef CATCH_LOG
						if(authResp->base_response->has_err_msg)
						{
							//printV("\r\n base_response error msg:%s",authResp->base_response->err_msg.str, 16);	
						}
						#endif
						epb_unpack_auth_response_free(authResp);

				
						return authResp->base_response->err_code;
					}
				}
			#if defined EAM_md5AndAesEnrypt// get sessionkey
				if(authResp->aes_session_key.len)
				{
				#ifdef CATCH_LOG
					printf("\r\nsession_key:");
				#endif
					AES_Init(key);
					AES_Decrypt(session_key,authResp->aes_session_key.data,authResp->aes_session_key.len,key);
					#ifdef CATCH_LOG
					for(uint8 i = 0;i<16;i++)
					{
						printf(" 0x%02x",session_key[i]);	
					}
					#endif
				}
			#endif
				epb_unpack_auth_response_free(authResp);
			}
			break;
		case ECI_resp_sendData:
			{
                        #ifdef DGB_UART
			printf("resp_senddata\r\n");
                        #endif
		#ifdef CATCH_LOG
			printS("\r\n@@Received 'sendDataResp'\r\n");
		#endif
		#if defined EAM_md5AndAesEnrypt		
				uint32 length = len- fix_head_len;//
				uint8 *p = osal_mem_alloc (length);
				if(!p){if(data)osal_mem_free(data);data = NULL; return 0;}
				AES_Init(session_key);
				//
				AES_Decrypt(p,data+fix_head_len,len- fix_head_len,session_key);
				
				uint8 temp;
				temp = p[length - 1];//
				len = len - temp;//
				memcpy(data + fix_head_len, p ,length -temp);//
				if(p){osal_mem_free(p);p = NULL;}
		#endif	
			SendDataResponse *sendDataResp;
			sendDataResp = epb_unpack_send_data_response(data+fix_head_len,len-fix_head_len);
			if(!sendDataResp)
				{
					return errorCodeUnpackSendDataResp;
				}
		#ifdef CATCH_LOG
			BlueDemoHead *bledemohead = (BlueDemoHead*)sendDataResp->data.data;
			if(ntohs(bledemohead->m_cmdid) == sendTextResp)
				{
					//printV("\r\n received msg:0x",sendDataResp->data.data+sizeof(BlueDemoHead), 16);
				}
		#endif
				if(sendDataResp->base_response->err_code)
				{
					epb_unpack_send_data_response_free(sendDataResp);
					return sendDataResp->base_response->err_code;
				}
				epb_unpack_send_data_response_free(sendDataResp);
		}
			break;
		case ECI_resp_init:
			{
                          #ifdef DBG_UART
                          printf("resp_init\r\n");
                          #endif
		#ifdef CATCH_LOG
			printS("\r\n@@Received 'initResp'\r\n");
		#endif
		#if defined EAM_md5AndAesEnrypt		
				uint32 length = len- fix_head_len;//
				uint8 *p = osal_mem_alloc (length);	
				if(!p){if(data)osal_mem_free(data);data = NULL; return 0;}
				AES_Init(session_key);
				//
				AES_Decrypt(p,data+fix_head_len,len- fix_head_len,session_key);
				
				uint8 temp;
				temp = p[length - 1];//
				len = len - temp;//
				memcpy(data + fix_head_len, p ,length -temp);//
				if(p){osal_mem_free(p);p = NULL;}
		#endif		
			InitResponse *initResp = epb_unpack_init_response(data+fix_head_len, len-fix_head_len);
			if(!initResp)
			{
				return errorCodeUnpackInitResp;
			}
			#ifdef CATCH_LOG
				printS("\r\n unpack 'initResp' success!");
			#endif	
				if(initResp->base_response)
				{
					if(initResp->base_response->err_code == 0)
					{
						if(initResp->has_challeange_answer)
						{
						//	if(crc32(0,challeange,CHALLENAGE_LENGTH) == initResp->challeange_answer)
							{
								wechatState.init_state = true;
							}
						}
						else wechatState.init_state = true;
						wechatState.wechats_switch_state = true;
					}
					else
					{
					#ifdef CATCH_LOG
						printV("\r\n error code:%d",initResp->base_response->err_code, 16);
					#endif	
						if(initResp->base_response->has_err_msg)
						{
						#ifdef CATCH_LOG
							//printf("\r\n base_response error msg:%s",initResp->base_response->err_msg.str);
						#endif	
						}
						epb_unpack_init_response_free(initResp);
						return initResp->base_response->err_code;
					}
				}
			epb_unpack_init_response_free(initResp);
			//add by allen
			device_SendData(aaaa,9);
		}
			break;
		case ECI_push_recvData:
			{
			#if defined EAM_md5AndAesEnrypt
				uint32 length = len- fix_head_len;//
				uint8 *p = osal_mem_alloc (length);
				if(!p){if(data)osal_mem_free(data); data =NULL; return 0;}
				AES_Init(session_key);
				//
				AES_Decrypt(p,data+fix_head_len,len- fix_head_len,session_key);
				
				uint8 temp;
				temp = p[length - 1];//
				len = len - temp;//
				memcpy(data + fix_head_len, p ,length -temp);//
				if(p){osal_mem_free(p);p = NULL;}
			#endif
		RecvDataPush *recvDatPush;
			
		recvDatPush = epb_unpack_recv_data_push(data+fix_head_len, len-fix_head_len);
	#ifdef CATCH_LOG
		printS("\r\n@@Received 'recvDataPush'\r\n");
	#endif
		if(!recvDatPush)
		{
			return errorCodeUnpackRecvDataPush;
		}
	#ifdef CATCH_LOG
		printS("\r\n unpack the 'recvDataPush' successfully! \r\n");
		if(recvDatPush->base_push == NULL)
		{
			printS("\r\n recvDatPush->base_push is NULL! \r\n");
		}
		else 
		{
			printS("\r\n recvDatPush->base_push is not NULL! \r\n");
		}
		printV("\r\n recvDatPush->data.len: ",recvDatPush->data.len, 10);
		printS("\r\n recvDatPush->data.data:  ");
		const uint8 *d = recvDatPush->data.data;
		for(uint8 i=0;i<recvDatPush->data.len;++i){
		  printV(" ",d[i],10);}
		if(recvDatPush->has_type)
		{
			printS("\r\n recvDatPush has type! \r\n");
			printV("\r\n type:",recvDatPush->type, 16);
		}
	#endif	
		BlueDemoHead *bledemohead = (BlueDemoHead*)recvDatPush->data.data;
	#ifdef CATCH_LOG
		printV("\r\n magicCode: 0x",bledemohead->m_magicCode[0], 16);
		printV(" %x",bledemohead->m_magicCode[1], 16);
		printV("\r\n version: 0x",ntohs(bledemohead->m_version), 16);
		printV("\r\n totalLength: 0x",ntohs(bledemohead->m_totalLength), 16);
		printV("\r\n cmdid: 0x",ntohs(bledemohead->m_cmdid ), 16);
		printV("\r\n errorCode: 0x",ntohs(bledemohead->m_errorCode), 16);
	#endif	
		if(ntohs(bledemohead->m_cmdid ) == openLightPush)
		{
	#ifdef CATCH_LOG
			printS("\r\n light on!! ");
	#endif
			
			//light_on(MPBLEDEMO2_LIGHT);
			
			
		}
		else if(ntohs(bledemohead->m_cmdid )  == closeLightPush)
		{
	#ifdef CATCH_LOG
				printS("\r\n light off!! ");
	#endif
		
				//light_off(MPBLEDEMO2_LIGHT);
				
		}
		epb_unpack_recv_data_push_free(recvDatPush);
		wechatState.push_data_seq++;
	}
			break;
		case ECI_push_switchView:
			{
		
				wechatState.wechats_switch_state = !wechatState.wechats_switch_state;
		#ifdef CATCH_LOG
			printS("\r\n@@Received 'switchViewPush'\r\n");
		#endif
		#if defined EAM_md5AndAesEnrypt		
				uint32 length = len- fix_head_len;//
				uint8 *p = osal_mem_alloc (length);
				if(!p){if(data)osal_mem_free(data);data = NULL; return 0;}
				AES_Init(session_key);
				//
				AES_Decrypt(p,data+fix_head_len,len- fix_head_len,session_key);
				
				uint8 temp;
				temp = p[length - 1];//
				len = len - temp;//
				memcpy(data + fix_head_len, p ,length -temp);//
				if(p){osal_mem_free(p);p = NULL;}
		#endif		
			SwitchViewPush *swichViewPush;
			swichViewPush = epb_unpack_switch_view_push(data+fix_head_len,len-fix_head_len);
			if(!swichViewPush)
			{
				return errorCodeUnpackSwitchViewPush;
			}
			epb_unpack_switch_view_push_free(swichViewPush);
		}
			break;
		case ECI_push_switchBackgroud:
			{
		#ifdef CATCH_LOG
			printS("\r\n@@Received 'switchBackgroudPush'\r\n");
		#endif
		#if defined EAM_md5AndAesEnrypt
				uint32 length = len- fix_head_len;//
				uint8 *p = osal_mem_alloc (length);
				if(!p){if(data)osal_mem_free(data);data = NULL;  return 0;}
				AES_Init(session_key);
				//
				AES_Decrypt(p,data+fix_head_len,len- fix_head_len,session_key);
				uint8 temp;
				temp = p[length - 1];//
				len = len - temp;//
				memcpy(data + fix_head_len, p ,length -temp);//
				if(data){osal_mem_free(p);p = NULL;}
		#endif
			SwitchBackgroudPush *switchBackgroundPush = epb_unpack_switch_backgroud_push(data+fix_head_len,len-fix_head_len);
			if(! switchBackgroundPush)
			{
				return errorCodeUnpackSwitchBackgroundPush;
			}	
			epb_unpack_switch_backgroud_push_free(switchBackgroundPush);
		}
			break;
		case ECI_err_decode:
			break;
		default:
			{
	#ifdef CATCH_LOG
		//printS("\r\n !!ERROR CMDID:%d",ntohs(fix_head->nCmdId));
	#endif
		}
			break;
	}
	return 0;
}


void wechat_data_produce_func(void *args, uint8 **r_data, uint32 *r_len)
{
		
		static uint16 bleDemoHeadLen = sizeof(BlueDemoHead);
		message_info *info = (message_info *)args;
		BaseRequest basReq = {NULL};
		static uint8 fix_head_len = sizeof(BpFixHead);
		BpFixHead fix_head = {0xFE, 1, 0, htons(ECI_req_auth), 0};
		wechatState.seq++;
		switch (info->cmd)
		{
		case CMD_AUTH:
			{
			#if defined EAM_md5AndAesEnrypt
				uint8 deviceid[] = DEVICE_ID;
				static uint32 seq = 0x00000001;//
				uint32 ran = 0x11223344;//为了方便起见这里放了一个固定值做为随机数，在使用时请自行生成随机数。
				ran = t_htonl(ran);
				seq = t_htonl(seq);
				uint8 id_len = strlen(DEVICE_ID);
				uint8* data = osal_mem_alloc(id_len+8);
				if(!data){return;}
				memcpy(data,deviceid,id_len);
				memcpy(data+id_len,(uint8*)&ran,4);
				memcpy(data+id_len+4,(uint8*)&seq,4);
				uint32 crc = crc32(0, data, id_len+8);
				crc = t_htonl(crc);
				memset(data,0x00,id_len+8);
				memcpy(data,(uint8*)&ran,4);
				memcpy(data+4,(uint8*)&seq,4);
				memcpy(data+8,(uint8*)&crc,4);	
				uint8 CipherText[16];
				AES_Init(key);
				AES_Encrypt_PKCS7 (data, CipherText, 12, key);
				if(data){osal_mem_free(data);data = NULL;}
				AuthRequest authReq = {&basReq, true,{md5_type_and_id, MD5_TYPE_AND_ID_LENGTH}, PROTO_VERSION, AUTH_PROTO, (EmAuthMethod)AUTH_METHOD, true ,{CipherText, CIPHER_TEXT_LENGTH}, false, {NULL, 0}, false, {NULL, 0}, false, {NULL, 0},true,{DEVICE_ID,sizeof(DEVICE_ID)}};
				seq++;
			#endif
				
			#if defined EAM_macNoEncrypt
				static uint8 mac_address[B_ADDR_LEN];
				GAPRole_GetParameter(GAPROLE_BD_ADDR, mac_address);
				headTail(mac_address);
				AuthRequest authReq = {&basReq, false,{NULL, 0}, PROTO_VERSION, AUTH_PROTO, (EmAuthMethod)AUTH_METHOD, false,{NULL, 0}, true, {mac_address, B_ADDR_LEN}, false, {NULL, 0}, false, {NULL, 0},true,{DEVICE_ID,sizeof(DEVICE_ID)}};
			#endif
				
			#if defined EAM_md5AndNoEnrypt
				AuthRequest authReq = {&basReq, true,{md5_type_and_id, MD5_TYPE_AND_ID_LENGTH}, PROTO_VERSION, (EmAuthMethod)AUTH_PROTO, (EmAuthMethod)AUTH_METHOD, false ,{NULL, 0}, false, {NULL, 0}, false, {NULL, 0}, false, {NULL, 0},true,{DEVICE_ID,sizeof(DEVICE_ID)}};
			#endif
				*r_len = epb_auth_request_pack_size(&authReq) + fix_head_len;
				*r_data = (uint8 *)osal_mem_alloc(*r_len);
				if(!(*r_data)){return;}
				if(epb_pack_auth_request(&authReq, *r_data+fix_head_len, *r_len-fix_head_len)<0)
				{
					*r_data = NULL;
					return;
				}
				fix_head.nCmdId = htons(ECI_req_auth);
				fix_head.nLength = htons(*r_len);
				fix_head.nSeq = htons(wechatState.seq);
				memcpy(*r_data, &fix_head, fix_head_len);
				return ;
		}

		case CMD_INIT:
			{
				//has challeange
				InitRequest initReq = {&basReq,false, {NULL, 0},true, {challeange, CHALLENAGE_LENGTH}};
				*r_len = epb_init_request_pack_size(&initReq) + fix_head_len;
			#if defined EAM_md5AndAesEnrypt
				uint8 length = *r_len;				
				uint8 *p = osal_mem_alloc(AES_get_length( *r_len-fix_head_len));
				if(!p){return;}
				*r_len = AES_get_length( *r_len-fix_head_len)+fix_head_len;
			#endif
			//pack data
			    *r_data = (uint8 *)osal_mem_alloc(*r_len);
				if(!(*r_data)){return;}
				if(epb_pack_init_request(&initReq, *r_data+fix_head_len, *r_len-fix_head_len)<0)
				{*r_data = NULL;return;}
				//encrypt body
			#if defined EAM_md5AndAesEnrypt
				AES_Init(session_key);
				AES_Encrypt_PKCS7(*r_data+fix_head_len,p,length-fix_head_len,session_key);//原始数据长度
				memcpy(*r_data + fix_head_len, p, *r_len-fix_head_len);
				if(p)osal_mem_free(p);
			#endif
				fix_head.nCmdId = htons(ECI_req_init);
				fix_head.nLength = htons(*r_len);
				fix_head.nSeq = htons(wechatState.seq);
				memcpy(*r_data, &fix_head, fix_head_len);
				return  ;
		}
#if 1
		case CMD_SENDDAT:
			{
                          #ifdef DBG_UART
                          printf("cmd_senddata\r\n");
                          #endif
			#ifdef CATCH_LOG
				//printS(info->send_msg.str);
			#endif
				BlueDemoHead  *bleDemoHead = (BlueDemoHead*)osal_mem_alloc(bleDemoHeadLen+info->send_msg.len);
				if(!bleDemoHead){return;}
				bleDemoHead->m_magicCode[0] = MPBLEDEMO2_MAGICCODE_H;
				bleDemoHead->m_magicCode[1] = MPBLEDEMO2_MAGICCODE_L;
				bleDemoHead->m_version = htons( MPBLEDEMO2_VERSION);
				bleDemoHead->m_totalLength = htons(bleDemoHeadLen + info->send_msg.len);
				bleDemoHead->m_cmdid = htons(sendTextReq);
				bleDemoHead->m_seq = htons(wechatState.seq);
				bleDemoHead->m_errorCode = 0;	
				/*connect body and head.*/
				/*turn to uint8* befort offset.*/
				memcpy((uint8*)bleDemoHead+bleDemoHeadLen, info->send_msg.str, info-> send_msg.len);			
				SendDataRequest sendDatReq = {&basReq, {(uint8*) bleDemoHead, (bleDemoHeadLen + info->send_msg.len)}, true, (EmDeviceDataType)10001};
                                #ifdef DBG_UART
                                printf("sendDatReq->type :%d\r\n", sendDatReq.type);
                                #endif
				*r_len = epb_send_data_request_pack_size(&sendDatReq) + fix_head_len;
			#if defined EAM_md5AndAesEnrypt
				uint16 length = *r_len;
				uint8 *p = osal_mem_alloc(AES_get_length( *r_len-fix_head_len));
				if(!p){return;}
				*r_len = AES_get_length( *r_len-fix_head_len)+fix_head_len;
			#endif
				*r_data = (uint8 *)osal_mem_alloc(*r_len);
				if(!(*r_data)){return;}
				if(epb_pack_send_data_request(&sendDatReq, *r_data+fix_head_len, *r_len-fix_head_len)<0)
				{
					*r_data = NULL;
				#if defined EAM_md5AndAesEnrypt
					if(p){osal_mem_free(p);
					p = NULL;}
					#endif
					return;
				}
			#if defined EAM_md5AndAesEnrypt
				//encrypt body
				AES_Init(session_key);
				AES_Encrypt_PKCS7(*r_data+fix_head_len,p,length-fix_head_len,session_key);//原始数据长度
				memcpy(*r_data + fix_head_len, p, *r_len-fix_head_len);
				if(p){osal_mem_free(p); p = NULL;}
			#endif
				fix_head.nCmdId = htons(ECI_req_sendData);
				fix_head.nLength = htons(*r_len);
				fix_head.nSeq = htons(wechatState.seq);
				memcpy(*r_data, &fix_head, fix_head_len);
				if(bleDemoHead){osal_mem_free(bleDemoHead);bleDemoHead = NULL;}
				
				//return;
			#ifdef  DBG_UART
				printf("send data:");
				uint8 *d = *r_data;
				for(uint8 i=0;i<*r_len;++i){
				printf("%d,",d[i]);}
				BpFixHead *fix_head = (BpFixHead *)*r_data;
				printf("\r\n CMDID: %d",ntohs(fix_head->nCmdId));
				printf("\r\n len: %d", ntohs(fix_head->nLength ));
				printf("\r\n Seq: %d", ntohs(fix_head->nSeq));
			#endif
			wechatState.send_data_seq++;
				
				return ;
		}
		#endif
	}	
}


uint8 device_auth()
{	
        #ifdef DBG_UART
        printf("auth start\r\n");
        #endif
	message_info test;	//定义message数据
	test.cmd = CMD_AUTH;    //设置message的status
	wechatReq.reqCmd = CMD_AUTH;    //设置req的status
	//ARGS_ITEM_SET(mpbledemo2_info, m_mpbledemo2_handler->m_data_produce_args, cmd, CMD_AUTH);	

	wechat_data_produce_func((void *)&test, &wechatReq.data, &wechatReq.len);  //打包数据

	//m_mpbledemo2_handler->m_data_produce_func(m_mpbledemo2_handler->m_data_produce_args, &data, &len);
	if(wechatReq.data == NULL)
	{
		return 0;//errorCodeProduce;
	}
        //发送数据	
        wechatReq.step = 0;
	ble_wechat_indicate_data_chunk();

#ifdef CATCH_LOG
	printS("\r\n auth send! ");
#endif
#ifdef CATCH_LOG
	printS("\r\n##send data: ");
	uint8 *d = wechatReq.data;
	for(uint8 i=0;i<wechatReq.len;++i){
	   printV(" ,",d[i], 16);}
	BpFixHead *fix_head = (BpFixHead *)wechatReq.data;
	printV("\r\n CMDID: ", ntohs(fix_head->nCmdId), 10);
	printV("\r\n len: ", ntohs(fix_head->nLength ), 10);
	printV("\r\n Seq: ", ntohs(fix_head->nSeq), 10);
#endif

	return 0;
}


uint8 device_init()
{
	//ARGS_ITEM_SET(mpbledemo2_info, m_mpbledemo2_handler->m_data_produce_args, cmd, CMD_INIT);
        #ifdef DBG_UART
        printf("init start\r\n");
        #endif
	message_info test;	//定义wechat的数据
        test.cmd = CMD_INIT;    //修改status的状态
	wechatReq.reqCmd = CMD_INIT;    //设置req的status
	wechat_data_produce_func((void *)&test, &wechatReq.data, &wechatReq.len);   //打包数据
	
	//m_mpbledemo2_handler->m_data_produce_func(m_mpbledemo2_handler->m_data_produce_args, &data, &len);
	if(wechatReq.data == NULL)
	{
		return 0;//errorCodeProduce;
	}
	//发送数据
	wechatReq.step = 0;
	ble_wechat_indicate_data_chunk();

#ifdef CATCH_LOG
	printS("\r\n##send data: ");
	uint8 *d = wechatReq.data;
	for(uint8 i=0;i<wechatReq.len;++i){
	   printV(" ,",d[i], 16);}
	BpFixHead *fix_head = (BpFixHead *)wechatReq.data;
	printV("\r\n CMDID: ", ntohs(fix_head->nCmdId), 10);
	printV("\r\n len: ", ntohs(fix_head->nLength ), 10);
	printV("\r\n Seq: ", ntohs(fix_head->nSeq), 10);
#endif
	return 0;
}

uint8 device_SendData(char *data, uint8 data_len)
{

    if (wechatState.init_state == false)
    {
    	device_auth();
		return 1;
    }
	//char data[6] = {1,23,35,255,55,9};
	
	message_info test;	
	test.cmd = CMD_SENDDAT;
	wechatReq.reqCmd = CMD_SENDDAT;
	test.send_msg.str=data;
	test.send_msg.len=data_len;
	
	wechat_data_produce_func((void *)&test, &wechatReq.data, &wechatReq.len); //数据打包
	
	if(wechatReq.data == NULL)
	{
		return 0;//errorCodeProduce;
	}
        //发送数据
	wechatReq.step = 0;  //清除发送数据的指针
	ble_wechat_indicate_data_chunk();
	//ble_wechat_indicate_data(p_wcs, m_mpbledemo2_handler, data, len);

	#ifdef CATCH_LOG
		printS("\r\n##send data: ");
		uint8 *d = wechatReq.data;
		for(uint8 i=0;i<wechatReq.len;++i){
		   printV(" ,",d[i], 16);}
		BpFixHead *fix_head = (BpFixHead *)wechatReq.data;
		printV("\r\n CMDID: ", ntohs(fix_head->nCmdId), 10);
		printV("\r\n len: ", ntohs(fix_head->nLength ), 10);
		printV("\r\n Seq: ", ntohs(fix_head->nSeq), 10);
	#endif
	return 0;
}


uint8* headTail(uint8 *src)
{
	uint8 data;
	uint8 len = 6;//strlen(src);//(sizeof(src)/(sizeof(src[0])));
	//printV("mac len=", len, 10);
	for (uint8 i=0; i<len/2; i++)
	{	
		data=src[i];
		src[i]=src[len-i-1]; 
		src[len-i-1]=data;
	}
	return src;
}


