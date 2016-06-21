/**************************************************************************************************
  Filename:       bsp_beep.h
  Revised:        $Date: 2015/6/16 $
  Revision:       $Revision: 1 $

  Description:    This file contains the interface to the Bsp Beep Service.


  Copyright 2015 - 2016 QijiTek. All rights reserved.
**************************************************************************************************/

#ifndef BSP_BEEP_H
#define BSP_BEEP_H

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
#define BSP_BEEP_SINGLE    0x00
#define BSP_BEEP_MULTI     0x01

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */


/*********************************************************************
 * GLOBAL VARIABLES
 */

/*
 * Initialize Beep Service.
 */
extern void BspBeepInit( void );

/*
 * Set the Beep ON/OFF
 */ 
extern void BspBeep( void );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif
