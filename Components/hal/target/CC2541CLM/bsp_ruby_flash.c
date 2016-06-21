/**************************************************************************************************
  Filename:       bsp_ruby_flash.c
  Revised:        $Date: 2015/9/10 $
  Revision:       $Revision: 1 $

  Description:    This file contains the interface to set and get history data.

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
#include "bsp_ruby_lum.h"
#include "hal_adc.h"
#include "hal_flash.h"
#include "bsp_ruby_flash.h"

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

/***************************************************************************************************
 *                                            LOCAL FUNCTION
 ***************************************************************************************************/
static uint8  bsp_ruby_flash_pgs = 0;
static uint16 bsp_ruby_flash_ofs = 0;

/***************************************************************************************************
 *                                            FUNCTIONS - API
 ***************************************************************************************************/
/***************************************************************************************************
 * @fn      BspRubyFlashInit
 *
 * @brief   
 *
 * @param   None
 *          
 * @return  None
 ***************************************************************************************************/
void BspRubyFlashInit( void )
{
  bsp_ruby_flash_pgs = BSP_RUBY_FLASH_PAGE_BASE;
  bsp_ruby_flash_ofs = 0;
  
  HalFlashErase( bsp_ruby_flash_pgs );
}

/***************************************************************************************************
 * @fn      BspRubySetData
 *
 * @brief   
 *
 * @param   None
 *          
 * @return  None
 ***************************************************************************************************/
void BspRubySetData( uint8* pBuf )
{
  uint16 addr = 0;
  
  addr = (((uint16)bsp_ruby_flash_pgs)<<9) + bsp_ruby_flash_ofs; // get the word address
  
  HalFlashWrite(addr, pBuf, 2); // write into flash
  
  bsp_ruby_flash_ofs += 2; // offset increased by 2
  
  if( bsp_ruby_flash_ofs == 512 ) // cross page
  {
    bsp_ruby_flash_ofs = 0;
    
#if (defined HAL_IMAGE_B)
    bsp_ruby_flash_pgs += 1;
    
    if( bsp_ruby_flash_pgs == (BSP_RUBY_FLASH_PAGE_BASE + BSP_RUBY_FLASH_PAGE_SIZE) )
    {
      bsp_ruby_flash_pgs = BSP_RUBY_FLASH_PAGE_BASE_2;
    }
    else if( bsp_ruby_flash_pgs == BSP_RUBY_FLASH_PAGE_BASE_2 + BSP_RUBY_FLASH_PAGE_SIZE_2 )
    {
      bsp_ruby_flash_pgs = BSP_RUBY_FLASH_PAGE_BASE;
    }
#else
    bsp_ruby_flash_pgs += 1;
    
    if( bsp_ruby_flash_pgs == (BSP_RUBY_FLASH_PAGE_BASE + BSP_RUBY_FLASH_PAGE_SIZE) )
    {
      bsp_ruby_flash_pgs = BSP_RUBY_FLASH_PAGE_BASE;
    }
#endif
    
    HalFlashErase( bsp_ruby_flash_pgs ); // erase next page
  }
}

/***************************************************************************************************
 * @fn      BspRubyCheckLength
 *
 * @brief   
 *
 * @param   None
 *          
 * @return  None
 ***************************************************************************************************/
uint16 BspRubyCheckLength( void )
{
#if (defined HAL_IMAGE_B)
  uint16 length=0;
  
  if( bsp_ruby_flash_pgs >= BSP_RUBY_FLASH_PAGE_BASE_2 ) // in the part 2
  {
    length = ((((uint16)bsp_ruby_flash_pgs-BSP_RUBY_FLASH_PAGE_BASE_2+BSP_RUBY_FLASH_PAGE_SIZE)<<9) + bsp_ruby_flash_ofs)/2;
  }
  else // still in the part 1
  {
    length = ((((uint16)bsp_ruby_flash_pgs-BSP_RUBY_FLASH_PAGE_BASE)<<9) + bsp_ruby_flash_ofs)/2;
  }
  return length;
#else
  return ((((uint16)bsp_ruby_flash_pgs-BSP_RUBY_FLASH_PAGE_BASE)<<9) + bsp_ruby_flash_ofs)/2;
#endif
}

/***************************************************************************************************
 * @fn      BspRubyGetData
 *
 * @brief   
 *
 * @param   None
 *          
 * @return  None
 ***************************************************************************************************/
void BspRubyGetData( uint16 point_index, uint8* pBuf )
{
  uint8  pgs;
  uint16 ofs;
  
  if( point_index < BspRubyCheckLength() )
  {
    pgs = (point_index >> 8);
#if (defined HAL_IMAGE_B)
    if( pgs >= (BSP_RUBY_FLASH_PAGE_BASE+BSP_RUBY_FLASH_PAGE_SIZE) )
    {
      pgs = pgs-(BSP_RUBY_FLASH_PAGE_BASE+BSP_RUBY_FLASH_PAGE_SIZE)+BSP_RUBY_FLASH_PAGE_BASE_2;
    }
    else
    {
      pgs += BSP_RUBY_FLASH_PAGE_BASE;
    }
#else
    pgs += BSP_RUBY_FLASH_PAGE_BASE;
#endif    
    ofs =  (point_index % 256) << 3;
  
    HalFlashRead( pgs, ofs, pBuf, 8);
  }
  else
  {
    for(pgs=0; pgs<8; pgs++)
    {
      *(pBuf+pgs) = 0;
    }
  }
}

/***************************************************************************************************
***************************************************************************************************/




