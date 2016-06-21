/**************************************************************************************************
  Filename:       bsp_ruby_lum.c
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
#include "bsp_ruby_lum.h"
#include "hal_adc.h"
#include "bsp_ruby_rom.h"

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
static uint16 bsp_ruby_romwr_idx;  // from 0 to 4095
static uint8  bsp_ruby_cache_idx;  // from 0 to 15
static uint8  bsp_ruby_cache[128]; // divide into low section and high section

/***************************************************************************************************
 *                                            LOCAL FUNCTION
 ***************************************************************************************************/

/***************************************************************************************************
 *                                            FUNCTIONS - API
 ***************************************************************************************************/
/***************************************************************************************************
 * @fn      BspRubyRomInit
 *
 * @brief   
 *
 * @param   None
 *          
 * @return  None
 ***************************************************************************************************/
void BspRubyRomInit( void )
{
  /* clear write related index */
  bsp_ruby_cache_idx = 0;
  bsp_ruby_romwr_idx = 0;
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
  uint8 i;
  
  for(i=0; i<8; i++)
  {
    bsp_ruby_cache[(bsp_ruby_cache_idx<<3)+i] = *(pBuf+i);
  }
  
  bsp_ruby_cache_idx++;
  if( bsp_ruby_cache_idx == 16 ) bsp_ruby_cache_idx = 0;
  
  if( (bsp_ruby_cache_idx%8) == 0)
  {
  #if (defined DBG_UART) && (DBG_UART == TRUE)
    printf("[BSP ROM] 8 packages written into eeprom\r\n");
  #endif
    // schedule a rom write
    BspRubyRomWrite();
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
  return bsp_ruby_romwr_idx + (bsp_ruby_cache_idx%8);
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
void BspRubyGetData( uint16 package_index, uint8* pBuf )
{
  uint8 cache_line = 0, i;
  
  // exceed the current history data range
  if( package_index >= (bsp_ruby_romwr_idx + (bsp_ruby_cache_idx%8)) )
  {
    for( i=0; i<8; i++ )
    {
      pBuf[i] = 0;
    }
  }
  // when needed package in eeprom
  else if( package_index < bsp_ruby_romwr_idx )
  {
    HalI2CInit(0x50, i2cClock_267KHZ);

    HalI2CRead( package_index<<3, 8, pBuf );
  }
  // needed package in cache
  else
  {
    if( bsp_ruby_cache_idx >= 8)
    {
      cache_line = 8;
    }
    else
    {
      cache_line = 0;
    }
    osal_memcpy( pBuf, bsp_ruby_cache+(((package_index%8)+cache_line)<<3), 8 );
  }
}

/***************************************************************************************************
 * @fn      BspRubyRomWrite
 *
 * @brief   
 *
 * @param   None
 *          
 * @return  None
 ***************************************************************************************************/
void BspRubyRomWrite(void)
{ 
  HalI2CInit(0x50, i2cClock_267KHZ);

  HalI2CWrite( bsp_ruby_romwr_idx<<3, 64, bsp_ruby_cache+(64-(bsp_ruby_cache_idx<<3)) );
  
  bsp_ruby_romwr_idx += 8;
}

/***************************************************************************************************
***************************************************************************************************/




