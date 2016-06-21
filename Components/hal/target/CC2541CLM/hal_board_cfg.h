/**************************************************************************************************
  Filename:       hal_board_cfg.h
  Revised:        $Date: 2015/6/17 $
  Revision:       $Revision: 0 $

  Description:    Board configuration for CC2541 ColomoBLE


  Copyright 2015 -2016 QijiTek. All rights reserved.
**************************************************************************************************/

#ifndef HAL_BOARD_CFG_H
#define HAL_BOARD_CFG_H

/*
 *     =============================================================
 *     |               QijiTek CC2541 ColomoBLE Board              |
 *     | --------------------------------------------------------- |
 *     |  mcu   : 8051 core                                        |
 *     |  clock : 32MHz                                            |
 *     =============================================================
 */

#define ACTIVE_LOW        !
#define ACTIVE_HIGH       !!    /* double negation forces result to be '1' */

/* ------------------------------------------------------------------------------------------------
 *                                           Includes
 * ------------------------------------------------------------------------------------------------
 */

#include "hal_mcu.h"
#include "hal_defs.h"
#include "hal_types.h"

/* ------------------------------------------------------------------------------------------------
 *                                       Board Indentifier
 *
 *      Define the Board Identifier to CC2541_SB
 * ------------------------------------------------------------------------------------------------
 */

#define CC2541_SB

/* ------------------------------------------------------------------------------------------------
 *                                          Clock Speed
 * ------------------------------------------------------------------------------------------------
 */
#define HAL_CPU_CLOCK_MHZ     32
#define EXTERNAL_CRYSTAL_OSC  0x00
#define INTERNAL_RC_OSC       0x80

/*
 * If not using power management, assume that the 32kHz crystal is not
 * installed. Even if a 32kHz crystal is present on the board, it will
 * never get used since device does not ever go to sleep. By forcing
 * OSC32K_CRYSTAL_INSTALLED to FALSE, we avoid mismatches between
 * libraries built with power management off, and applications in which
 * power management is not used.
 */
#if ( !defined ( POWER_SAVING ) ) && ( !defined ( OSC32K_CRYSTAL_INSTALLED ) )
  #define OSC32K_CRYSTAL_INSTALLED FALSE
#endif

/* 32 kHz clock source select in CLKCONCMD */
#if !defined (OSC32K_CRYSTAL_INSTALLED) || (defined (OSC32K_CRYSTAL_INSTALLED) && (OSC32K_CRYSTAL_INSTALLED == TRUE))
  #define OSC_32KHZ  EXTERNAL_CRYSTAL_OSC /* external 32 KHz xosc */
#else
  #define OSC_32KHZ  INTERNAL_RC_OSC      /* internal 32 KHz rcosc */
#endif

// Minimum Time for Stable External 32kHz Clock (in ms)
#define MIN_TIME_TO_STABLE_32KHZ_XOSC 400

/* ------------------------------------------------------------------------------------------------
 *                                       LED Configuration
 * ------------------------------------------------------------------------------------------------
 */
#define BSP_SYSLED_PORT          P1
#define BSP_SYSLED_BV            BV(0)
#define BSP_SYSLED_SBIT          P1_0
#define BSP_SYSLED_DDR           P1DIR
#define BSP_SYSLED_INP           P1INP
#define BSP_SYSLED_POLARITY      ACTIVE_HIGH

#define BSP_CLRLED_PORT          P1
#define BSP_CLRLED_BV            BV(1)
#define BSP_CLRLED_SBIT          P1_1
#define BSP_CLRLED_DDR           P1DIR
#define BSP_CLRLED_INP           P1INP
#define BSP_LUMEN_APCFG       APCFG
#define BSP_CLRLED_POLARITY      ACTIVE_HIGH


/* ------------------------------------------------------------------------------------------------
 *                                       Peripheral Power Configuration
 * ------------------------------------------------------------------------------------------------
 */
#define BSP_PERI_PWR_BV           BV(2)
#define BSP_PERI_PWR_SBIT         P1_2
#define BSP_PERI_PWR_DDR          P1DIR
#define BSP_PERI_PWR_SEL          P1SEL
#define BSP_PERI_PWR_POLARITY     ACTIVE_HIGH

/* ------------------------------------------------------------------------------------------------
 *                         OSAL NV implemented by internal flash pages.
 * ------------------------------------------------------------------------------------------------
 */

// Flash is partitioned into 8 banks of 32 KB or 16 pages.
#define HAL_FLASH_PAGE_PER_BANK    16
// Flash is constructed of 128 pages of 2 KB.
#define HAL_FLASH_PAGE_SIZE        2048
#define HAL_FLASH_WORD_SIZE        4

// CODE banks get mapped into the XDATA range 8000-FFFF.
#define HAL_FLASH_PAGE_MAP         0x8000

// The last 16 bytes of the last available page are reserved for flash lock bits.
// NV page definitions must coincide with segment declaration in project *.xcl file.
#if defined NON_BANKED
#define HAL_FLASH_LOCK_BITS        16
#define HAL_NV_PAGE_END            30
#else
#define HAL_FLASH_LOCK_BITS        16
#define HAL_NV_PAGE_END            126
#endif

// Re-defining Z_EXTADDR_LEN here so as not to include a Z-Stack .h file.
#define HAL_FLASH_IEEE_SIZE        8
#define HAL_FLASH_IEEE_PAGE       (HAL_NV_PAGE_END+1)
#define HAL_FLASH_IEEE_OSET       (HAL_FLASH_PAGE_SIZE - HAL_FLASH_LOCK_BITS - HAL_FLASH_IEEE_SIZE)
#define HAL_INFOP_IEEE_OSET        0xC

