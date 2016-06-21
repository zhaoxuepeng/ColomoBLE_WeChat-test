/**************************************************************************************************
  Filename:       bsp_ruby_lum.h
  Revised:        $Date: 2015/9/10 $
  Revision:       $Revision: 1 $

  Description:    This file contains the interface to the capacity measurement.


  Copyright 2015 - 2016 QijiTek. All rights reserved.
**************************************************************************************************/

#ifndef BSP_RUBY_RES_H
#define BSP_RUBY_RES_H

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
/*
 * Initialize Charger IO.
 */
extern void BspRubyResInit( void );

/*
 * Initialize Analog Comparator.
 */
extern uint16 BspRubyResMeas( void );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif
