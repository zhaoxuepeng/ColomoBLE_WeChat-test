/**************************************************************************************************
  Filename:       bsp_acc.c
  Revised:        $Date: 2015/7/5 $
  Revision:       $Revision: 1 $

  Description:    This file contains the interface to the accelerator.
                  
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
#include "bsp_acc.h"

/***************************************************************************************************
 *                                             CONSTANTS
 ***************************************************************************************************/

/***************************************************************************************************
 *                                              MACROS
 ***************************************************************************************************/

/***************************************************************************************************
 *                                              TYPEDEFS
 ***************************************************************************************************/

/***************************************************************************************************
 *                                           GLOBAL VARIABLES
 ***************************************************************************************************/
/* need to be shared with isr functions, so no static */
uint8 BspAccDelegateState  = BSP_ACC_INT_IDLE;
uint8 BspAccDelegateTaskID = BSP_ACC_INT_NO_TASK;
uint8 BspAccDelegateEvent  = BSP_ACC_INT_NO_EVENT;

uint8 BspAccCntFlg = 0;
uint8 BspAccShkCnt = 0;
/***************************************************************************************************
 *                                            LOCAL FUNCTION
 ***************************************************************************************************/

/***************************************************************************************************
 *                                            FUNCTIONS - API
 ***************************************************************************************************/

/***************************************************************************************************
 * @fn      BspAccReadData
 * @brief   Read acc reg data from LIS3DH
 *
 * @param   addr: reg address in LIS3DH
 *
 * @return  reg value
 *
 ***************************************************************************************************/
uint8 BspAccReadData(uint8 addr)
{
  uint8  pBuf=0;
  HalI2CInit(ACC_I2C_ADDR, i2cClock_123KHZ);
  HalI2CReads( addr, 1, &pBuf);
  return pBuf;
}

/***************************************************************************************************
 * @fn      BspAccWriteData
 * @brief   Write acc reg data to LIS3DH
 *
 * @param   addr: reg address in LIS3DH
 *          value: reg value to be written
 *
 * @return
 *
 ***************************************************************************************************/
void BspAccWriteData(uint8 addr, uint8 value)
{
  uint8  pBuf=0;
  pBuf = value;
  HalI2CInit(ACC_I2C_ADDR, i2cClock_123KHZ);
  HalI2CWrites( addr, 1, &pBuf);
}

/***************************************************************************************************
 * @fn      BspAccInit
 * @brief   Initial the accelerate meter interrupt pins (Need to call in Hal initial session)
 * @param   None          
 *
 * @return  None
 ***************************************************************************************************/
void BspAccInit(void){
  BSP_ACC_INT_SEL &= ~BSP_ACC_INT_BIT;      /* Set pin function to GPIO */
  BSP_ACC_INT_DIR &= ~BSP_ACC_INT_BIT;      /* Set pin direction to Input */

  BSP_ACC_INT_ICTL |= BSP_ACC_INT_ICTLBIT;  /* enable interrupt generation at port 1 */
  BSP_ACC_INT_IEN  |= BSP_ACC_INT_IENBIT;   /* enable CPU interrupt for port 1 */
  BSP_ACC_INT_PXIFG = 0;                    /* Clear any pending interrupt */
  
  BspAccDelegateState |= BSP_ACC_INT_INIT;
}

/***************************************************************************************************
 * @fn      BspAccIntDelegate
 * @brief   Delegate task id to bsp accelerator service (Need to call in ColomoBLE task initial session)
 * @param   None          
 *
 * @return  None
 ***************************************************************************************************/
uint8 BspAccIntDelegate(uint8 task_id, uint16 event)
{
  if ( BspAccDelegateTaskID == BSP_ACC_INT_NO_TASK )
  {
    BspAccDelegateTaskID = task_id;
    BspAccDelegateEvent  = event;

    BspAccDelegateState |= BSP_ACC_INT_CFG;     /* Bsp Acc interrupt is configured */
    return ( true );
  }
  else
    return ( false );
}

/***************************************************************************************************
 * @fn      BspAccConfig
 * @brief   Configure LIS3DH accelerate meter (call this function in application)
 * @param   state: on/off          
 *
 * @return  None
 ***************************************************************************************************/
void BspAccConfig(uint8 state)
{
  // clear the LIS3DH interrupt 1 flag
  VOID BspAccReadData(INT1_SRC);
  
  if ( state == BSP_ACC_ON )
  {
    BspAccCntFlg = 0;
    BspAccShkCnt = 0;
    BspAccWriteData(CTRL_REG1,0x2f);          // Turn on the sensor, enable X,Y, and Z, ODR=10Hz
    BspAccWriteData(CTRL_REG2,0xf9);          // High-pass filter enabled on data and interrupt,auto reset
    BspAccWriteData(CTRL_REG3,0x40);          // Interrupt driven to INT1 pad
    BspAccWriteData(CTRL_REG4,0x00);          // FS = 2g
    BspAccWriteData(CTRL_REG5,0x08);          // Interrupt latched
    BspAccWriteData(CTRL_REG6,0x00);          // Interrupt active high
    BspAccWriteData(INT1_THS, 0x20);          // Threshold = 0.5g
    BspAccWriteData(INT1_DUR, 0x00);          // Duration = 0
    BspAccReadData (REFERENCE     );
    BspAccWriteData(INT1_CFG, 0x2a);          // Configure desired wake-up event
    BspAccDelegateState |= BSP_ACC_INT_OPEN;  // set int state to open
 
#if (defined DBG_UART)
    printf("[BSP ACC] WHO_AM_I=%x\r\n", BspAccReadData(WHO_AM_I));
#endif
  }
  else
  {
    BspAccWriteData(CTRL_REG1,0x00);          // Power down
    BspAccDelegateState &= ~BSP_ACC_INT_OPEN; // close the acc interrupt
  }
}

/***************************************************************************************************
 * @fn      BspAccReadMotion
 * @brief   Read LIS3DH accelerate meter value
 * @param   x, y, z accelerate value          
 *
 * @return  None
 ***************************************************************************************************/
void BspAccReadMotion(uint16 *x_val, uint16 *y_val, uint16 *z_val){
  uint8 x_val_lo = 0;
  uint8 x_val_hi = 0;
  uint8 y_val_lo = 0;
  uint8 y_val_hi = 0;
  uint8 z_val_lo = 0;
  uint8 z_val_hi = 0;

  HalI2CInit(ACC_I2C_ADDR, i2cClock_123KHZ);
  HalI2CReads( OUTX_L, 1, &x_val_lo);
  HalI2CReads( OUTX_H, 1, &x_val_hi);
  HalI2CReads( OUTY_L, 1, &y_val_lo);
  HalI2CReads( OUTY_H, 1, &y_val_hi);
  HalI2CReads( OUTZ_L, 1, &z_val_lo);
  HalI2CReads( OUTZ_H, 1, &z_val_hi);
  *x_val = BUILD_UINT16(x_val_lo,x_val_hi);
  *y_val = BUILD_UINT16(y_val_lo,y_val_hi);
  *z_val = BUILD_UINT16(z_val_lo,z_val_hi);
}

/***************************************************************************************************
***************************************************************************************************/
