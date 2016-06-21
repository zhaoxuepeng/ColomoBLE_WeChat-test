/**************************************************************************************************
  Filename:       bsp_isr.c
  Revised:        $Date: 2015/7/8 $
  Revision:       $Revision: 1 $

  Description:    This file contains the interrupt service routines

  Copyright 2015 - 2016 QijiTek. All rights reserved.
**************************************************************************************************/

/***************************************************************************************************
 *                                             INCLUDES
 ***************************************************************************************************/
#include "hal_mcu.h"
#include "hal_defs.h"
#include "hal_types.h"
#include "hal_drivers.h"
#include "osal.h"
#include "hal_board.h"
#include "hal_i2c.h"

// need to include all bsp packages who need isr functions
#include "bsp_acc.h"
#include "bsp_cap_io.h"
#include "bsp_smart_cover.h"

/***************************************************************************************************
 *                                             CONSTANTS
 ***************************************************************************************************/

/***************************************************************************************************
 *                                              MACROS
 ***************************************************************************************************/
#define P0IRCON       IRCON
#define P0IRCON_BIT   BV(5)
#define P1IRCON       IRCON2
#define P1IRCON_BIT   BV(3)

/***************************************************************************************************
 *                                              TYPEDEFS
 ***************************************************************************************************/

/***************************************************************************************************
 *                                           GLOBAL VARIABLES
 ***************************************************************************************************/
extern uint8 timer4_count = 0;
/***************************************************************************************************
 *                                            LOCAL FUNCTION
 ***************************************************************************************************/

/***************************************************************************************************
 *                                    INTERRUPT SERVICE ROUTINE
 ***************************************************************************************************/

/***************************************************************************************************
 * @fn      Port 0 ISR
 *
 * @brief
 *
 * @param
 *
 * @return
 ***************************************************************************************************/
HAL_ISR_FUNCTION( P0IntIsr, P0INT_VECTOR )
{
  HAL_ENTER_ISR();

  // ---------------------------------------------------------------------------------------------//
  // bsp smart cover interrupt process
  if (BSP_SC_INT_PXIFG & BSP_SC_INT_BIT)
  {
    BSP_SC_INT_PXIFG &= ~BSP_SC_INT_BIT;
    
    if (BspScDelegateState == BSP_SC_INT_RDY)
    {
      // delay 100ms to send event to Hal confirming the open state of smart cover
      // and then starts to polling the state with 2s
      osal_start_timerEx (Hal_TaskID, BSP_SC_INT_EVENT, BSP_SC_INT_DELAY);
    }
  }
  
  // need to clear port1 interrupt flag in IRCON2
  P0IRCON &= ~P0IRCON_BIT;
  
  CLEAR_SLEEP_MODE();
  HAL_EXIT_ISR();

  return;
}

/***************************************************************************************************
 * @fn      Port 1 ISR
 *
 * @brief
 *
 * @param
 *
 * @return
 ***************************************************************************************************/

HAL_ISR_FUNCTION( P1IntIsr, P1INT_VECTOR )
{
  HAL_ENTER_ISR();
  
  // ---------------------------------------------------------------------------------------------//
  // bsp accelerator cover interrupt process
  /*
  if (BSP_ACC_INT_PXIFG & BSP_ACC_INT_BIT)
  {
    BSP_ACC_INT_PXIFG &= ~BSP_ACC_INT_BIT;

    if (BspAccDelegateState == BSP_ACC_INT_RDY)
    {
      // set a hal event and polling for shake event happend and then set application event 
      osal_set_event (Hal_TaskID, BSP_ACC_INT_EVENT);
    }
  }
  */
  // ---------------------------------------------------------------------------------------------//
  // bsp cap io interrupt process
  if (BSP_CAP_IO_PXIFG & BSP_CAP_IO_BIT)
  { 
    BSP_CAP_IO_PXIFG &= ~BSP_CAP_IO_BIT;

    bsp_cap_io_cnt ++;
  }
  
  // need to clear port1 interrupt flag in IRCON2
  P1IRCON &= ~P1IRCON_BIT;
  
  CLEAR_SLEEP_MODE();
  HAL_EXIT_ISR();

  return;
}


HAL_ISR_FUNCTION( Timer1IntIsr ,T1_VECTOR)
{
  HAL_ENTER_ISR();
  IRCON &= ~0x02;       //清除定时器1中断标志
  T1STAT &= ~0x01;
  
  // ---------------------------------------------------------------------------------------------//
  
  BSP_CAP_IO_ICTL &= ~BSP_CAP_IO_ICTLBIT;//关闭io中断
  
  CLEAR_SLEEP_MODE();
  HAL_EXIT_ISR();

  return;
}


/***************************************************************************************************
***************************************************************************************************/




