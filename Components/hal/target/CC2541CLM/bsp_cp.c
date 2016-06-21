/**************************************************************************************************
  Filename:       bsp_cp.c
  Revised:        $Date: 2015/7/5 $
  Revision:       $Revision: 1 $

  Description:    This file contains the interface to the Bsp Coprocessor Service.
                  Colomo hires STM32F030 as the coprocessor via an I2C bus.

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
#include "bsp_cp.h"

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
uint8 BspCpDelegateState  = BSP_CP_INT_IDLE;
uint8 BspCpDelegateTaskID = BSP_CP_INT_NO_TASK;
uint8 BspCpDelegateEvent  = BSP_CP_INT_NO_EVENT;

uint16 cap_val, ind_val;

/***************************************************************************************************
 *                                            LOCAL FUNCTION
 ***************************************************************************************************/

/***************************************************************************************************
 *                                            FUNCTIONS - API
 ***************************************************************************************************/

/***************************************************************************************************
 * @fn      BspCpInit
 * @brief   Initialize CC2541 interrupt resource (call in board level initial session)
 * @param   None          
 *
 * @return  None
 ***************************************************************************************************/
void BspCpInit(void)
{
  BSP_CP_INT_SEL &= ~BSP_CP_INT_BIT;      /* Set pin function to GPIO */
  BSP_CP_INT_DIR &= ~BSP_CP_INT_BIT;      /* Set pin direction to Input */

  BSP_CP_INT_ICTL |= BSP_CP_INT_ICTLBIT;  /* enable interrupt generation at port 1 */
  BSP_CP_INT_IEN  |= BSP_CP_INT_IENBIT;   /* enable CPU interrupt for port 1 */
  BSP_CP_INT_PXIFG = 0;                   /* Clear any pending interrupt */

  cap_val = 0; ind_val = 0;
  
  BspCpDelegateState |= BSP_CP_INT_INIT;  /* Bsp Cp interrupt is initialed */
}

/***************************************************************************************************
 * @fn      BspCpIntDelegate
 * @brief   Delegate task id to bsp cp service (Need to call in ColomoBLE task initial session)
 * @param   None          
 *
 * @return  None
 ***************************************************************************************************/
uint8 BspCpIntDelegate(uint8 task_id, uint16 event)
{
  if ( BspCpDelegateTaskID == BSP_CP_INT_NO_TASK )
  {
    BspCpDelegateTaskID = task_id;
    BspCpDelegateEvent  = event;

    BspCpDelegateState |= BSP_CP_INT_CFG;     /* Bsp Cp interrupt is configured */
    return ( true );
  }
  else
    return ( false );
}

/***************************************************************************************************
 * @fn      BspCpReadData
 * @brief   Read Cap and Ind data from STM32
 *
 * @param   cap_val: the capacity frequency
 *          ind_val: the inductance frequency
 *          clr_flg: clear the cp interrupt flag or not
 *
 * @return  None
 ***************************************************************************************************/
void BspCpReadData(/*uint16 *cap_val, uint16 *ind_val, */uint8 clr_flg)
{
  uint8  pBuf;

  HalI2CInit(CP_I2C_ADDR, i2cClock_123KHZ);

  HalI2CReads( 0x15, 1, &pBuf); // read cap data high
  cap_val = pBuf;
  HalI2CReads( 0x16, 1, &pBuf); // read cap data low
  cap_val = cap_val<<8 | pBuf;

  HalI2CReads( 0x25, 1, &pBuf); // read ind data high
  ind_val = pBuf;
  HalI2CReads( 0x26, 1, &pBuf); // read ind data low
  ind_val = ind_val<<8 | pBuf;
  
  // Clear CP interrupt flag if needed
  if(clr_flg == TRUE)
  {
    pBuf = 0;
    HalI2CWrites( 0x01, 1, &pBuf);
  }

#if (defined DBG_UART)
  printf("[BSP_CP] cap_val=%d, ind_val=%d\r\n", cap_val, ind_val);
#endif
}

/***************************************************************************************************
 * @fn      BspCpCommand
 * @brief   Send STM32 command (need to call after power on peri)
 *
 * @param   command
 *
 * @return  None
 ***************************************************************************************************/
void BspCpCommand(uint8 command)
{
  uint8 pBuf = 0x01;
  
  HalI2CInit(CP_I2C_ADDR, i2cClock_123KHZ);
  
  HalI2CWrites( command, 1, &pBuf);
}

/***************************************************************************************************
 * @fn      BspCpSetLed
 * @brief   Set System Led state and color
 *
 * @param   state: on/off
 *          bl_cnt: blink times
 *          color: select the led color
 *          mode:  fade/blink etc
 *
 * @return  None
 ***************************************************************************************************/
void BspCpSetLed(uint8 state, uint8 bl_cnt, uint8 mode, uint8 color)
{
  uint8  pBuf;

  HalI2CInit(CP_I2C_ADDR, i2cClock_123KHZ);

  pBuf = ((bl_cnt<<5) | (mode << 3) | color) & state; // set 0x02 reg: | BL_CNT[7:5] | BL BR | R G B |
  HalI2CWrites( 0x02, 1, &pBuf);

#if (defined DBG_UART)
#endif
}
/***************************************************************************************************
***************************************************************************************************/