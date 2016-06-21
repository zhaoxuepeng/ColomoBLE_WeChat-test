/**************************************************************************************************
  Filename:       bsp_color_sensor.c
  Revised:        $Date: 2015/6/29 $
  Revision:       $Revision: 1 $

  Description:    This file contains the interface to the Bsp Color Sensor Service.
                  Colomo hires TCS3414 Digital Color Sensor via an I2C bus.

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
#include "bsp_color_sensor.h"
#include "hal_board_cfg.h"

/***************************************************************************************************
 *                                             CONSTANTS
 ***************************************************************************************************/

/***************************************************************************************************
 *                                              MACROS
 ***************************************************************************************************/
#define CMD_BYTE  0x00
#define CMD_WORD  0x01
#define CMD_BLOCK 0x02
#define CMD_CLEAR 0x03

#define CMD(_len_, _addr_) (0x80 | ((_len_)<<5) | (_addr_))

/***************************************************************************************************
 *                                              TYPEDEFS
 ***************************************************************************************************/

/***************************************************************************************************
 *                                           GLOBAL VARIABLES
 ***************************************************************************************************/
uint8  BspCsDelegateTaskID = BSP_CS_NO_TASK;
uint16 BspCsDelegateEvent  = BSP_CS_NO_EVENT;

uint16 red, green, blue, all;

/***************************************************************************************************
 *                                            LOCAL FUNCTION
 ***************************************************************************************************/
void BspColorSensorDelayMS(uint8 msec)
{ 
    volatile uint16 i,j;
    
    for (i=0; i<msec; i++)
        for (j=0; j<475; j++);
}
/***************************************************************************************************
 *                                            FUNCTIONS - API
 ***************************************************************************************************/

/***************************************************************************************************
 * @fn      BspColorSensorInit
 *
 * @brief   Initialize TCS3414 (call this function at hal initial)
 *
 * @param   None        
 *
 * @return  None
 ***************************************************************************************************/
void BspColorSensorInit(void)
{
  red=0; green=0; blue=0; all=0;

  BSP_CLRLED_SBIT = 0;
}


/***************************************************************************************************
 * @fn      BspColorSensorDelegate
 * @brief   Delegate task id to bsp cs service (Need to call in ColomoBLE task initial session)
 * @param   None          
 *
 * @return  None
 ***************************************************************************************************/
uint8 BspColorSensorDelegate(uint8 task_id, uint16 event)
{
  if ( BspCsDelegateTaskID == BSP_CS_NO_TASK )
  {
    BspCsDelegateTaskID = task_id;
    BspCsDelegateEvent  = event;
    
    return ( true );
  }
  else
    return ( false );
}

/***************************************************************************************************
 * @fn      BspColorSensorConfig
 *
 * @brief   Configure TCS3414 (call this function at application initial)
 *
 * @param   mode: select TCS3414 running mode from {free run, manually, nominal and pulse cnt}
 *          param: select integration time of pulse cnt
 *          gain: select ADC gain
 *          prescaler: select ADC sensitivity by prescaling divider          
 *
 * @return  None
 ***************************************************************************************************/
void BspColorSensorConfig(uint8 mode, uint8 param, uint8 gain, uint8 prescaler)
{
  uint8 pBuf;

  HalI2CInit(TCS_I2C_ADDR, i2cClock_123KHZ);

  // Power up the TCS3414
  pBuf = 0x01;
  HalI2CWrites( CMD(CMD_BYTE, TCS_CONTROL_REG), 1, &pBuf );

  // Configure the TIMING reg
  pBuf = ((mode<<4) | param);
  HalI2CWrites( CMD(CMD_BYTE, TCS_TIMING_REG),  1, &pBuf );

  // Configure the GAIN reg
  pBuf = ((gain<<4) | prescaler);
  HalI2CWrites( CMD(CMD_BYTE, TCS_GAIN_REG),    1, &pBuf );

  // Enable the color sensor ADC
  pBuf = 0x03;
  HalI2CWrites( CMD(CMD_BYTE, TCS_CONTROL_REG), 1, &pBuf );

#if defined( DBG_UART )
//  HalI2CReads( CMD(CMD_BYTE, TCS_ID_REG), 1, &pBuf );
//  printf("[BSP CS] ID=%x\r\n", pBuf);
//  HalI2CReads( CMD(CMD_BYTE, TCS_CONTROL_REG), 1, &pBuf );
//  printf("[BSP CS] Control=%x\r\n", pBuf);
#endif
}

/***************************************************************************************************
 * @fn      BspColorSensorSyncLed
 *
 * @brief   Output Led control signal and sync signal for a certain period
 *
 * @param   led1 or led2 channels
 *
 * @return  None
 ***************************************************************************************************/
void BspColorSensorSyncLed(uint8 on)
{ 
  BSP_CLRLED_SBIT = on;
}

/***************************************************************************************************
 * @fn      BspColorSensorReadColor
 *
 * @brief   Read the color data from four channels
 *
 * @param   red, green, blue and all are the 4 channels          
 *
 * @return  None
 ***************************************************************************************************/
void BspColorSensorReadColor(void)
{
  uint8 pBufL=0, pBufH=0;

  HalI2CInit(TCS_I2C_ADDR, i2cClock_123KHZ);
  
  // TODO: block read is more efficient but not work correctly now
  //       so we use byte read instead
  //--------------------------------------------------------------------------//
  // green value
  //--------------------------------------------------------------------------//
  HalI2CReads( CMD(CMD_BYTE, TCS_DATA1LOW_REG), 1, &pBufL);
  HalI2CReads( CMD(CMD_BYTE, TCS_DATA1HIGH_REG), 1, &pBufH);

  green = ((uint16)pBufH << 8) | pBufL;
  
  //--------------------------------------------------------------------------//
  // red value
  //--------------------------------------------------------------------------//
  HalI2CReads( CMD(CMD_BYTE, TCS_DATA2LOW_REG), 1, &pBufL);
  HalI2CReads( CMD(CMD_BYTE, TCS_DATA2HIGH_REG), 1, &pBufH);

  red = ((uint16)pBufH << 8) | pBufL;

  //--------------------------------------------------------------------------//
  // blue value
  //--------------------------------------------------------------------------//
  HalI2CReads( CMD(CMD_BYTE, TCS_DATA3LOW_REG), 1, &pBufL);
  HalI2CReads( CMD(CMD_BYTE, TCS_DATA3HIGH_REG), 1, &pBufH);

  blue = ((uint16)pBufH << 8) | pBufL;

  //--------------------------------------------------------------------------//
  // clear value (no filter)
  //--------------------------------------------------------------------------//
  HalI2CReads( CMD(CMD_BYTE, TCS_DATA4LOW_REG), 1, &pBufL);
  HalI2CReads( CMD(CMD_BYTE, TCS_DATA4HIGH_REG), 1, &pBufH);

  all = ((uint16)pBufH << 8) | pBufL;
}

/***************************************************************************************************
***************************************************************************************************/




