/**************************************************************************************************
  Filename:       bsp_ruby_led.h
  Revised:        $Date: 2015/9/10 $
  Revision:       $Revision: 1 $

  Description:    This file contains the interface to the capacity measurement.


  Copyright 2015 - 2016 QijiTek. All rights reserved.
**************************************************************************************************/

#ifndef BSP_RUBY_TIMER_H
#define BSP_RUBY_TIMER_H

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
  

extern void BspRubyTimer4Init(void);
extern void BspRubyTimer4Start(void);
extern void BspRubyTimer4Stop(void);
extern void BspRubyTimer1Start(void);
extern void BspRubyTimer1Stop(void);
/*
 * Switch Pwm
 */
//extern void BspRubyMotorChangePwm( uint8 pwm );


/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif
