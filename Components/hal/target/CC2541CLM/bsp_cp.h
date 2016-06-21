/**************************************************************************************************
  Filename:       bsp_cp.h
  Revised:        $Date: 2015/7/5 $
  Revision:       $Revision: 1 $

  Description:    This file contains the interface to the Bsp Coprocessor Service.
                  Colomo hires STM32F030 as coprocessor via an I2C bus.

  Copyright 2015 - 2016 QijiTek. All rights reserved.
**************************************************************************************************/
#ifndef BSP_CP_H
#define BSP_CP_H

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
#define CP_I2C_ADDR             0x01

// ----------------------------------------------------------
// Coprocessor registers
// ----------------------------------------------------------
#define CP_INTCTRL_REG   0x00
#define CP_INTFLAG_REG   0x01
#define CP_LEDCTRL_REG   0x02

#define CP_CAPCTRL_REG   0x10
#define CP_CAPTHSH_HREG  0x11
#define CP_CAPTHSH_LREG  0x12
#define CP_CAPSTBL_HREG  0x13
#define CP_CAPSTBL_LREG  0x14
#define CP_CAPDATA_HREG  0x15
#define CP_CAPDATA_LREG  0x16
#define CP_CAPAVRG_HREG  0x17
#define CP_CAPAVRG_LREG  0x18
#define CP_INDCTRL_REG   0x20
#define CP_INDTHSH_HREG  0x21
#define CP_INDTHSH_LREG  0x22
#define CP_INDSTBL_HREG  0x23
#define CP_INDSTBL_LREG  0x24
#define CP_INDDATA_HREG  0x25
#define CP_INDDATA_LREG  0x26
#define CP_INDAVRG_HREG  0x27
#define CP_INDAVRG_LREG  0x28
  
// ----------------------------------------------------------
// Coprocessor controlled system led
// ----------------------------------------------------------
 /* System led display color */
#define CP_SYS_LEDR         0x04
#define CP_SYS_LEDG         0x02
#define CP_SYS_LEDB         0x01
#define CP_SYS_LEDY         (CP_SYS_LEDR | CP_SYS_LEDG)
#define CP_SYS_LEDC         (CP_SYS_LEDG | CP_SYS_LEDB)
#define CP_SYS_LEDP         (CP_SYS_LEDR | CP_SYS_LEDB)
#define CP_SYS_LEDW         (CP_SYS_LEDR | CP_SYS_LEDG | HAL_SYS_LEDB)
/* System led display mode */
#define CP_SYS_LED_NBL_NFD  0x00
#define CP_SYS_LED_NBL_FD   0x01
#define CP_SYS_LED_BL_NFD   0x02
#define CP_SYS_LED_BL_FD    0x03
/* System led blink count */
#define CP_SYS_LED_BL_1     0x00
#define CP_SYS_LED_BL_3     0x01
#define CP_SYS_LED_BL_5     0x02
#define CP_SYS_LED_BL_7     0x03
#define CP_SYS_LED_BL_9     0x04
#define CP_SYS_LED_BL_11    0x05
#define CP_SYS_LED_BL_13    0x06
#define CP_SYS_LED_BL_15    0x07    
/* System led display state */
#define CP_SYS_LED_ON       0xff
#define CP_SYS_LED_OFF      0x00
#define CP_SYS_LED_NO_PARA  0x00

/* Bsp Cp Interrupt pin is assigned on P0_6 */
#define BSP_CP_INT_PORT     P0
#define BSP_CP_INT_BIT      BV(6)
#define BSP_CP_INT_SEL      P0SEL
#define BSP_CP_INT_DIR      P0DIR
#define BSP_CP_INT_SBIT     P0_6

#define BSP_CP_INT_IEN      IEN1  /* CPU interrupt mask register */
#define BSP_CP_INT_IENBIT   BV(5) /* Mask bit for all of Port_0 */

#define BSP_CP_INT_ICTL     P0IEN /* Port Interrupt Control register */
#define BSP_CP_INT_ICTLBIT  BV(6) /* P0IEN - P0.6 enable/disable bit */

#define BSP_CP_INT_PXIFG    P0IFG /* Interrupt flag at source */

#define BSP_CP_INT_NO_TASK  0xff
#define BSP_CP_INT_NO_EVENT 0x0000
  
#define BSP_CP_RESET        0x10
#define BSP_CP_CALIBRATE    0x03

/*********************************************************************
 * CONSTANTS
 */
#define BSP_CP_INT_IDLE     0x00
#define BSP_CP_INT_INIT     0x01
#define BSP_CP_INT_CFG      0x02
#define BSP_CP_INT_RDY      0x03

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
extern uint8 BspCpDelegateState;
extern uint8 BspCpDelegateTaskID;
extern uint8 BspCpDelegateEvent;

extern uint16 cap_val;
extern uint16 ind_val;

/*********************************************************************
 * GLOBAL Functions
 */
extern void  BspCpInit         (void);
extern uint8 BspCpIntDelegate  (uint8 task_id, uint16 event);
extern void  BspCpReadData     (/*uint16 *cap_val, uint16 *ind_val, */uint8 clr_flg);
extern void  BspCpSetLed       (uint8 state, uint8 bl_cnt, uint8 mode, uint8 color);
extern void  BspCpCommand      (uint8 command);
/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif