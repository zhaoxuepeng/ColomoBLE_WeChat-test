/**************************************************************************************************
  Filename:       bsp_smart_cover.h
  Revised:        $Date: 2015/7/7 $
  Revision:       $Revision: 1 $

  Description:    This file contains the interface to the Bsp Smart Cover Service.

  Copyright 2015 - 2016 QijiTek. All rights reserved.
**************************************************************************************************/
#ifndef BSP_SMART_COVER_H
#define BSP_SMART_COVER_H

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
/* Bsp Smart Cover Interrupt pin is assigned on P0_7 */
#define BSP_SC_INT_PORT     P0
#define BSP_SC_INT_BIT      BV(7)
#define BSP_SC_INT_SEL      P0SEL
#define BSP_SC_INT_DIR      P0DIR

#define BSP_SC_INT_IEN      IEN1  /* CPU interrupt mask register */
#define BSP_SC_INT_IENBIT   BV(5) /* Mask bit for all of Port_0  */

#define BSP_SC_INT_ICTL     P0IEN /* Port Interrupt Control register */
#define BSP_SC_INT_ICTLBIT  BV(7) /* P0IEN - P0.7 enable/disable bit */

#define BSP_SC_INT_PXIFG    P0IFG /* Interrupt flag at source */

#define BSP_SC_INT_NO_TASK  0xff
#define BSP_SC_INT_NO_EVENT 0x0000

#define BSP_SC_PIN_STATE    P0_7  /* Smart cover input pin */
#define BSP_SC_HIGH         1
#define BSP_SC_LOW          0

/*********************************************************************
 * CONSTANTS
 */
#define BSP_SC_INT_IDLE     0x00
#define BSP_SC_INT_INIT     0x01
#define BSP_SC_INT_CFG      0x02
#define BSP_SC_INT_RDY      0x03

#define BSP_SC_STATE_CHANGE 0x01 /* used in ColomoBLE application message type */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
extern uint8 BspScDelegateState;
extern uint8 BspScDelegateTaskID;
extern uint8 BspScDelegateEvent;
extern uint8 BspScVirtualCnt;

/*********************************************************************
 * GLOBAL FUNCTIONS
 */
extern void  BspSmartCoverInit       (void);
extern uint8 BspSmartCoverIntDelegate(uint8 task_id, uint16 event1, uint16 event2, uint16 event3);
extern void  BspSmartCoverProcess    (uint8 pin_state);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif