/**************************************************************************************************
  Filename:       bsp_power.c
  Revised:        $Date: 2015/6/16 $
  Revision:       $Revision: 1 $

  Description:    This file contains the interface to the Hal Power Control Service.


  Copyright 2015 - 2016 QijiTek. All rights reserved.
**************************************************************************************************/
#ifndef BSP_POWER_H
#define BSP_POWER_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "hal_board.h"

/*********************************************************************
 * MACROS
 */
#define BSP_PERI_PWR_ON()  BSP_TURN_ON_PERI_PWR()
#define BSP_PERI_PWR_OFF() BSP_TURN_OFF_PERI_PWR()
  
/* Bsp usb plug in pin is assigned on P0_4 */
#define BSP_USB_IN_PORT     P0
#define BSP_USB_IN_BIT      BV(4)
#define BSP_USB_IN_SEL      P0SEL
#define BSP_USB_IN_DIR      P0DIR

#define BSP_USB_IN_IEN      IEN1  /* CPU interrupt mask register */
#define BSP_USB_IN_IENBIT   BV(5) /* Mask bit for all of Port_0  */

#define BSP_USB_IN_ICTL     P0IEN /* Port Interrupt Control register */
#define BSP_USB_IN_ICTLBIT  BV(4) /* P0IEN - P0.7 enable/disable bit */

#define BSP_USB_IN_PXIFG    P0IFG /* Interrupt flag at source */

#define BSP_USB_IN_NO_TASK  0xff
#define BSP_USB_IN_NO_EVENT 0x0000

#define BSP_USB_PIN_STATE   P0_4  /* usb plug input pin */  
#define BSP_USB_IN          1
#define BSP_USB_OUT         0
  
/* Bsp usb charge finish pin is assigned on P0_5 */
#define BSP_CHRG_PIN_STATE  P0_5
#define BSP_CHRG_FINISH     1
#define BSP_CHRG_NOT_FINISH 0

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*
 * Initialize Colomo Power Control Service.
 */
extern void BspPwrCtrlInit       ( void );

extern void BspUsbChargerInit    ( void );

extern void BspUsbChargerProcess ( uint8 pin_state, uint8 chrg_state );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif