/**************************************************************************************************
  Filename:       ColomoBLE.h
  Revised:        $Date: 2015/6/16 $
  Revision:       $Revision: 0 $

  Description:    This file contains the ColomoBLE application
                  definitions and prototypes.

  Copyright 2015 - 2016 QijiTek. All rights reserved.
**************************************************************************************************/

#ifndef ColomoBLE_H
#define ColomoBLE_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
  
/*********************************************************************
 * CONSTANTS
 */
// Simple BLE Peripheral Task Events
#define CLM_INIT_PH1_EVT                                  0x0001
#define CLM_INIT_PH2_EVT                                  0x0002
#define CLM_INIT_PH3_EVT                                  0x0004
#define CLM_PERIODIC_EVT                                  0x0008

// Ruby BLE Task Events  
#define BSP_INIT_DONE_EVT                                 0x0010
#define BSP_SC_OPEN_EVENT                                 0x0020
#define BSP_SC_SWITCH_EVENT                               0x0200
#define BSP_SC_CLOSE_EVENT                                0x0040
#define BSP_ACC_SHAKE_EVENT                               0x0080
#define BSP_POWER_CLOSE_EVENT                             0x4000
   
#define BSP_CS_MOTOR_EVENT                                0x0400

// Ruby Data Collection Task Events
#define BSP_DATA_READ_FINISH_EVENT                        0x0100
#define TEST_EVT                                          0x0800
  
#define CLM_START_DETECT_EVT                              0x1000
#define CLM_CONFIRM_DETECT_PH1_EVT                        0x2000
  
/* Ruby advertising period is 5 seconds */  
#define BSP_ADV_PERIOD                                    5000
/* Ruby advertising active time is 0.5 seconds */
#define BSP_ADV_ACTIVE_TIME                               450
/* Ruby advertising interval = period - active_time */  
#define BSP_ADV_INTERVAL                                  4500
 
/* Ruby State */
#define CLM_STATE_POFF                                   0
#define CLM_STATE_NCNT                                   1
#define CLM_STATE_CNT                                    2
#define CLM_STATE_PAIR                                   3
#define CLM_STATE_WORK                                   4
#define CLM_STATE_OAD                                    5
#define CLM_STATE_MOTOR                                  6
#define CLM_STATE_LUM                                    7
#define CLM_STATE_FACTORY                                8
  
#define BSP_RUNTIME_PERIOD                                1000  
#define BSP_HISTORY_PERIOD                                1000
#define MAX_HISTORY_LENGTH                                1000
  
#define SNV_COLOR_ID                                      0x80
#define SNV_MOTOR_ID                                      0x81
/*********************************************************************
 * MACROS
 */
#define CLM_POFF_TOUT_REFILL                             st(ClmPoffTimeOut = 30;)

/*********************************************************************
 * FUNCTIONS
 */

/*
 * Task Initialization for the BLE Application
 */
extern void ColomoBLE_Init( uint8 task_id );

/*
 * Task Event Processor for the BLE Application
 */
extern uint16 ColomoBLE_ProcessEvent( uint8 task_id, uint16 events );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* ColomoBLE_H */