#define HAL_NV_PAGE_CNT            2
#define HAL_NV_PAGE_BEG           (HAL_NV_PAGE_END-HAL_NV_PAGE_CNT+1)

// Used by DMA macros to shift 1 to create a mask for DMA registers.
#define HAL_NV_DMA_CH              0
#define HAL_DMA_CH_RX              3
#define HAL_DMA_CH_TX              4

#define HAL_NV_DMA_GET_DESC()      HAL_DMA_GET_DESC0()
#define HAL_NV_DMA_SET_ADDR(a)     HAL_DMA_SET_ADDR_DESC0((a))


/* ------------------------------------------------------------------------------------------------
 *                                            Macros
 * ------------------------------------------------------------------------------------------------
 */

/* ----------- Cache Prefetch control ---------- */
#define PREFETCH_ENABLE()     st( FCTL = 0x08; )
#define PREFETCH_DISABLE()    st( FCTL = 0x04; )

/* Setting Clocks */

// switch to the 16MHz HSOSC and wait until it is stable
#define SET_OSC_TO_HSOSC()                                                     \
{                                                                              \
  CLKCONCMD = (CLKCONCMD & 0x80) | CLKCONCMD_16MHZ;                            \
  while ( (CLKCONSTA & ~0x80) != CLKCONCMD_16MHZ );                            \
}

// switch to the 32MHz XOSC and wait until it is stable
#define SET_OSC_TO_XOSC()                                                      \
{                                                                              \
  CLKCONCMD = (CLKCONCMD & 0x80) | CLKCONCMD_32MHZ;                            \
  while ( (CLKCONSTA & ~0x80) != CLKCONCMD_32MHZ );                            \
}

// set 32kHz OSC and wait until it is stable
#define SET_32KHZ_OSC()                                                        \
{                                                                              \
  CLKCONCMD = (CLKCONCMD & ~0x80) | OSC_32KHZ;                                 \
  while ( (CLKCONSTA & 0x80) != OSC_32KHZ );                                   \
}

#define START_HSOSC_XOSC()                                                     \
{                                                                              \
  SLEEPCMD &= ~OSC_PD;            /* start 16MHz RCOSC & 32MHz XOSC */         \
  while (!(SLEEPSTA & XOSC_STB)); /* wait for stable 32MHz XOSC */             \
}

#define STOP_HSOSC()                                                           \
{                                                                              \
  SLEEPCMD |= OSC_PD;             /* stop 16MHz RCOSC */                       \
}
/* ----------- Board Initialization ---------- */

#define HAL_BOARD_INIT()                                         \
{                                                                \
   /* Set to 16Mhz to set 32kHz OSC, then back to 32MHz */       \
  START_HSOSC_XOSC();                                            \
  SET_OSC_TO_HSOSC();                                            \
  SET_32KHZ_OSC();                                               \
  SET_OSC_TO_XOSC();                                             \
  STOP_HSOSC();                                                  \
                                                                 \
  /* Turn on cache prefetch mode */                              \
  PREFETCH_ENABLE();                                             \
                                                                 \
  /* Configure for all IOs */                                    \
  P2INP = BV(5) | BV(6);                                         \
  /* P1INP = BV(5); */                                           \
  /* Set output pins */                                          \
  BSP_SYSLED_DDR |= BSP_SYSLED_BV;                               \
  BSP_CLRLED_DDR |= BSP_CLRLED_BV;                               \
  BSP_PERI_PWR_DDR|= BSP_PERI_PWR_BV;                            \
  BSP_SYSLED_SBIT = 0;                                           \
  BSP_CLRLED_SBIT = 0;                                           \
  BSP_PERI_PWR_SBIT = 0;                                         \
}

/* ------------------------------------------------------------------------------------------------
 *                                     Driver Configuration
 * ------------------------------------------------------------------------------------------------
 */

/* Set to TRUE enable H/W TIMER usage, FALSE disable it */
#ifndef HAL_TIMER
#define HAL_TIMER FALSE
#endif

/* Set to TRUE enable ADC usage, FALSE disable it */
#ifndef HAL_ADC
#define HAL_ADC FALSE
#endif

/* Set to TRUE enable DMA usage, FALSE disable it */
#ifndef HAL_DMA
#define HAL_DMA TRUE
#endif

/* Set to TRUE enable Flash access, FALSE disable it */
#ifndef HAL_FLASH
#define HAL_FLASH TRUE
#endif

/* Set to TRUE enable AES usage, FALSE disable it */
#ifndef HAL_AES
#define HAL_AES FALSE
#endif

#ifndef HAL_AES_DMA
#define HAL_AES_DMA FALSE
#endif

/* Set to TRUE enable LCD usage, FALSE disable it */
#ifndef HAL_LCD
#define HAL_LCD FALSE
#endif

/* Set to TRUE enable LED usage, FALSE disable it */
#ifndef HAL_LED
#define HAL_LED TRUE
#endif
#if (!defined BLINK_LEDS) && (HAL_LED == TRUE)
#define BLINK_LEDS
#endif

/* Set to TRUE enable KEY usage, FALSE disable it */
#ifndef HAL_KEY
#define HAL_KEY TRUE
#endif

/*******************************************************************************************************
*/
#endif
