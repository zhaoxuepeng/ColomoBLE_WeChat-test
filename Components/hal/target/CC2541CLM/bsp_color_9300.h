/**************************************************************************************************
  Filename:       bsp_color_sensor.c
  Revised:        $Date: 2015/6/29 $
  Revision:       $Revision: 1 $

  Description:    This file contains the interface to the Bsp Color Sensor Service.
                  Colomo hires TCS3414 Digital Color Sensor via an I2C bus.

  Copyright 2015 - 2016 QijiTek. All rights reserved.
**************************************************************************************************/
#ifndef BSP_COLOR_9300_H
#define BSP_COLOR_9300_H

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
#define APDS_I2C_ADDR             0x39

extern uint8  BspCsDelegateTaskID;
extern uint16 BspCsDelegateEvent;  
   
extern uint16 all;

extern void BspColorSensorDelayMS(uint8 msec);

extern void BspColorSensorInit(void);

extern uint8 BspColorSensorDelegate(uint8 task_id, uint16 event);

extern void BspColorSensorColorStart(void);
extern void BspColorSensorColorStop(void);
extern void BspColorSensorColorRead(void);


/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif