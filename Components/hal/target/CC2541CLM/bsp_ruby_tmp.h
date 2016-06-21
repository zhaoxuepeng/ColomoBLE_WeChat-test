/**************************************************************************************************
  Filename:       bsp_ruby_tmp.h
  Revised:        $Date: 2015/9/10 $
  Revision:       $Revision: 1 $

  Description:    This file contains the interface to the capacity measurement.


  Copyright 2015 - 2016 QijiTek. All rights reserved.
**************************************************************************************************/

#ifndef BSP_RUBY_TMP_H
#define BSP_RUBY_TMP_H

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
 * Initialize Temperature
 */
extern void BspRubyTmpInit( void );
/*
 * Get temperature from offchip sensor
 */
extern uint16 BspRubyTmpMeas( void );
/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif
