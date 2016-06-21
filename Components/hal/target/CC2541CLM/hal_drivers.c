/**************************************************************************************************
  Filename:       hal_drivers.c
  Revised:        $Date: 2007-07-06 10:42:24 -0700 (Fri, 06 Jul 2007) $
  Revision:       $Revision: 13579 $

  Description:    This file contains the interface to the Drivers Service.


  Copyright 2005-2012 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED 揂S IS?WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
**************************************************************************************************/

/**************************************************************************************************
 *                                            INCLUDES
 **************************************************************************************************/
#if (defined HAL_AES) && (HAL_AES == TRUE)
#include "hal_aes.h"
#endif
#if (defined HAL_DMA) && (HAL_DMA == TRUE)
#include "hal_dma.h"
#endif
#include "hal_drivers.h"
#include "hal_key.h"
#include "hal_sleep.h"
#include "hal_timer.h"
#include "hal_types.h"
#include "hal_adc.h"
#include "bsp_ruby_led.h"
#include "bsp_ruby_res.h"
#include "bsp_ruby_flash.h"
#include "bsp_power.h"
#include "bsp_color_9300.h"
#include "bsp_acc.h"
#include "bsp_cap_io.h"
#include "bsp_smart_cover.h"
#include "bsp_ruby_motor.h"
#include "bsp_ruby_timer.h"
#include "bsp_ruby_lum.h"

#if defined DBG_UART
#include "bsp_uart.h"
#endif

#include "OSAL.h"

#if defined POWER_SAVING
#include "bsp_power.h"
#include "OSAL_PwrMgr.h"
#endif

/**************************************************************************************************
 *                                      GLOBAL VARIABLES
 **************************************************************************************************/
uint8 Hal_TaskID;
uint8 LedBlinking = 0;
uint8 blink=0;
uint8 pwm=0;
uint8 add=1;

/**************************************************************************************************
 * @fn      Hal_Init
 *
 * @brief   Hal Initialization function.
 *
 * @param   task_id - Hal TaskId
 *
 * @return  None
 **************************************************************************************************/
void Hal_Init( uint8 task_id )
{
  /* Register task ID */
  Hal_TaskID = task_id;
}

/**************************************************************************************************
 * @fn      Hal_DriverInit
 *
 * @brief   Initialize HW - These need to be initialized before anyone.
 *
 * @param   task_id - Hal TaskId
 *
 * @return  None
 **************************************************************************************************/
void HalDriverInit (void)
{
  /* UART */
#if (defined DBG_UART) && (DBG_UART == TRUE)
  DbgUartInit();
#endif

#if (defined HAL_DMA) && (HAL_DMA == TRUE)  
  HalDmaInit();
#endif
    
  HalAdcInit();
  
  BspSmartCoverInit();
      
  BspRubyLumInit();
  
  BspCapIOInit();
  
  BspRubyResInit();
   
  BspRubyFlashInit();
  /**/
}

/**************************************************************************************************
 * @fn      Hal_ProcessEvent
 *
 * @brief   Hal Process Event
 *
 * @param   task_id - Hal TaskId
 *          events - events
 *
 * @return  None
 **************************************************************************************************/
uint16 Hal_ProcessEvent( uint8 task_id, uint16 events )
{
  uint8 *msgPtr;
  
  (void)task_id;  // Intentionally unreferenced parameter
  
  //--------------------------------------------------------------------------//
  // processing the system message
  //--------------------------------------------------------------------------//
  if ( events & SYS_EVENT_MSG )
  {
    msgPtr = osal_msg_receive(Hal_TaskID);

    while (msgPtr)
    {
      /* Do something here - for now, just deallocate the msg and move on */
      blink = msgPtr[0];
      
      /*if( blink >= 31 ) // if blink times great than 30 means perminant blink
      {                 // unless the app turn it off by setting blink to 0
        blink = 31;
      }
      */
      /* De-allocate */
      osal_msg_deallocate( msgPtr );
      /* Next */
      msgPtr = osal_msg_receive( Hal_TaskID );
    }
    
    if ( blink )
    {
      osal_start_timerEx( Hal_TaskID, HAL_LED_PERIODIC_EVT, HAL_LED_PERIODIC_EVT_PERIOD );
    }
    else
    {
      LedBlinking = 0;
      BspRubyLedOff();
    }
    
    return events ^ SYS_EVENT_MSG;
  }
  
  //--------------------------------------------------------------------------//
  // processing led blink event
  //--------------------------------------------------------------------------//
  if( events & HAL_LED_PERIODIC_EVT){
    if ( blink )
    {
      osal_start_timerEx( Hal_TaskID, HAL_LED_PERIODIC_EVT, HAL_LED_PERIODIC_EVT_PERIOD );
      //osal_pwrmgr_task_state( task_id, PWRMGR_HOLD );
      LedBlinking = 1;
      if( add )
      {
        if( ++pwm == 100 )
        {
          add = 0;
          if( blink == 0xff )    //0xff表示pwm为100%时常亮
          {
             blink = 0;
          }
        }
        else
          BspRubyLedOn(pwm); 
      }
      else
      {
        if( --pwm == 0)
        {
          add = 1;
          if( blink == 0xfe )     //0xfe表示pwm为0%是关闭
            blink = 0;
          else if( blink != 0xff )
            blink--;
          BspRubyLedOff();
        }
        else
          BspRubyLedOn(pwm);
      }
      /*if ( add )
      {
        pwm=pwm+1;
        if(pwm==100) add = 0;
        BspRubyLedOn(pwm);
        if( blink == 0xff )
        {
          
        }
      }
      else
      {
        pwm=pwm-1;
        if(pwm==0)
        {
          add = 1;
          if( blink != 31 )
          {
            blink--;
          }
          BspRubyLedOff();
        }
        else
        {
          BspRubyLedOn(pwm);
        }
      }*/
    }
    else
    {
      //osal_pwrmgr_task_state( task_id, PWRMGR_CONSERVE );
      LedBlinking=0;
    }
    
    return events ^ HAL_LED_PERIODIC_EVT;
  }
  //--------------------------------------------------------------------------//
  // processing the bsp color sensor related events
  //--------------------------------------------------------------------------//
  if ( events & BSP_CAP_IO_START_EVENT )
  {
    BSP_CAP_IO_ICTL |= BSP_CAP_IO_ICTLBIT;
    osal_start_timerEx( task_id, BSP_CAP_IO_END_EVENT, 100 );
    return (events ^ BSP_CAP_IO_START_EVENT); 
  }
  
  if ( events & BSP_CAP_IO_END_EVENT )
  {
    BSP_CAP_IO_ICTL &= ~BSP_CAP_IO_ICTLBIT;
#if defined( DBG_UART )
    printf("[BSP CAP] %d\r\n", bsp_cap_io_cnt);
#endif
    bsp_cap_io_cnt = 0;
    
    osal_set_event(BspCapIODelegateTaskID, BspCapIODelegateEvent);
    
    return (events ^ BSP_CAP_IO_END_EVENT); 
  }
  
  //--------------------------------------------------------------------------//
  // processing the bsp color sensor related events
  //--------------------------------------------------------------------------//
  if ( events & BSP_CS_LED_START_EVENT )
  {
    //打开 led
    BSP_CLRLED_SBIT = 1;
    osal_start_timerEx( task_id, BSP_CS_LED_END_EVENT, 100 );
    return (events ^ BSP_CS_LED_START_EVENT); 
  }
  
  if ( events & BSP_CS_LED_END_EVENT )
  {
    //100ms led延时，开始检查 color, 10ms 传感器充电
    BspColorSensorColorStart();
    osal_start_timerEx( task_id, BSP_CS_READ_EVENT, 10 );
    return (events ^ BSP_CS_LED_END_EVENT); 
  }
  
  if ( events & BSP_CS_READ_EVENT )
  {
    //停止传感器充电，读取传感器数据，关闭 led
    BspColorSensorColorStop();
    BspColorSensorColorRead();
    BSP_CLRLED_SBIT = 0;
    osal_set_event( BspCsDelegateTaskID, BspCsDelegateEvent );
    return (events ^ BSP_CS_READ_EVENT); 
  }   
  //--------------------------------------------------------------------------//
  // processing the smart cover event
  //--------------------------------------------------------------------------//
  if ( events & BSP_SC_INT_EVENT )
  {
    uint8 pin_state;
    static bool short_state;

    pin_state = BSP_SC_PIN_STATE;

    if ( pin_state == BSP_SC_HIGH )
    {
      if ( BspScVirtualCnt++ > 2 )
      {
        //key_state = KEY_STATE_LONG;
        BspScVirtualCnt = 0;
      #ifdef DBG_UART
       printf("long\r\n");
      #endif
       BspSmartCoverProcess( 3 );
      }
      else
      {
        osal_start_timerEx( task_id, BSP_SC_INT_EVENT, 500 );
      }
    }
    else
    {
      if ( BspScVirtualCnt >= 1 )
      {
        BspScVirtualCnt = 0;
        if ( short_state++ == 0 )
        {
        #ifdef DBG_UART
          printf("short_0\r\n");
        #endif
          BspSmartCoverProcess( 1 );
        }
        else
        {
          //short_state = 0;
        #ifdef DBG_UART
          printf("short_1\r\n");
        #endif
          BspSmartCoverProcess( 2 );
        }
      }
    }
    /*if( BspScVirtualCnt <= 4 )
    {
      if ( pin_state == BSP_SC_LOW )
      {
        BspScVirtualCnt ++;
        osal_start_timerEx( task_id, BSP_SC_INT_EVENT, 500 );
      }
      else
      {
        BspSmartCoverProcess(1);
        BspScVirtualCnt = 0;
      }
    }
    else
    {
      BspSmartCoverProcess(0);
      BspScVirtualCnt = 0;
    }*/
    
    return events ^ BSP_SC_INT_EVENT;
  }
  
  return 0;
}

/**************************************************************************************************
 * @fn      Hal_ProcessPoll
 *
 * @brief   This routine will be called by OSAL to poll UART, TIMER...
 *
 * @param   task_id - Hal TaskId
 *
 * @return  None
 **************************************************************************************************/
void Hal_ProcessPoll ()
{
#if defined( POWER_SAVING )
  /* Allow sleep before the next OSAL event loop */
  ALLOW_SLEEP_MODE();
#endif
}

/**************************************************************************************************
 * @fn      Hal_Delay100uS
 *
 * @brief   This function generates blocking delay
 *
 * @param   msec
 *
 * @return  None
 **************************************************************************************************/
void Hal_Delay100uS(uint16 usec)
{ 
    volatile uint16 i,j;
    
    for (i=0; i<usec; i++)
        for (j=0; j<38; j++);
}

/**************************************************************************************************
**************************************************************************************************/

