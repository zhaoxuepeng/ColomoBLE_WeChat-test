/**************************************************************************************************
  Filename:       ColomoBLE.c
  Revised:        $Date: 2015/6/16 $
  Revision:       $Revision: 0 $

  Description:    This file contains the ColomoBLE application
                  for use with the CC2541 Bluetooth Low Energy Protocol Stack.

  Copyright 2015 - 2016 QijiTek. All rights reserved.
**************************************************************************************************/

/*********************************************************************
 * INCLUDES
 */

#include "bcomdef.h"\
//#include "OSAL.h"
#include "OSAL_PwrMgr.h"
#include "OSAL_Clock.h"

#include "OnBoard.h"
#include "hal_adc.h"
#include "hal_led.h"
#include "hal_key.h"
#include "hal_lcd.h"
#include "bsp_ruby_cap.h"
#include "bsp_ruby_res.h"
#include "bsp_ruby_lum.h"
#include "bsp_ruby_res.h"
#include "bsp_ruby_flash.h"
#include "bsp_power.h"
#include "bsp_color_9300.h"
#include "bsp_cap_io.h"
#include "bsp_acc.h"
#include "hal_i2c.h"
#include "hal_drivers.h"
#include "hal_flash.h"
#include "bsp_smart_cover.h"
#include "bsp_ruby_motor.h"
#include "bsp_ruby_lum.h"
#include "bsp_ruby_timer.h"
#include "gatt.h"
#include "osal_snv.h"

#include "hci.h"

#include "gapgattserver.h"
#include "gattservapp.h"
#include "devinfoservice.h"
#include "ColomoService.h"

#if defined ( PLUS_BROADCASTER )
  #include "peripheralBroadcaster.h"
#else
  #include "peripheral.h"
#endif

#include "gapbondmgr.h"

#include "ColomoBLE.h"

#if defined FEATURE_OAD
  #include "oad.h"
  #include "oad_target.h"
#endif
#ifdef WECHAT
#include "WeChat.h"
#include "WeChatService.h"
#endif

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

// How often to perform periodic event
#define SBP_PERIODIC_EVT_PERIOD               5000

// What is the advertising interval when device is discoverable (units of 625us, 1600=1s)
#define DEFAULT_ADVERTISING_INTERVAL          160
//#define DEFAULT_ADVERTISING_INTERVAL          1600

#if (defined DISCOVERABLE_LIMITED)
  // Limited discoverable mode advertises for 10s, and then stops ( for ColomoBLE, we need limited mode)
  // COLOMOBLE_ADVERTISING_TIMEOUT determines the lasting of one advertising event
  // gapRole_AdvertOffTime determines the interval of two advertising events
  #define COLOMOBLE_DISCOVERABLE_MODE         GAP_ADTYPE_FLAGS_LIMITED
#else
  // General discoverable mode advertises indefinitely
  #define COLOMOBLE_DISCOVERABLE_MODE         GAP_ADTYPE_FLAGS_GENERAL
#endif
#define COLOMOBLE_ADVERTISING_TIMEOUT         100
#define DEFAULT_DISCOVERABLE_MODE             GAP_ADTYPE_FLAGS_GENERAL

// Minimum connection interval (units of 1.25ms, 80=100ms) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_MIN_CONN_INTERVAL     16

// Maximum connection interval (units of 1.25ms, 800=1000ms) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_MAX_CONN_INTERVAL     16

// Slave latency to use if automatic parameter update request is enabled
#define DEFAULT_DESIRED_SLAVE_LATENCY         3

// Supervision timeout value (units of 10ms, 1000=10s) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_CONN_TIMEOUT          1000

// Whether to enable automatic parameter update request when a connection is formed
#define DEFAULT_ENABLE_UPDATE_REQUEST         TRUE

// Connection Pause Peripheral time value (in seconds)
#define DEFAULT_CONN_PAUSE_PERIPHERAL         6

// Company Identifier: Texas Instruments Inc. (13)
#define TI_COMPANY_ID                         0x000D

#define INVALID_CONNHANDLE                    0xFFFF

// Length of bd addr as a string
#define B_ADDR_STR_LEN                        15

#if defined ( PLUS_BROADCASTER )
  #define ADV_IN_CONN_WAIT                    500 // delay 500 ms
#endif

/*********************************************************************
 * TYPEDEFS
 */
#define HISTORY_NULL    0x00
#define HISTORY_HEAD    0x01
#define HISTORY_DATA    0x02
#define HISTORY_END     0x04

#define BAND_CONNECTED       1
#define WAITING         2
#define PAIR            3

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */
/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
//读取flash值和出厂检测
static uint8 factory_cnt;
static uint8 Color_Buf[4];
static float ColorPara;
static uint16 ColorValue, ColorMax, ColorMid, ColorMin;

#ifdef WECHAT
static uint8 charValueRead[WECHAT_READ_LEN];
#endif

static uint8 ColomoBLE_TaskID;   // Task ID for internal task/event processing

static gaprole_States_t gapProfileState = GAPROLE_INIT;

static uint8 GapRoleState;
// static uint8 ClmHeartBeat; // for ble advertising

static uint8 ClmState;
static uint8 ClmPairTimeOut;
static uint8 ClmPoffTimeOut;

static uint16 initial_cap_io_cnt = 0;
static uint8  skin_detect_cnt = 0;
static uint16 skin_array[4];
static uint16 skin_mois;

//the valve of voltage
static uint16 valve = 4200;

UTCTime LastRecordTime = 0;

// GAP - SCAN RSP data (max size = 31 bytes)
static uint8 scanRspData[] =
{
  // complete name
  0x08,   // length of this data
  GAP_ADTYPE_LOCAL_NAME_COMPLETE,
  'A','l','p','h','a','c','e',

  // connection interval range
  0x05,   // length of this data
  GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGE,
  LO_UINT16( DEFAULT_DESIRED_MIN_CONN_INTERVAL ),   // 100ms
  HI_UINT16( DEFAULT_DESIRED_MIN_CONN_INTERVAL ),
  LO_UINT16( DEFAULT_DESIRED_MAX_CONN_INTERVAL ),   // 1s
  HI_UINT16( DEFAULT_DESIRED_MAX_CONN_INTERVAL ),

  // Tx power level
  0x02,   // length of this data
  GAP_ADTYPE_POWER_LEVEL,
  0       // 0dBm
};

// GAP - Advertisement data (max size = 31 bytes, though this is
// best kept short to conserve power while advertisting)
static uint8 advertData[] =
{
  // Flags; this sets the device to use limited discoverable
  // mode (advertises for 30 seconds at a time) instead of general
  // discoverable mode (advertises indefinitely)
  0x02,   // length of this data
  GAP_ADTYPE_FLAGS,
  COLOMOBLE_DISCOVERABLE_MODE | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,
  
  
  // service UUID, to notify central devices what services are included
  // in this peripheral
  0x03,   // length of this data
  GAP_ADTYPE_16BIT_MORE,      // some of the UUID's, but not all
  LO_UINT16( WECHAT_SERV_UUID ),
  HI_UINT16( WECHAT_SERV_UUID ),
  /**/
  // 8 byte broadcast of the data for moisture, lumen, temperature and fertilizer
  0x11,   // length of this data including the data type byte
  GAP_ADTYPE_MANUFACTURER_SPECIFIC,      // manufacturer specific advertisement data type
  'M',
  'I',
  'G',
  0,   // [8]: Advertising Heart Beat
  0,   // [9]: Moisture L
  0,   // [10]:Moisture H
  0,   // [11]:Lumen L
  0,   // [12]:Lumen H
  0,   // [13]:Temperature L
  0,   // [14]:Temperature H
  0,   // [15]:Fertilizer L
  0,   // [16]:Fertilizer H
  0,    // [17]:Battery Voltage
  0,
  0,
  0
};

// GAP GATT Attributes
static uint8  attDeviceName[GAP_DEVICE_NAME_LEN] = "Alphace";
static uint16 gapConnHandle;

char wechat_pBuf[6] = {0};
/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void Indication_ProcessGattMsg( gattMsgEvent_t *pMsg );  
static void ColomoBLE_ProcessOSALMsg( osal_event_hdr_t *pMsg );
static void peripheralStateNotificationCB( gaprole_States_t newState );
// static void performPeriodicTask( void );
static void ColomoProfileChangeCB( uint8 paramID );

#ifdef WECHAT
static void WeChatAttrCB( uint8 attrParamID );
#endif
/*
static void ColomoPasscodeCB(uint8 *deviceAddr,uint16 connectionHandle,uint8 uiInputs,uint8 uiOutputs );
static void ColomoPairStateCB( uint16 connectionHandle, uint8 state, uint8 status );
*/
// static void ColomoProcessRawData( uint16 red, uint16 green, uint16 blue, uint16 lumen, uint8 mois, uint16 fat, uint16 ela, uint8* pBuf);
//----------------------------------------------------------------------------//

#if (defined DBG_UART) && (DBG_UART == TRUE)
static char *bdAddr2Str ( uint8 *pAddr );
#endif

/*********************************************************************
 * PROFILE CALLBACKS
 */

// GAP Role Callbacks
static gapRolesCBs_t ColomoBLE_PeripheralCBs =
{
  peripheralStateNotificationCB,  // Profile State Change Callbacks
  NULL                            // When a valid RSSI is read from controller (not used by application)
};

// GAP Bond Manager Callbacks
static gapBondCBs_t ColomoBLE_BondMgrCBs =
{
  NULL,
  NULL
  //ColomoPasscodeCB,           // Passcode callback
  //ColomoPairStateCB           // Pairing / Bonding state Callback   
};

// Simple GATT Profile Callbacks
static colomoProfileCBs_t ColomoBLE_SimpleProfileCBs =
{
  ColomoProfileChangeCB
};

// wechat Profile Callbacks
#ifdef WECHAT
static wechatCBs_t WeChat_ProfileCBs =
{
  WeChatAttrCB  // Whenever the Link Loss Alert attribute changes
};
#endif
/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      ColomoBLE_Init
 *
 * @brief   Initialization function for the Colomo BLE Peripheral App Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notificaiton ... ).
 *
 * @param   task_id - the ID assigned by OSAL.  This ID should be
 *                    used to send messages and set timers.
 *
 * @return  none
 */
void ColomoBLE_Init( uint8 task_id )
{
  ColomoBLE_TaskID = task_id;

  //按键设备的识别标志
  advertData[16]= 0xfe;
  advertData[17]= 0x01;
  advertData[18]= 0x01;
  //将本机的MAC添加到广播数据中
  advertData[19]= *((uint8*)0x7813);
  advertData[20]= *((uint8*)0x7812);
  advertData[21]= *((uint8*)0x7811);
  advertData[22]= *((uint8*)0x7810);
  advertData[23]= *((uint8*)0x780F);
  advertData[24]= *((uint8*)0x780E);
  
#ifdef WECHAT
  charValueRead[0] = *((uint8*)0x7813);
  charValueRead[1] = *((uint8*)0x7812);
  charValueRead[2] = *((uint8*)0x7811);
  charValueRead[3] = *((uint8*)0x7810);
  charValueRead[4] = *((uint8*)0x780F);
  charValueRead[5] = *((uint8*)0x780E);
#endif
  
  GAPRole_SetParameter( GAPROLE_ADVERT_DATA, sizeof(advertData ), advertData );
#ifdef WECHAT
  Wechat_SetParameter(WECHAT_READ, WECHAT_READ_LEN, charValueRead );
#endif
#if defined( DBG_UART )
  printf("Hello, I am Tuliface.\r\n");
#endif
  // ClmHeartBeat = 0;
  ClmState = CLM_STATE_POFF;
  ClmPairTimeOut = 0xff;
  ClmPoffTimeOut = 0xff;
  
  //读取flash中color的值
  
  osal_snv_read(SNV_COLOR_ID, sizeof(Color_Buf), Color_Buf);
  ColorMax = (((uint16)Color_Buf[1]) << 8) + Color_Buf[0];
  ColorMin = (((uint16)Color_Buf[3]) << 8) + Color_Buf[2];
  ColorPara = 1000 / (float)(ColorMax - ColorMin);
  osal_snv_read(SNV_MOTOR_ID, sizeof(pwm_change_value), &pwm_change_value);
  #if (defined DBG_UART)
    printf("ColorPara : %f\r\n", ColorPara);
    printf("ColorMax : %d\r\n", ColorMax);
    printf("ColorMin : %d\r\n", ColorMin);
    printf("pwm_change_value: %d\r\n", pwm_change_value);
  #endif
      
  // Setup the GAP
  VOID GAP_SetParamValue( TGAP_CONN_PAUSE_PERIPHERAL, DEFAULT_CONN_PAUSE_PERIPHERAL );
  VOID GAP_SetParamValue( TGAP_LIM_ADV_TIMEOUT,       COLOMOBLE_ADVERTISING_TIMEOUT ); // set ColomoBLE advertising timeout
  // Setup the GAP Peripheral Role Profile
  {
    // Do not advertising when initialization
#if (defined DISCOVERABLE_INIT)
    uint8 initial_advertising_enable = TRUE;
#else
    uint8 initial_advertising_enable = FALSE;
#endif

    // By setting this to zero, the device will go into the waiting state after
    // being discoverable for 10 second, and will not being advertising again
    // until the enabler is set back to TRUE
    uint16 gapRole_AdvertOffTime = 0;

    uint8  enable_update_request = DEFAULT_ENABLE_UPDATE_REQUEST;
    uint16 desired_min_interval  = DEFAULT_DESIRED_MIN_CONN_INTERVAL;
    uint16 desired_max_interval  = DEFAULT_DESIRED_MAX_CONN_INTERVAL;
    uint16 desired_slave_latency = DEFAULT_DESIRED_SLAVE_LATENCY;
    uint16 desired_conn_timeout  = DEFAULT_DESIRED_CONN_TIMEOUT;

    // Set the GAP Role Parameters
    GAPRole_SetParameter( GAPROLE_ADVERT_ENABLED, sizeof( uint8 ), &initial_advertising_enable );
    GAPRole_SetParameter( GAPROLE_ADVERT_OFF_TIME, sizeof( uint16 ), &gapRole_AdvertOffTime );

    GAPRole_SetParameter( GAPROLE_SCAN_RSP_DATA, sizeof ( scanRspData ), scanRspData );
    GAPRole_SetParameter( GAPROLE_ADVERT_DATA, sizeof( advertData ), advertData );

    GAPRole_SetParameter( GAPROLE_PARAM_UPDATE_ENABLE, sizeof( uint8 ), &enable_update_request );
    GAPRole_SetParameter( GAPROLE_MIN_CONN_INTERVAL, sizeof( uint16 ), &desired_min_interval );
    GAPRole_SetParameter( GAPROLE_MAX_CONN_INTERVAL, sizeof( uint16 ), &desired_max_interval );
    GAPRole_SetParameter( GAPROLE_SLAVE_LATENCY, sizeof( uint16 ), &desired_slave_latency );
    GAPRole_SetParameter( GAPROLE_TIMEOUT_MULTIPLIER, sizeof( uint16 ), &desired_conn_timeout );
  }

  // Set the GAP Characteristics
  GGS_SetParameter( GGS_DEVICE_NAME_ATT, GAP_DEVICE_NAME_LEN, attDeviceName );

  // Set advertising interval
  {
    uint16 advInt = DEFAULT_ADVERTISING_INTERVAL;

    GAP_SetParamValue( TGAP_LIM_DISC_ADV_INT_MIN, advInt );
    GAP_SetParamValue( TGAP_LIM_DISC_ADV_INT_MAX, advInt );
    GAP_SetParamValue( TGAP_GEN_DISC_ADV_INT_MIN, advInt );
    GAP_SetParamValue( TGAP_GEN_DISC_ADV_INT_MAX, advInt );
  }

  // Setup the GAP Bond Manager
  {
    uint32 passkey = 0; // passkey "000000"
    uint8 pairMode = GAPBOND_PAIRING_MODE_WAIT_FOR_REQ;
    uint8 mitm = TRUE;
    uint8 ioCap = GAPBOND_IO_CAP_DISPLAY_ONLY;
    uint8 bonding = TRUE;
    GAPBondMgr_SetParameter( GAPBOND_DEFAULT_PASSCODE, sizeof ( uint32 ), &passkey );
    GAPBondMgr_SetParameter( GAPBOND_PAIRING_MODE, sizeof ( uint8 ), &pairMode );
    GAPBondMgr_SetParameter( GAPBOND_MITM_PROTECTION, sizeof ( uint8 ), &mitm );
    GAPBondMgr_SetParameter( GAPBOND_IO_CAPABILITIES, sizeof ( uint8 ), &ioCap );
    GAPBondMgr_SetParameter( GAPBOND_BONDING_ENABLED, sizeof ( uint8 ), &bonding );
  }

  // Initialize GATT attributes
  GGS_AddService( GATT_ALL_SERVICES );            // GAP
  GATTServApp_AddService( GATT_ALL_SERVICES );    // GATT attributes
  DevInfo_AddService();                           // Device Information Service
  colomo_AddService(GATT_ALL_SERVICES);           // Colomo GATT Profile
#ifdef WECHAT
  Wechat_AddService(GATT_ALL_SERVICES);      // WeChat GATT Profile
#endif
#if defined FEATURE_OAD
  VOID OADTarget_AddService(); // OAD Profile
#endif
  // Start the wechat Profile
#ifdef WECHAT
  VOID Wechat_RegisterAppCBs(&WeChat_ProfileCBs);
#endif
  
  VOID BspAccIntDelegate       ( ColomoBLE_TaskID, BSP_ACC_SHAKE_EVENT );
  VOID BspColorSensorDelegate  ( ColomoBLE_TaskID, BSP_DATA_READ_FINISH_EVENT );
  VOID BspSmartCoverIntDelegate( ColomoBLE_TaskID, BSP_SC_OPEN_EVENT, BSP_SC_SWITCH_EVENT, BSP_SC_CLOSE_EVENT );
  VOID BspRubyMotorInitDelegate(ColomoBLE_TaskID, BSP_CS_MOTOR_EVENT);
  // Register callback with SimpleGATTprofile
  VOID colomo_Register( &ColomoBLE_SimpleProfileCBs );

  // Enable clock divide on halt
  // This reduces active current while radio is active and CC254x MCU
  // is halted
  HCI_EXT_ClkDivOnHaltCmd( HCI_EXT_ENABLE_CLK_DIVIDE_ON_HALT );
  
  // Setup a delayed application startup, phase 1l
   //osal_start_timerEx( ColomoBLE_TaskID, CLM_INIT_PH1_EVT,    100 );
  // osal_start_timerEx( ColomoBLE_TaskID, SBP_PERIODIC_EVT,    10000 );
  //ClmState = 0;osal_start_timerEx( ColomoBLE_TaskID, BSP_SC_OPEN_EVENT,    1000 );
#ifdef WECHAT
  getTaskId(ColomoBLE_TaskID);
#endif
}

/*********************************************************************
 * @fn      ColomoBLE_ProcessEvent
 *
 * @brief   Simple BLE Peripheral Application Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id  - The OSAL assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  events not processed
 */
uint16 ColomoBLE_ProcessEvent( uint8 task_id, uint16 events )
{
  VOID task_id; // OSAL required parameter that isn't used in this function
  
  //--------------------------------------------------------------------------//
  // processing the system message
  //--------------------------------------------------------------------------//
  if ( events & SYS_EVENT_MSG )
  {
    uint8 *pMsg;

    if ( (pMsg = osal_msg_receive( ColomoBLE_TaskID )) != NULL )
    {
      ColomoBLE_ProcessOSALMsg( (osal_event_hdr_t *)pMsg );

      // Release the OSAL message
      /*
      if ((((osal_event_hdr_t *)pMsg)->event) == GATT_MSG_EVENT)
      {
        if ( ((gattMsgEvent_t *)pMsg)->method == ATT_HANDLE_VALUE_CFM ) //receive confirmation from the client
        {
          if( package_type != HISTORY_NULL )
          {
            osal_set_event( ColomoBLE_TaskID, BSP_HISTORY_DATA_EVT); //start to send the next history data
          }
          else // quit the history mode
          {
            RubyState = RUBY_STATE_CNT;
          }
        }
      }*/
      VOID osal_msg_deallocate( pMsg );
   
    }   
    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }
  
  //--------------------------------------------------------------------------//
  // processing the initial event
  //--------------------------------------------------------------------------//
  if ( events & CLM_INIT_PH1_EVT )
  {
    uint8 advEn = TRUE;
    uint16 lum_val[5];
    // power on the peripherals and setup voltage to 2.9v
    BSP_PERI_PWR_SBIT = 1;
    BspColorSensorDelayMS(100);

    pwrmgr_attribute.pwrmgr_device = PWRMGR_ALWAYS_ON;
    
    //电压检测
    lum_val[0] = BspRubyLumMeas();
    lum_val[1] = BspRubyLumMeas();
    lum_val[2] = BspRubyLumMeas();
    lum_val[3] = BspRubyLumMeas();
    lum_val[4] = ( lum_val[0] + lum_val[1] + lum_val[2] + lum_val[3]) >> 2;
    #if (defined DBG_UART) && (DBG_UART == TRUE)
      printf("lum_val : %d\r\n", lum_val[4]);
    #endif
    if(lum_val[4] < valve)
    {
      uint8 *blink;
      ClmState = CLM_STATE_LUM;
      blink = osal_msg_allocate(1);
      (*blink) = 2;
      osal_msg_send ( Hal_TaskID, blink );
      osal_start_timerEx(ColomoBLE_TaskID, BSP_SC_CLOSE_EVENT, 3000);
    }
    else
    {
      //open and configure color sensor
      BspColorSensorInit();
      // clear history data
      initial_cap_io_cnt = 0;
      skin_detect_cnt = 0;
      skin_array[0]=0; skin_array[1]=0; skin_array[2]=0; skin_array[3]=0; 
      
      // Start the Device
      VOID GAPRole_StartDevice( &ColomoBLE_PeripheralCBs );

      // Start Bond Manager
      VOID GAPBondMgr_Register( &ColomoBLE_BondMgrCBs );
      
      // Start advertising
      GAPRole_SetParameter( GAPROLE_ADVERT_ENABLED, sizeof( uint8 ), &advEn );
      
      // Start initial phase 2
      osal_start_timerEx( ColomoBLE_TaskID, CLM_INIT_PH2_EVT, 500 );
    }
    return ( events ^ CLM_INIT_PH1_EVT );
  }
  
  if ( events & CLM_INIT_PH2_EVT )
  { 
    BSP_CAP_IO_ICTL |= BSP_CAP_IO_ICTLBIT;  //打开io中断
    BspRubyTimer1Start();                   //打开定时器1
    //osal_pwrmgr_task_state( task_id, PWRMGR_HOLD );  //退出节点模式

    // Start initial phase 2
    osal_start_timerEx( ColomoBLE_TaskID, CLM_INIT_PH3_EVT, 200 );
    
    return ( events ^ CLM_INIT_PH2_EVT );
  }
  
  if ( events & CLM_INIT_PH3_EVT )
  {
    uint8 *blink;
    //----------------------关闭定时器----------------------//    
    BSP_CAP_IO_ICTL &= ~BSP_CAP_IO_ICTLBIT;
    BspRubyTimer1Stop();        //关闭定时器1  
    //osal_pwrmgr_task_state( task_id, PWRMGR_CONSERVE );  //进入节点模式
     //-----------------------------------------------------//
    initial_cap_io_cnt = bsp_cap_io_cnt;
    bsp_cap_io_cnt = 0;
    
#if defined( DBG_UART )
  printf("Application initial done, cap_io_cnt=%d\r\n", initial_cap_io_cnt);
#endif
    
    blink = osal_msg_allocate(1);
    (*blink) = 0xff;
    // Lightening the led to indicate initialization finished
    osal_msg_send ( Hal_TaskID, blink );
  
    //osal_start_timerEx(ColomoBLE_TaskID, CLM_START_DETECT_EVT, 2000);
    osal_start_timerEx(ColomoBLE_TaskID, CLM_PERIODIC_EVT,    2000);
  
    // terminate any connection and set to NCNT state
    GAPRole_TerminateConnection();
    ClmState = CLM_STATE_NCNT;
    
    // start power off time out
    CLM_POFF_TOUT_REFILL;
    return ( events ^ CLM_INIT_PH3_EVT );
  }
  
  //--------------------------------------------------------------------------//
  // process power on/off event
  //--------------------------------------------------------------------------//
  if ( events & BSP_SC_OPEN_EVENT )
  {
    if( ClmState == CLM_STATE_POFF )
    {
  #if (defined DBG_UART)
      printf("[ColomoBLE] BSP_SC_OPEN_EVENT\r\n");
  #endif
      
      osal_start_timerEx( ColomoBLE_TaskID, CLM_INIT_PH1_EVT, 100 );
    }
    return ( events ^ BSP_SC_OPEN_EVENT );
  }
  
  if ( events & BSP_SC_SWITCH_EVENT )           
  {
    if ( ClmState == CLM_STATE_PAIR )           //按键响应软件层绑定
    {
  #if (defined DBG_UART)
      printf("[ColomoBLE] 按键响应\r\n");
  #endif
      uint8 *blink, pBuf;
      
      CLM_POFF_TOUT_REFILL;
      
      ClmState = CLM_STATE_CNT;
      ClmPairTimeOut = 0xff;
      
      blink = osal_msg_allocate(1);
      (*blink) = 0xff;
      osal_msg_send ( Hal_TaskID, blink );
      
      pBuf = 1;
      ColomoSendNotify(gapConnHandle, COLOMO_CNFM_VALUE_POS, &pBuf, 1);
    }
    else if ( ClmState == CLM_STATE_MOTOR )     //切换为皮肤检查模式
    {
      ClmState = CLM_STATE_WORK;
      
     //----------------------打开检测-----------------------//
     if( GapRoleState == BAND_CONNECTED )
        osal_start_timerEx(ColomoBLE_TaskID, CLM_START_DETECT_EVT, 200);
     //----------------------------------------------------//
     
      BspRubyMotorClose();                      //关闭电机pwm
    }
    else if( ClmState != CLM_STATE_LUM && ClmState != CLM_STATE_FACTORY )//切换到震动模式，在低电量和出厂设置中无法开启震动         
    {

    #if (defined DBG_UART)
      printf("[ColomoBLE] BSP_SC_MOTOR_EVENT\r\n");
    #endif
      ClmState = CLM_STATE_MOTOR;
      
      // stop skin detect
      //----------------------关闭检测----------------------//
      BSP_CAP_IO_ICTL &= ~BSP_CAP_IO_ICTLBIT;
      BspRubyTimer1Stop();
      osal_stop_timerEx(ColomoBLE_TaskID, CLM_START_DETECT_EVT);
      osal_stop_timerEx(ColomoBLE_TaskID, CLM_CONFIRM_DETECT_PH1_EVT);
      //----------------------------------------------------//
      
      skin_detect_cnt=0;
      
      CLM_POFF_TOUT_REFILL;
      BspRubyMotorOpen(pwm_change_value);  //打开电机pwm
    }
    else if( ClmState == CLM_STATE_FACTORY )   //出厂设置，三次按键，分别是色卡的白，黑，黄测试
    {
      #if (defined DBG_UART)
      printf("factory_cnt:%d\r\n", factory_cnt);
      #endif
      if (factory_cnt < 3)
      {
        factory_cnt++;
        osal_start_timerEx( Hal_TaskID, BSP_CS_LED_START_EVENT, 100 );
      }
    }
    return ( events ^ BSP_SC_SWITCH_EVENT );
  }
  
  if ( events & BSP_SC_CLOSE_EVENT )
  {
    uint8 colomoAdvEn, *blink;
    if( ClmState != CLM_STATE_POFF )
    {
  #if (defined DBG_UART)
      printf("[ColomoBLE] BSP_SC_CLOSE_EVENT\r\n");
  #endif
      
      //关闭外部按键中断
      BSP_SC_INT_IEN  &= ~BSP_SC_INT_IENBIT;  
      // BLE advertising management TODO
      colomoAdvEn = FALSE;
      GAPRole_SetParameter( GAPROLE_ADVERT_ENABLED, sizeof( uint8 ), &colomoAdvEn );
      GAPRole_TerminateConnection();   // terminate the connection
      
      // change the device state
      ClmState = CLM_STATE_POFF;
      
      // stop skin detect
      //----------------------关闭定时器----------------------//
      BSP_CAP_IO_ICTL &= ~BSP_CAP_IO_ICTLBIT;
      skin_detect_cnt=0;
      BspRubyTimer1Stop();                   //关闭定时器1
      //osal_pwrmgr_task_state( task_id, PWRMGR_HOLD );  //退出节点模式
      //-----------------------------------------------------//
            
      BspRubyMotorClose();  //关闭电机pwm
    
      blink = osal_msg_allocate(1);
      (*blink) = 0xfe;
      osal_msg_send ( Hal_TaskID, blink );
      osal_start_timerEx(ColomoBLE_TaskID, BSP_POWER_CLOSE_EVENT, 2000);
    }
    return ( events ^ BSP_SC_CLOSE_EVENT );
  }
  
  if ( events & BSP_POWER_CLOSE_EVENT )
  {
    if( ClmState == CLM_STATE_POFF | ClmState == CLM_STATE_LUM )
    {
  #if (defined DBG_UART)
      printf("[ColomoBLE] BSP_POWER_CLOSE_EVENT\r\n");
  #endif
      pwrmgr_attribute.pwrmgr_device = PWRMGR_BATTERY;
      // turn off the peri power and set core voltage to 2.1v
      BSP_PERI_PWR_SBIT = 0;
      
      HAL_SYSTEM_RESET();
    }

    return ( events ^ BSP_POWER_CLOSE_EVENT );
  }
  
  //--------------------------------------------------------------------------//
  // processing the motor events
  //--------------------------------------------------------------------------//
  if ( events & BSP_CS_MOTOR_EVENT )
  {
    BspRubyMotorClose();  //关闭电机pwm
    return ( events ^ BSP_CS_MOTOR_EVENT );
  }
  
  //--------------------------------------------------------------------------//
  // processing colomo periodic even
  //--------------------------------------------------------------------------//
  if ( events & CLM_PERIODIC_EVT )
  {
    uint16 lum_val[5];
    if( ClmState != CLM_STATE_POFF && ClmState != CLM_STATE_LUM )
    {
#if (defined DBG_UART)
      printf("[Clm State]: %d, [RTC Clock]: %d\r\n", ClmState, (uint16)(osal_getClock()));
#endif
      osal_start_timerEx(ColomoBLE_TaskID, CLM_PERIODIC_EVT,    2000);
    }
    
    //电压检测
    lum_val[0] = BspRubyLumMeas();
    lum_val[1] = BspRubyLumMeas();
    lum_val[2] = BspRubyLumMeas();
    lum_val[3] = BspRubyLumMeas();
    lum_val[4] = (lum_val[0] + lum_val[1] + lum_val[2] + lum_val[3]) >> 2;
    #if (defined DBG_UART) && (DBG_UART == TRUE)
      //printf("lum_val : %d\r\n", lum_val[4]);
    #endif
      
    if(lum_val[4] < valve)
    {
      uint8 *blink;
      ClmState = CLM_STATE_LUM;
      blink = osal_msg_allocate(1);
      (*blink) = 2;
      osal_msg_send ( Hal_TaskID, blink );
      osal_start_timerEx(ColomoBLE_TaskID, BSP_SC_CLOSE_EVENT, 3000);
    }

    if( ClmPairTimeOut != 0xff )
    {
      if( (ClmPairTimeOut-- == 0) )
      {
        //if( ClmState == CLM_STATE_PAIR | ClmState == CLM_STATE_NCNT)
        //{
          uint8 *blink;
          ClmState = CLM_STATE_CNT;
          GapRoleState = PAIR;
          
          blink = osal_msg_allocate(1);
          (*blink) = 0xff;
          osal_msg_send ( Hal_TaskID, blink );
       // }
        ClmPairTimeOut = 0xff;
      }
    }
    
    if( (ClmPoffTimeOut != 0xff) && (ClmState != CLM_STATE_MOTOR) )
    {
      if( (ClmPoffTimeOut-- == 0) )
      {
        ClmPoffTimeOut = 0xff;
        osal_set_event( ColomoBLE_TaskID, BSP_SC_CLOSE_EVENT );
      }
    }
    
    return ( events ^ CLM_PERIODIC_EVT );
  }
  
  //--------------------------------------------------------------------------//
  // processing the skin detect event
  //--------------------------------------------------------------------------//
  if ( events & CLM_START_DETECT_EVT )
  {
    if( ClmState == CLM_STATE_WORK )
    {
    #if (defined DBG_UART)
      //printf("[ColomoBLE] CLM_START_DETECT_EVT\r\n");
    #endif
      BSP_CAP_IO_ICTL |= BSP_CAP_IO_ICTLBIT;  //打开io中断
      BspRubyTimer1Start();                   //打开定时器1
      //osal_pwrmgr_task_state( task_id, PWRMGR_HOLD );  //退出节点模式
    
      osal_start_timerEx( ColomoBLE_TaskID, CLM_CONFIRM_DETECT_PH1_EVT, 200 );
    }
    return ( events ^ CLM_START_DETECT_EVT );
  }
  
  if ( events & CLM_CONFIRM_DETECT_PH1_EVT )
  { 
    #if (defined DBG_UART)
      //printf("bsp_cap_io_cnt :%d\r\n", bsp_cap_io_cnt);
    #endif
    uint16 temp;
    
    //----------------------关闭定时器----------------------//
    BSP_CAP_IO_ICTL &= ~BSP_CAP_IO_ICTLBIT;
    BspRubyTimer1Stop();                   //关闭定时器1
    //osal_pwrmgr_task_state( task_id, PWRMGR_CONSERVE );  //进入节点模式
    //------------------------------------------------------//  
    
    if( bsp_cap_io_cnt <= initial_cap_io_cnt-40 )
    {
      skin_array[skin_detect_cnt] = bsp_cap_io_cnt;
      skin_detect_cnt++;
    }
    else
    {
      skin_detect_cnt = 0;
    }
    bsp_cap_io_cnt = 0;
    
    if(skin_detect_cnt==4)
    {
      temp = skin_array[0]+skin_array[1]+skin_array[2]+skin_array[3];
      temp = temp>>2;
      skin_mois = initial_cap_io_cnt - temp;
      skin_detect_cnt=0;
      
      // start skin color event
      osal_start_timerEx( Hal_TaskID, BSP_CS_LED_START_EVENT, 100 );
    }
    else
    {
      // start a new skin detect event
      osal_start_timerEx( ColomoBLE_TaskID, CLM_START_DETECT_EVT, 200 );
    }
    return ( events ^ CLM_CONFIRM_DETECT_PH1_EVT );
  }
  
  //--------------------------------------------------------------------------//
  // processing the skin color event
  //--------------------------------------------------------------------------//
  if ( events & BSP_DATA_READ_FINISH_EVENT )
  {
    uint16 res;
    uint8 pBuf[14] = {0};
    res = BspRubyResMeas();
    
    BspRubyMotorTimer();
    
    ColorValue = all;
    if(ColorValue > ColorMax)
      ColorValue = ColorMax;
    if(ColorValue < ColorMin)
      ColorValue = ColorMin;
    ColorValue = (uint16)((float)(ColorValue - ColorMin) * ColorPara); 
     
#if defined( DBG_UART )
    printf("Color: %d \r\n", ColorValue);
    printf("[color]=%d | [mois]=%d | [res]=%d\r\n", all, skin_mois, res);
#endif
    
    if( ClmState == CLM_STATE_WORK )
    {
      pBuf[0] = (uint8) ColorValue;
      pBuf[1] = (uint8)(ColorValue>>8);
      pBuf[2] = (uint8) skin_mois;
      pBuf[3] = (uint8)(skin_mois>>8);
      pBuf[4] = (uint8) res;
      pBuf[5] = (uint8)(res>>8);
      ColomoSendNotify(gapConnHandle, COLOMO_MEAS_VALUE_POS, pBuf, 14);
      wechat_pBuf[0] = (uint8) all;
      wechat_pBuf[1] = (uint8)(all>>8);
      wechat_pBuf[2] = (uint8) skin_mois;
      wechat_pBuf[3] = (uint8)(skin_mois>>8);
      //printf("aaaaaaaaaa:%d , %d , %d , %d \r\n", wechat_pBuf[0],wechat_pBuf[1],wechat_pBuf[2],wechat_pBuf[3]);
      //printf("bbbbbbbbbb:%d , %d , %d , %d \r\n", (uint8) ColorValue,(uint8)(ColorValue>>8),(uint8) skin_mois,(uint8)(skin_mois>>8));
      //pBuf[4] = (uint8) res;
      //pBuf[5] = (uint8)(res>>8);
#ifdef WECHAT
      device_SendData(wechat_pBuf, 6);
#endif
    }
    else if( ClmState == CLM_STATE_FACTORY )
    {
      static uint8 count;
      static uint8 flash_buf[4];
      static uint16 buf[5];
      if(count < 3)
      {       
        buf[count] = all;
        count++;
        osal_start_timerEx( Hal_TaskID, BSP_CS_LED_START_EVENT, 100 );
      }
      else
      {
        buf[3] = all;
        buf[4] = (buf[0] + buf[1] + buf[2] + buf[3]) >> 2;
        count = 0;
        if(factory_cnt == 1)
        {
          //写入flash
          ColorMax = buf[4];
          for(int i = 0; i < 5; i++)  //清除数组
            buf[i] = 0;
        }
        else if(factory_cnt == 2)
        {
          //写入flash
          ColorMin = buf[4];
          if( ColorMax < ColorMin )
          {
            ColorMid = ColorMin;
            ColorMin = ColorMax;
            ColorMax = ColorMid;
            ColorMid = 0;
          }
          flash_buf[0] = (uint8)ColorMax;
          flash_buf[1] = (uint8)(ColorMax >> 8);
          flash_buf[2] = (uint8)ColorMin;
          flash_buf[3] = (uint8)(ColorMin >> 8);
          /*
          HalFlashErase(0x78);
          while( FCTL & 0x80 ); //等待擦除完成
          HalFlashWrite(0xF000, flash_buf, 4); 
          */
          osal_snv_write(SNV_COLOR_ID, sizeof(flash_buf), flash_buf);
          for(int i = 0; i < 5; i++)  //清除数组
            buf[i] = 0;          
          for(int i = 0; i < 4; i++)  //清除数组
            flash_buf[i] = 0;
        }
        else if(factory_cnt == 3)
        {
          //判断检测是否有误差
          
        }
      }
      CLM_POFF_TOUT_REFILL;
      return ( events ^ BSP_DATA_READ_FINISH_EVENT );
    }
    // start a new skin detect event
    osal_start_timerEx( ColomoBLE_TaskID, CLM_START_DETECT_EVT, 400 );
    
    CLM_POFF_TOUT_REFILL;
    
    return ( events ^ BSP_DATA_READ_FINISH_EVENT );
  }
  
  // Discard unknown  events
  return 0;
}

/********************************************************************* 
 * @fn      ColomoBLE_ProcessOSALMsg 
 * 
 * @brief   Process an incoming task message. 
 * 
 * @param   pMsg - message to process 
 * 
 * @return  none 
 */  
static void ColomoBLE_ProcessOSALMsg( osal_event_hdr_t *pMsg )  
{  
  switch ( pMsg->event )  
  {  
  case GATT_MSG_EVENT: 
      Indication_ProcessGattMsg( (gattMsgEvent_t *) pMsg );  
      break;  
        
  default:  
    // do nothing  
    break;  
  }  
}  

/********************************************************************* 
 * @fn      Indication_ProcessGattMsg 
 * 
 * @brief   Process GATT messages 
 * 
 * @return  none 
 */  
static void Indication_ProcessGattMsg( gattMsgEvent_t *pMsg )  
{  
  switch ( pMsg->method )
  {
  case ATT_HANDLE_VALUE_CFM:
    {
      //20字节片段发送
#ifdef WECHAT
      ble_wechat_indicate_data_chunk();
#endif
    }
  }  
}  

/*********************************************************************
 * @fn      peripheralStateNotificationCB
 *
 * @brief   Notification from the profile of a state change.
 *
 * @param   newState - new state
 *
 * @return  none
 */
static void peripheralStateNotificationCB( gaprole_States_t newState )
{
  if( ClmState != CLM_STATE_POFF )
  {
    ClmState = CLM_STATE_NCNT;
    CLM_POFF_TOUT_REFILL;
  }
  
  switch ( newState )
  {
    case GAPROLE_STARTED:
      {
        uint8 ownAddress[B_ADDR_LEN];
        uint8 systemId[DEVINFO_SYSTEM_ID_LEN];

        GAPRole_GetParameter(GAPROLE_BD_ADDR, ownAddress);

        // use 6 bytes of device address for 8 bytes of system ID value
        systemId[0] = ownAddress[0];
        systemId[1] = ownAddress[1];
        systemId[2] = ownAddress[2];

        // set middle bytes to zero
        systemId[4] = 0x00;
        systemId[3] = 0x00;

        // shift three bytes up
        systemId[7] = ownAddress[5];
        systemId[6] = ownAddress[4];
        systemId[5] = ownAddress[3];

        DevInfo_SetParameter(DEVINFO_SYSTEM_ID, DEVINFO_SYSTEM_ID_LEN, systemId);
        
        #if (defined DBG_UART) && (DBG_UART == TRUE)
          printf("[GapRole] Initialized Device: %s\r\n", bdAddr2Str( ownAddress ));
        #endif
      }
      break;

    case GAPROLE_ADVERTISING:
      {
        #if (defined DBG_UART) && (DBG_UART == TRUE)
          printf("[GapRole] Advertising\r\n");
        #endif
      }
      break;

    case GAPROLE_CONNECTED:
      {
        GAPRole_GetParameter(GAPROLE_CONNHANDLE, &gapConnHandle);
        #if (defined DBG_UART) && (DBG_UART == TRUE)
          printf("[GapRole] Connected\r\n");
        #endif
        ClmState = CLM_STATE_CNT;
        BspRubyMotorClose();                   //关闭电机pwm
      }
      break;

    case GAPROLE_WAITING:
      {
        #if (defined DBG_UART) && (DBG_UART == TRUE)
          printf("[GapRole] Waiting\r\n");
        #endif
          GapRoleState = WAITING;                                                           
          
          //----------------------关闭检测-----------------------//
          osal_stop_timerEx(ColomoBLE_TaskID, CLM_START_DETECT_EVT);
          osal_stop_timerEx(ColomoBLE_TaskID, CLM_CONFIRM_DETECT_PH1_EVT);
          BSP_CAP_IO_ICTL &= ~BSP_CAP_IO_ICTLBIT;
          BspRubyTimer1Stop();                   //关闭定时器1
          //osal_pwrmgr_task_state( ColomoBLE_TaskID, PWRMGR_CONSERVE );  //进入节点模式
          //-----------------------------------------------------//
      }
      break;

    case GAPROLE_WAITING_AFTER_TIMEOUT:
      {
        #if (defined DBG_UART) && (DBG_UART == TRUE)
          printf("[GapRole] Time out\r\n");
        #endif
      }
      break;

    case GAPROLE_ERROR:
      {
        #if (defined DBG_UART) && (DBG_UART == TRUE)
          printf("[GapRole] Error\r\n");
        #endif
      }
      break;

    default:
      {
      }
      break;

  }

  gapProfileState = newState;
  
  // if not connected, turn off the light
  if(gapProfileState == CLM_STATE_NCNT)
  {
    uint8 * blink;
    blink = osal_msg_allocate(1);
    (*blink) = 0;
    osal_msg_send ( Hal_TaskID, blink );
  }

#if !defined( CC2540_MINIDK )
  VOID gapProfileState;     // added to prevent compiler warning with
                            // "CC2540 Slave" configurations
#endif
}

/*********************************************************************
 * @fn      performPeriodicTask
 *
 * @brief   Perform a periodic application task. This function gets
 *          called every five seconds as a result of the SBP_PERIODIC_EVT
 *          OSAL event. In this example, the value of the third
 *          characteristic in the SimpleGATTProfile service is retrieved
 *          from the profile, and then copied into the value of the
 *          the fourth characteristic.
 *
 * @param   none
 *
 * @return  none
 */
//static void performPeriodicTask( void )
//{
//}

/*********************************************************************
 * @fn      ColomoProfileChangeCB
 *
 * @brief   Callback from ColomoBLEProfile indicating a value change
 *
 * @param   paramID - parameter ID of the value that was changed.
 *
 * @return  none
 */
static void ColomoProfileChangeCB(uint8 paramID)
{
  uint8 newValue[3];
  
  switch( paramID )
  {
    case COLOMO_CTRL:
      colomo_GetParameter(COLOMO_CTRL, &newValue);
      
      if ( newValue[0] == 0x01 ) // Set to bond mode
      {
        uint8 *blink;
        GapRoleState = PAIR;
        
        //----------------------关闭检测-----------------------//
        osal_stop_timerEx(ColomoBLE_TaskID, CLM_START_DETECT_EVT);
        osal_stop_timerEx(ColomoBLE_TaskID, CLM_CONFIRM_DETECT_PH1_EVT);
        BSP_CAP_IO_ICTL &= ~BSP_CAP_IO_ICTLBIT;
        BspRubyTimer1Stop();                   //关闭定时器1
        BspRubyMotorClose();                   //关闭电机pwm
        //osal_pwrmgr_task_state( ColomoBLE_TaskID, PWRMGR_CONSERVE );  //进入节点模式
        //-----------------------------------------------------//
          
        ClmState = CLM_STATE_PAIR;
        blink = osal_msg_allocate(1);
        (*blink) = 8;
        // Lightening the led to indicate initialization finished
        osal_msg_send ( Hal_TaskID, blink );
        
        // 20 second time out
        ClmPairTimeOut=6;

      }
      
      if ( newValue[0] == 0x02 ) // Set to work mode
      {
        #ifdef DBG_UART
        printf("go into the work mode\r\n");
        #endif
        ClmState = CLM_STATE_WORK;
        GapRoleState = BAND_CONNECTED;
        
        //----------------------打开检测-----------------------//
         BspRubyMotorClose();  //关闭电机pwm
         osal_start_timerEx(ColomoBLE_TaskID, CLM_START_DETECT_EVT, 200);
        //----------------------------------------------------//
      }
      if ( newValue[0] == 0x11 ) //调节电机的pwm为1档
      {
        if( ClmState == CLM_STATE_MOTOR )
        {
          pwm_change_value = 30;
          BspRubyMotorChangePwm(pwm_change_value);
          osal_snv_write(SNV_MOTOR_ID, sizeof(pwm_change_value), &pwm_change_value);
        }
      }
      if ( newValue[0] == 0x12 ) //调节电机的pwm为2档
      {
        if( ClmState == CLM_STATE_MOTOR )
        {
          pwm_change_value = 50;
          BspRubyMotorChangePwm(pwm_change_value);
          osal_snv_write(SNV_MOTOR_ID, sizeof(pwm_change_value), &pwm_change_value);
        }
      }
      if ( newValue[0] == 0x13 ) //调节电机的pwm为3档
      {
        if( ClmState == CLM_STATE_MOTOR )
        {
          pwm_change_value = 90;
          BspRubyMotorChangePwm(pwm_change_value);
          osal_snv_write(SNV_MOTOR_ID, sizeof(pwm_change_value), &pwm_change_value);
        }
      }

      if ( newValue[0] == 0x03 ) // Color test
      {
        // start skin color event
        osal_start_timerEx( Hal_TaskID, BSP_CS_LED_START_EVENT, 100 );
      }
      
      if ( newValue[0] == 0x04 ) // Voltage test
      {
        // change the valve of voltage
        valve = 6000;
      }
      
      if ( newValue[0] == 0x05 ) // Color calibration
      {
        if(ClmState == CLM_STATE_CNT | ClmState == CLM_STATE_FACTORY)
        {
          ClmState = CLM_STATE_FACTORY;
          factory_cnt = 0;
        }
      }
      if ( newValue[0] == 0x06 ) // Enter the OAD mode
      {
        ClmState = CLM_STATE_OAD;
        
        //关闭外部按键中断
        //BSP_SC_INT_IEN  &= ~BSP_SC_INT_IENBIT;          
        // stop skin detect
        //----------------------关闭定时器----------------------//
        BSP_CAP_IO_ICTL &= ~BSP_CAP_IO_ICTLBIT;
        skin_detect_cnt=0;
        BspRubyTimer1Stop();                   //关闭定时器1
        //-----------------------------------------------------//
        osal_stop_timerEx(ColomoBLE_TaskID, CLM_START_DETECT_EVT);
        osal_stop_timerEx(ColomoBLE_TaskID, CLM_CONFIRM_DETECT_PH1_EVT);
        osal_stop_timerEx(ColomoBLE_TaskID, CLM_PERIODIC_EVT);
        BspRubyMotorClose();  //关闭电机pwm
        #if (defined DBG_UART) && (DBG_UART == TRUE)
          printf("colomo enters oad mode\r\n");
        #endif
      }
      if ( newValue[0] == 0x07 ) // reset
      {
        osal_start_timerEx(ColomoBLE_TaskID, BSP_SC_CLOSE_EVENT, 200);
      }
      break;
      
    default:
      // should not reach here!
      break;
  }
}

#if (defined DBG_UART) && (DBG_UART == TRUE)
/*********************************************************************
 * @fn      bdAddr2Str
 *
 * @brief   Convert Bluetooth address to string. Only needed when
 *          LCD display is used.
 *
 * @return  none
 */
char *bdAddr2Str( uint8 *pAddr )
{
  uint8       i;
  char        hex[] = "0123456789ABCDEF";
  static char str[B_ADDR_STR_LEN];
  char        *pStr = str;

  *pStr++ = '0';
  *pStr++ = 'x';

  // Start from end of addr
  pAddr += B_ADDR_LEN;

  for ( i = B_ADDR_LEN; i > 0; i-- )
  {
    *pStr++ = hex[*--pAddr >> 4];
    *pStr++ = hex[*pAddr & 0x0F];
  }

  *pStr = 0;

  return str;
}
#endif

#ifdef WECHAT
extern uint8 wechatIndicate[WECHAT_WRITE_LEN];
static void WeChatAttrCB( uint8 attrParamID )
{
  switch( attrParamID )
  {	
	case WECHAT_WRITE:
#ifdef DBG_UART
          printf("write\r\n");
#endif
		Wechat_GetParameter( WECHAT_WRITE, &wechatIndicate);	
		Wechat_Message();
		break;
        case WECHAT_INDICATE:
#ifdef DBG_UART
          printf("indicate\r\n");
#endif
		WechatDisconnect();
		device_SendData(wechat_pBuf, 6);
                
                ClmState = CLM_STATE_WORK;
                GapRoleState = BAND_CONNECTED;
                //----------------------打开检测-----------------------//
                BspRubyMotorClose();  //关闭电机pwm
                osal_start_timerEx(ColomoBLE_TaskID, CLM_START_DETECT_EVT, 200);
                //----------------------------------------------------//
                break;
	default:
	  // should not reach here!
	  break;
    /**/
  }
}       
#endif

/*********************************************************************
*********************************************************************/
