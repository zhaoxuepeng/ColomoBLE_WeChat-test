/**************************************************************************************************
  Filename:       bsp_ruby_led.h
  Revised:        $Date: 2015/9/10 $
  Revision:       $Revision: 1 $

  Description:    This file contains the interface to the capacity measurement.


  Copyright 2015 - 2016 QijiTek. All rights reserved.
**************************************************************************************************/

#ifndef BSP_RUBY_LED_H
#define BSP_RUBY_LED_H

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
 * Lighten LED
 */
extern void BspRubyLedOn(uint8 PWM);

/*
 * Switch off LED
 */
extern void BspRubyLedOff();



/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif
