/**************************************************************************************************
  Filename:       bsp_ruby_led.h
  Revised:        $Date: 2015/9/10 $
  Revision:       $Revision: 1 $

  Description:    This file contains the interface to the capacity measurement.


  Copyright 2015 - 2016 QijiTek. All rights reserved.
**************************************************************************************************/

#ifndef BSP_RUBY_MOTOR_H
#define BSP_RUBY_MOTOR_H

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
  
extern uint8 pwm_change_value;
extern void BspRubyMotorInit(void);
extern uint8 BspRubyMotorInitDelegate(uint8 task_id, uint16 event);
extern void BspRubyMotorOpen(uint8 pwm);
extern void BspRubyMotorClose(void);
extern void BspRubyMotorTimer(void);
extern void BspRubyMotorChangePwm( uint8 pwm );


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
