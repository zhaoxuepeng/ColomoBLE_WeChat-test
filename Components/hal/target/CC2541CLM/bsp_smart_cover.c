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
#include "bsp_smart_cover.h"

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
uint8 BspScDelegateState  = BSP_SC_INT_IDLE;
uint8 BspScDelegateTaskID = BSP_SC_INT_NO_TASK;
uint8 BspScDelegateEvent1 = BSP_SC_INT_NO_EVENT;
uint16 BspScDelegateEvent2 = BSP_SC_INT_NO_EVENT;
uint8 BspScDelegateEvent3 = BSP_SC_INT_NO_EVENT;
uint8 BspScVirtualCnt     = 0;

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
 * @fn      BspSmartCoverInit
 * @brief   Initialize CC2541 interrupt resource (call in board level initial session)
 * @param   None          
 *
 * @return  None
 ***************************************************************************************************/
void BspSmartCoverInit(void)
{
  BSP_SC_INT_SEL &= ~BSP_SC_INT_BIT;      /* Set pin function to GPIO */
  BSP_SC_INT_DIR &= ~BSP_SC_INT_BIT;      /* Set pin direction to Input */
  

  BSP_SC_INT_ICTL |= BSP_SC_INT_ICTLBIT;  /* enable interrupt generation at port 1 */
  //PICTL |= 1;                             /* setting P0 interrupt triggered by falling edge */
  BSP_SC_INT_IEN  |= BSP_SC_INT_IENBIT;   /* enable CPU interrupt for port 1 */
  BSP_SC_INT_PXIFG = 0;                   /* Clear any pending interrupt */
    
  BspScDelegateState |= BSP_SC_INT_INIT;  /* Bsp smart cover interrupt is initialed */
}

/***************************************************************************************************
 * @fn      BspSmartCoverIntDelegate
 * @brief   Delegate task id to bsp sc service (Need to call in ColomoBLE task initial session)
 *          This will tell Hal to send message or event to ColomoBLE application level.
 * @param   task_id: delegate task id who will get the events
 *          event1:  smart cover open event
 *          event2:  smart cover close event
 *
 * @return  None
 ***************************************************************************************************/
uint8 BspSmartCoverIntDelegate(uint8 task_id, uint16 event1, uint16 event2, uint16 event3)
{
  if ( BspScDelegateTaskID == BSP_SC_INT_NO_TASK )
  {
    BspScDelegateTaskID = task_id;
    BspScDelegateEvent1 = event1;
    BspScDelegateEvent2 = event2;
    BspScDelegateEvent3 = event3;

    /* Bsp Smart Cover interrupt is configured */
    BspScDelegateState |= BSP_SC_INT_CFG;
    
    BspScVirtualCnt = 0;

    return ( true );
  }
  else
    return ( false );
}

/***************************************************************************************************
 * @fn      BspSmartCoverProcess
 * @brief   This function processes the smart cover state and send the change of state to application
 * 
 * @param   None          
 *
 * @return  None
 ***************************************************************************************************/
void BspSmartCoverProcess(uint8 pin_state)
{
  switch ( pin_state )
  {
  case 1:
    {
      osal_set_event ( BspScDelegateTaskID, BspScDelegateEvent1);
      break;
    }
  case 2:
    {
      osal_set_event ( BspScDelegateTaskID, BspScDelegateEvent2);
      break;
    }
  case 3:
    {
      osal_set_event ( BspScDelegateTaskID, BspScDelegateEvent3);
      break;
    }
  default:
    break;
      
  }
    
  //osal_set_event ( BspScDelegateTaskID, (pin_state==1) ? BspScDelegateEvent1 : BspScDelegateEvent2 );
}

/***************************************************************************************************
***************************************************************************************************/




