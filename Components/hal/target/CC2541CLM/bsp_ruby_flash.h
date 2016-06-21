/**************************************************************************************************
  Filename:       bsp_ruby_flash.h
  Revised:        $Date: 2015/9/10 $
  Revision:       $Revision: 1 $

  Description:    This file contains the interface to save the data into memory


  Copyright 2015 - 2016 QijiTek. All rights reserved.
**************************************************************************************************/

#ifndef BSP_RUBY_FLASH_H
#define BSP_RUBY_FLASH_H

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
#if (defined HAL_IMAGE_A)
  #define BSP_RUBY_FLASH_PAGE_BASE   8
  #define BSP_RUBY_FLASH_PAGE_SIZE   62
#elif (defined HAL_IMAGE_B)
  #define BSP_RUBY_FLASH_PAGE_BASE   1
  #define BSP_RUBY_FLASH_PAGE_SIZE   7
  #define BSP_RUBY_FLASH_PAGE_BASE_2 70
  #define BSP_RUBY_FLASH_PAGE_SIZE_2 55
#else
  #define BSP_RUBY_FLASH_PAGE_BASE   63
  #define BSP_RUBY_FLASH_PAGE_SIZE   62  
#endif
  
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
 * Initial the ptr for flash wr and rd
 */
extern void BspRubyFlashInit( void );  
  
/*
 * Save the measure data into memory
 */
extern void BspRubySetData( uint8* pBuf );

/*
 * Get the measure data from memory
 */
extern void BspRubyGetData( uint16 point_index, uint8* pBuf );

/*
 * Get the length of measured data by Point
 */
extern uint16 BspRubyCheckLength( void );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif
