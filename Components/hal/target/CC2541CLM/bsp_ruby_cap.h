/**************************************************************************************************
  Filename:       bsp_ruby_cap.h
  Revised:        $Date: 2015/9/10 $
  Revision:       $Revision: 1 $

  Description:    This file contains the interface to the capacity measurement.


  Copyright 2015 - 2016 QijiTek. All rights reserved.
**************************************************************************************************/

#ifndef BSP_RUBY_CAP_H
#define BSP_RUBY_CAP_H

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

/*********************************************************************
 * CONSTANTS
 */



/*********************************************************************
 * TYPEDEFS
 */


/*********************************************************************
 * GLOBAL VARIABLES
 */
  
extern volatile uint8 cap_trigger;
/*
 * Initialize Charger IO.
 */
extern void BspRubyCapIOInit( void );

/*
 * Initialize Analog Comparator.
 */
extern void BspCmpInit( void );

/*
 * Initialize Capacity Counther.
 */
extern void BspRubyCapInit( void );

/*
 * Capacity Measure
 */
extern uint16 BspRubyCapMeas( void );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif
