/**************************************************************************************************
  Filename:       bsp_smart_cover.h
  Revised:        $Date: 2015/7/7 $
  Revision:       $Revision: 1 $

  Description:    This file contains the interface to the Bsp Smart Cover Service.

  Copyright 2015 - 2016 QijiTek. All rights reserved.
**************************************************************************************************/
#ifndef BSP_CAP_IO_H
#define BSP_CAP_IO_H

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
/* Bsp Cap IO Interrupt pin is assigned on P1_3 */
#define BSP_CAP_IO_PORT     P1
#define BSP_CAP_IO_BIT      BV(3)
#define BSP_CAP_IO_SEL      P1SEL
#define BSP_CAP_IO_DIR      P1DIR

#define BSP_CAP_IO_IEN      IEN2  /* CPU interrupt mask register */
#define BSP_CAP_IO_IENBIT   BV(4) /* Mask bit for all of Port_0  */

#define BSP_CAP_IO_ICTL     P1IEN /* Port Interrupt Control register */
#define BSP_CAP_IO_ICTLBIT  BV(3) /* P1IEN - P1.3 enable/disable bit */

#define BSP_CAP_IO_PXIFG    P1IFG /* Interrupt flag at source */

#define BSP_CAP_IO_NO_TASK  0xff
#define BSP_CAP_IO_NO_EVENT 0x0000

#define BSP_CAP_IO_STATE    P1_3  /* CAP IO input pin */

/*********************************************************************
 * CONSTANTS
 */
#define BSP_CAP_IO_IDLE     0x00
#define BSP_CAP_IO_INIT     0x01
#define BSP_CAP_IO_CFG      0x02
#define BSP_CAP_IO_RDY      0x03

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
extern uint8 BspCapIODelegateState;
extern uint8 BspCapIODelegateTaskID;
extern uint8 BspCapIODelegateEvent;

extern uint16 bsp_cap_io_cnt;

/*********************************************************************
 * GLOBAL FUNCTIONS
 */
extern void  BspCapIOInit       (void);
extern uint8 BspCapIODelegate   (uint8 task_id, uint16 event);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif