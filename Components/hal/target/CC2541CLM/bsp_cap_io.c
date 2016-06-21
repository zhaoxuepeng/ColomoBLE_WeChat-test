/**************************************************************************************************
  Filename:       bsp_smart_cover.c
  Revised:        $Date: 2015/7/7 $
  Revision:       $Revision: 1 $

  Description:    This file contains the interface to the Bsp smart cover Service.
                  When Smart Cover is open, an interrupt will be send to Hal Process
                  Event, the Hal starts to polling the state of Smart Cover. Meanwhile,
                  a message will be send to ColomoBLE to notify the system startup.

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
#include "bsp_cap_io.h"

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
uint8 BspCapIODelegateState  = BSP_CAP_IO_IDLE;
uint8 BspCapIODelegateTaskID = BSP_CAP_IO_NO_TASK;
uint8 BspCapIODelegateEvent  = BSP_CAP_IO_NO_EVENT;
uint16 bsp_cap_io_cnt        = 0;

/***************************************************************************************************
 *                                           LOCAL VARIABLES
 ***************************************************************************************************/

/***************************************************************************************************
 *                                            LOCAL FUNCTION
 ***************************************************************************************************/

/***************************************************************************************************
 *                                            FUNCTIONS - API
 ***************************************************************************************************/

/***************************************************************************************************
 * @fn      BspCapIOInit
 * @brief   Initialize CC2541 interrupt resource (call in board level initial session)
 * @param   None          
 *
 * @return  None
 ***************************************************************************************************/
void BspCapIOInit(void)
{
  BSP_CAP_IO_SEL &= ~BSP_CAP_IO_BIT;      /* Set pin function to GPIO */
  BSP_CAP_IO_DIR &= ~BSP_CAP_IO_BIT;      /* Set pin direction to Input */

  BSP_CAP_IO_ICTL &= ~BSP_CAP_IO_ICTLBIT; /* disable interrupt generation at P1_3 */
  BSP_CAP_IO_IEN  |= BSP_CAP_IO_IENBIT;   /* enable CPU interrupt for port 1 */
  BSP_CAP_IO_PXIFG = 0;                   /* Clear any pending interrupt */
  
  bsp_cap_io_cnt = 0;
  
  BspCapIODelegateState |= BSP_CAP_IO_INIT;  /* Bsp smart cover interrupt is initialed */
}

/***************************************************************************************************
 * @fn      BspCapIODelegate
 * @brief   Delegate task id to bsp sc service (Need to call in ColomoBLE task initial session)
 *          This will tell Hal to send message or event to ColomoBLE application level.
 * @param   task_id: delegate task id who will get the events
 *          event:  smart cover open event
 *
 * @return  None
 ***************************************************************************************************/
uint8 BspCapIODelegate(uint8 task_id, uint16 event)
{
  if ( BspCapIODelegateTaskID == BSP_CAP_IO_NO_TASK )
  {
    BspCapIODelegateTaskID = task_id;
    BspCapIODelegateEvent  = event;

    /* Bsp Smart Cover interrupt is configured */
    BspCapIODelegateState |= BSP_CAP_IO_CFG;

    return ( true );
  }
  else
    return ( false );
}
/***************************************************************************************************
***************************************************************************************************/




