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
  PROVIDED “AS IS?WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
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

#if defined DBG_UART
#include "bsp_uart.h"
#include "SerialApp.h"
#include "hal_uart.h"
#endif

#include "OSAL.h"

#if defined POWER_SAVING
#include "OSAL_PwrMgr.h"
#endif

/**************************************************************************************************
 *                                      GLOBAL VARIABLES
 **************************************************************************************************/
uint8 Hal_TaskID;

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
  // DbgUartInit();
  HalUARTInit();
  serialAppInitTransport();
  SerialPrintString("Hello");
#endif
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

      /* De-allocate */
      osal_msg_deallocate( msgPtr );
      /* Next */
      msgPtr = osal_msg_receive( Hal_TaskID );
    }
    return events ^ SYS_EVENT_MSG;
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

