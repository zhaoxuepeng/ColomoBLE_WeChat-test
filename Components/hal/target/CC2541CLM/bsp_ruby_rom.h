/**************************************************************************************************
  Filename:       bsp_ruby_rom.h
  Revised:        $Date: 2015/9/10 $
  Revision:       $Revision: 1 $

  Description:    This file contains the interface to save the data into memory


  Copyright 2015 - 2016 QijiTek. All rights reserved.
**************************************************************************************************/

#ifndef BSP_RUBY_ROM_H
#define BSP_RUBY_ROM_H

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
  
/*********************************************************************
 * FUNCTION API
 */  

/*
 * Initial the data cache
 */
extern void BspRubyRomInit( void );  
  
/*
 * Save the measure data into memory
 */
extern void BspRubySetData( uint8* pBuf );

/*
 * Get the measure data into memory
 */
extern void BspRubyGetData( uint16 package_index, uint8* pBuf );

extern void BspRubyRomWrite(void);
extern uint16 BspRubyCheckLength( void );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif
