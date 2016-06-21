/**************************************************************************************************
  Filename:       bsp_ACC.h
  Revised:        $Date: 2015/7/5 $
  Revision:       $Revision: 1 $

  Description:    This file contains the interface to the LIS3DH.


  Copyright 2015 - 2016 QijiTek. All rights reserved.
**************************************************************************************************/
#ifndef BSP_ACC_H
#define BSP_ACC_H

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
#define         ACC_I2C_ADDR            0x19

// ----------------------------------------------------------
// ACC registers
// ----------------------------------------------------------
#define 	CTRL_REG1		0x20
#define		CTRL_REG2		0x21
#define		CTRL_REG3		0x22
#define 	CTRL_REG4		0x23
#define 	CTRL_REG5		0x24
#define 	CTRL_REG6		0x25
#define 	REFERENCE		0x26
#define 	INT1_THS		0x32
#define 	INT1_DUR		0x33
#define 	INT1_CFG		0x30
#define         INT1_SRC                0x31

#define 	STATUS_REG_AUX 	        0x07
#define		STATUS_REG2 	        0x27
#define 	OUTX_L			0x28
#define 	OUTX_H			0x29
#define 	OUTY_L			0x2a
#define 	OUTY_H			0x2b
#define 	OUTZ_L			0x2c
#define 	OUTZ_H			0x2d
#define		WHO_AM_I		0x0f
 

/*********************************************************************
 * CONSTANTS
 */

/* Bsp Acc Interrupt_1 pin is assigned on P1_7 */
#define BSP_ACC_INT_PORT     P1
#define BSP_ACC_INT_BIT      BV(7)
#define BSP_ACC_INT_SEL      P1SEL
#define BSP_ACC_INT_DIR      P1DIR

#define BSP_ACC_INT_IEN      IEN2  /* CPU interrupt mask register */
#define BSP_ACC_INT_IENBIT   BV(4) /* Mask bit for all of Port_1 */

#define BSP_ACC_INT_ICTL     P1IEN /* Port Interrupt Control register */
#define BSP_ACC_INT_ICTLBIT  BV(7) /* P1IEN - P1.7 enable/disable bit */

#define BSP_ACC_INT_PXIFG    P1IFG /* Interrupt flag at source */

#define BSP_ACC_INT_NO_TASK  0xff
#define BSP_ACC_INT_NO_EVENT 0x0000

#define BSP_ACC_INT_IDLE     0x00
#define BSP_ACC_INT_INIT     0x01
#define BSP_ACC_INT_CFG      0x02
#define BSP_ACC_INT_OPEN     0x04	
#define BSP_ACC_INT_RDY      0x07

#define BSP_ACC_OFF          0x00
#define BSP_ACC_ON           0x01
  
#define BSP_ACC_SHK_CNT_TH   3
/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
extern uint8 BspAccDelegateState;
extern uint8 BspAccDelegateTaskID;
extern uint8 BspAccDelegateEvent;

extern uint8 BspAccCntFlg;
extern uint8 BspAccShkCnt;

/*********************************************************************
 * GLOBAL Functions
 */
extern void     BspAccInit              (void);
extern uint8    BspAccIntDelegate       (uint8 task_id, uint16 event);
extern void     BspAccConfig            (uint8 state);
extern void  	BspAccReadMotion        (uint16 *x_val, uint16 *y_val, uint16 *z_val);
extern uint8    BspAccReadData          (uint8 addr);
extern void     BspAccWriteData         (uint8 addr, uint8 value);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif
