/**************************************************************************************************
  Filename:       bsp_color_sensor.c
  Revised:        $Date: 2015/6/29 $
  Revision:       $Revision: 1 $

  Description:    This file contains the interface to the Bsp Color Sensor Service.
                  Colomo hires TCS3414 Digital Color Sensor via an I2C bus.

  Copyright 2015 - 2016 QijiTek. All rights reserved.
**************************************************************************************************/
#ifndef BSP_COLOR_SENSOR_H
#define BSP_COLOR_SENSOR_H

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
#define TCS_I2C_ADDR             0x39

// ----------------------------------------------------------
// TCS3414 registers
// ----------------------------------------------------------
#define TCS_CONTROL_REG          0x00
#define TCS_TIMING_REG           0x01
#define TCS_INTERRUPT_REG        0x02
#define TCS_INTSOURCE_REG        0x03
#define TCS_ID_REG               0x04
#define TCS_GAIN_REG             0x07
#define TCS_LOW_THRESH_L_REG     0x08
#define TCS_LOW_THRESH_H_REG     0x09
#define TCS_HIGH_THRESH_L_REG    0x0A
#define TCS_HIGH_THRESH_H_REG    0x0B
#define TCS_DATABLOCK_REG        0x0F
#define TCS_DATA1LOW_REG         0x10
#define TCS_DATA1HIGH_REG        0x11
#define TCS_DATA2LOW_REG         0x12
#define TCS_DATA2HIGH_REG        0x13
#define TCS_DATA3LOW_REG         0x14
#define TCS_DATA3HIGH_REG        0x15
#define TCS_DATA4LOW_REG         0x16
#define TCS_DATA4HIGH_REG        0x17

// ----------------------------------------------------------
// TCS3414 TIMING reg fields
// ----------------------------------------------------------
#define TCS_INTEG_MODE_FREE      0x00
#define TCS_INTEG_MODE_MANUAL    0x01
#define TCS_INTEG_MODE_NOMINAL   0x02
#define TCS_INTEG_MODE_PULSE     0x03

#define TCS_INTEG_MODE_12MS      0x00
#define TCS_INTEG_MODE_100MS     0x01
#define TCS_INTEG_MODE_400MS     0x02

#define TCS_INTEG_MODE_1         0x00
#define TCS_INTEG_MODE_2         0x01
#define TCS_INTEG_MODE_4         0x02
#define TCS_INTEG_MODE_8         0x03
#define TCS_INTEG_MODE_16        0x04
#define TCS_INTEG_MODE_32        0x05
#define TCS_INTEG_MODE_64        0x06
#define TCS_INTEG_MODE_128       0x07
#define TCS_INTEG_MODE_256       0x08

// ----------------------------------------------------------
// TCS3414 GAIN reg fields
// ----------------------------------------------------------
#define TCS_GAIN_1X              0x00
#define TCS_GAIN_4X              0x01
#define TCS_GAIN_16X             0x02
#define TCS_GAIN_64X             0x03
#define TCS_PRESCALER_1          0x00
#define TCS_PRESCALER_2          0x01
#define TCS_PRESCALER_4          0x02
#define TCS_PRESCALER_8          0x03
#define TCS_PRESCALER_16         0x04
#define TCS_PRESCALER_32         0x05
#define TCS_PRESCALER_64         0x06

/*********************************************************************
 * CONSTANTS
 */
#define BSP_CS_NO_TASK       0xff
#define BSP_CS_NO_EVENT      0x0000
  
/*********************************************************************
 * TYPEDEFS
 */
#define BSP_CS_START_READ()      st(osal_set_event(Hal_TaskID, BSP_CS_LED_START_EVENT);)
  
/*********************************************************************
 * GLOBAL VARIABLES
 */  
extern uint8  BspCsDelegateTaskID;
extern uint16 BspCsDelegateEvent;  

extern uint16 red, green, blue, all;

extern void BspColorSensorDelayMS(uint8 msec);

/*
 * Color sensor initialization
 */  
extern void BspColorSensorInit(void);
  
/*
 * Color sensor initialization
 */  
extern uint8 BspColorSensorDelegate(uint8 task_id, uint16 event);

/*
 * Color sensor configuration
 */
extern void BspColorSensorConfig(uint8 mode, uint8 param, uint8 gain, uint8 prescaler);

/*
 * Color sensor read color data (master needs to wait enough time to read the valid data)
 */
extern void BspColorSensorReadColor(void);

/*
 * Turn on leds and sync signals for a certain period
 */
extern void BspColorSensorSyncLed(uint8 on);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif