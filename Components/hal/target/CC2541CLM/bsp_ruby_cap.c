/**************************************************************************************************
  Filename:       bsp_ruby_cap.c
  Revised:        $Date: 2015/9/10 $
  Revision:       $Revision: 1 $

  Description:    This file contains the interface to the bsp ruby capacity measurement.

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
#include "bsp_ruby_cap.h"

/***************************************************************************************************
 *                                             CONSTANTS
 ***************************************************************************************************/

/***************************************************************************************************
 *                                              MACROS
 ***************************************************************************************************/
#define PulseON()       BSP_CAP_CHG_SBIT = 1
#define PulseOFF()      BSP_CAP_CHG_SBIT = 0
/* TODO: need dynamic frequency adjustment */
#define TM3_start()     T3CTL |= 0xD0
//110 1 0 0 00
#define TM3_clear()     T3CTL |= 0x04
/***************************************************************************************************
 *                                              TYPEDEFS
 ***************************************************************************************************/


/***************************************************************************************************
 *                                           GLOBAL VARIABLES
 ***************************************************************************************************/
volatile uint8 cap_trigger;
/***************************************************************************************************
 *                                            LOCAL FUNCTION
 ***************************************************************************************************/

/***************************************************************************************************
 *                                            FUNCTIONS - API
 ***************************************************************************************************/
/***************************************************************************************************
 * @fn      BspCmpInit
 *
 * @brief   Initialize Comparator
 *
 * @param   None
 *          
 *          
 *                    
 *
 * @return  None
 ***************************************************************************************************/
 void BspCmpInit()
 {
	// DISABLE_ALL_INTERRUPTS();
	P0SEL |= 0x30; 	// Function select for P04 & P05
	APCFG |= 0x30;	// Analog funciont select for P04 & P05
	P0IEN |= 0x20;    // Enable Interrupt for  P0.5
	PICTL |= 0x01;    // Triggered by falling edge
	IEN1 |= 0x20;    // Enabe Interrupt for P0 
	P0IFG = 0x00;    // Clear Interrupt flags for P0 
	IRCON = 0;       // Clear Interrupt flags for  
	EA = 1;          // Enable Interrupts
	CMPCTL |= 0x02;  //enable comparator
 }
 /***************************************************************************************************
 * @fn      BspRubyCapIOInit
 *
 * @brief   Initialize Comparator
 *
 * @param   None
 *          
 *          
 *                    
 *
 * @return  None
 ***************************************************************************************************/
 void BspRubyCapIOInit()
 {
	P0DIR  |= 0x01;
	P0DIR &= ~(0x20);
	P0INP |= 0x20;
	P1DIR |= 0x03;
	P0_0 = 0;
	P1_0 = 0;
	PulseOFF();
 }
/***************************************************************************************************
 * @fn      BspRubyCapInit
 *
 * @brief   Initialize Capacity Counter
 *
 * @param   None
 *          
 *          
 *                    
 *
 * @return  None
 ***************************************************************************************************/
void BspRubyCapInit()
{
	// BspCmpInit();
	// BspRubyCapIOInit();
  
  /* Initial Comparator */
  BSP_CAP_SNS_SEL   |= BSP_CAP_SNS_POS_BV | BSP_CAP_SNS_NEG_BV; 	// Function select for P0_4 & P0_5
	BSP_CAP_SNS_APCFG |= BSP_CAP_SNS_POS_BV | BSP_CAP_SNS_NEG_BV;	  // Analog funciont select for P0_4 & P0_5

	BSP_CAP_SNS_ICTL  |= 0x01;    // Triggered by falling edge
	BSP_CAP_SNS_IEN   |= 0x20;    // Enabe Interrupt for P0
  BSP_CAP_SNS_PIEN  |= 0x20;    // Enable Interrupt for  P0.5
  
	BSP_CAP_SNS_PIFG   = 0x00;    // Clear Interrupt flags for P0 
	BSP_CAP_SNS_IRCON  = 0;       // Clear Interrupt flags
  
  /* Initial Charge IO */
	BSP_CAP_CHG_DDR   |= BSP_CAP_CHG_BV;
	PulseOFF();
}
/***************************************************************************************************
 * @fn      BspRubyCapMeas
 *
 * @brief   Initialize Capacity Counter
 *
 * @param   None
 *          
 *          
 *                    
 *
 * @return  Cap Value
 ***************************************************************************************************/
uint16 BspRubyCapMeas()
{
	uint16 meas=0;
  
  CMPCTL |= 0x02;
	TM3_clear();
	TM3_start();
  PulseON();
	while( (cap_trigger==0) && (T3CNT<255) )
  {
  }
	meas = T3CNT;
  CMPCTL &= ~0x02;
	cap_trigger = 0;
	PulseOFF();
  return meas;   	
}

/***************************************************************************************************
***************************************************************************************************/




