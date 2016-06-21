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
#define BSP_CS_LED_PORT          P1
/* 1 - Bsp Color Sensor LED1 White */
#define BSP_CS_LED1_BV           BV(1)
#define BSP_CS_LED1_SBIT         P1_1
#define BSP_CS_LED1_DDR          P1DIR
#define BSP_CS_LED1_POLARITY     ACTIVE_HIGH

/* 2 - Bsp Color Sensor LED2 White */
#define BSP_CS_LED2_BV           BV(0)
#define BSP_CS_LED2_SBIT         P1_0
#define BSP_CS_LED2_DDR          P1DIR
#define BSP_CS_LED2_POLARITY     ACTIVE_HIGH

/* 3 - Bsp Color Sensor Sync IO */
#define BSP_CS_SYNC_BV           BV(2)
#define BSP_CS_SYNC_SBIT         P1_2
#define BSP_CS_SYNC_DDR          P1DIR
#define BSP_CS_SYNC_POLARITY     ACTIVE_HIGH

// System RGB Led now controlled by Bsp Coprocessor

/* ------------------------------------------------------------------------------------------------
 *                                       Sensor Configuration
 * ------------------------------------------------------------------------------------------------
 */
/* Peripheral Power control */
#define BSP_PERI_PWR_BV           BV(0)
#define BSP_PERI_PWR_SBIT         P0_0
#define BSP_PERI_PWR_DDR          P0DIR
#define BSP_PERI_PWR_SEL          P0SEL
#define BSP_PERI_PWR_POLARITY     ACTIVE_HIGH

/* ------------------------------------------------------------------------------------------------
 *                                       Beep Configuration
 * ------------------------------------------------------------------------------------------------
 */
/* Beep pin is assigned to P1_3 */
#define BSP_BEEP_BV               BV(3)
#define BSP_BEEP_SBIT             P1_3
#define BSP_BEEP_DDR              P1DIR
#define BSP_BEEP_SEL              P1SEL
#define BSP_BEEP_POLARITY         ACTIVE_HIGH

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
  P2INP = BV(5) | BV(6); /* set Port 0 and 1 to pulldown input mode */ \
  P0INP = BV(5) | BV(7); /* set CHRG_IN to tri-state input mode */     \
  /* Set output pins */                                                \
  BSP_CS_LED1_DDR |= BSP_CS_LED1_BV;                                   \
  BSP_CS_LED2_DDR |= BSP_CS_LED2_BV;                                   \
  BSP_CS_SYNC_DDR |= BSP_CS_SYNC_BV;                                   \
  BSP_PERI_PWR_DDR|= BSP_PERI_PWR_BV;                                  \
  BSP_BEEP_DDR    |= BSP_BEEP_BV;                                      \
}

/* ----------- LED's ---------- */

#define BSP_TURN_OFF_CS_LED1()       st( BSP_CS_LED1_SBIT = BSP_CS_LED1_POLARITY (0); )
#define BSP_TURN_OFF_CS_LED2()       st( BSP_CS_LED2_SBIT = BSP_CS_LED2_POLARITY (0); )
#define BSP_TURN_OFF_CS_SYNC()       st( BSP_CS_SYNC_SBIT = BSP_CS_SYNC_POLARITY (0); )

#define BSP_TURN_ON_CS_LED1()        st( BSP_CS_LED1_SBIT = BSP_CS_LED1_POLARITY (1); )
#define BSP_TURN_ON_CS_LED2()        st( BSP_CS_LED2_SBIT = BSP_CS_LED2_POLARITY (1); )
#define BSP_TURN_ON_CS_SYNC()        st( BSP_CS_SYNC_SBIT = BSP_CS_SYNC_POLARITY (1); )

#define BSP_TOGGLE_CS_LED1()         st( if (BSP_CS_LED1_SBIT) {BSP_CS_LED1_SBIT = 0; } else { BSP_CS_LED1_SBIT = 1;} )
#define BSP_TOGGLE_CS_LED2()         st( if (BSP_CS_LED2_SBIT) {BSP_CS_LED2_SBIT = 0; } else { BSP_CS_LED2_SBIT = 1;} )
#define BSP_TOGGLE_CS_SYNC()         st( if (BSP_CS_SYNC_SBIT) {BSP_CS_SYNC_SBIT = 0; } else { BSP_CS_SYNC_SBIT = 1;} )

#define BSP_STATE_CS_LED1()          (BSP_CS_LED1_POLARITY (BSP_CS_LED1_SBIT))
#define BSP_STATE_CS_LED2()          (BSP_CS_LED2_POLARITY (BSP_CS_LED2_SBIT))
#define BSP_STATE_CS_SYNC()          (BSP_CS_SYNC_POLARITY (BSP_CS_SYNC_SBIT))

/* ----------- Power Control Pin ---------- */
#define BSP_TURN_OFF_PERI_PWR()       st( BSP_PERI_PWR_SBIT = BSP_PERI_PWR_POLARITY (0); )
#define BSP_TURN_ON_PERI_PWR()        st( BSP_PERI_PWR_SBIT = BSP_PERI_PWR_POLARITY (1); )
#define BSP_TOGGLE_PERI_PWR()         st( if (BSP_PERI_PWR_SBIT) {BSP_PERI_PWR_SBIT = 0; } else { BSP_PERI_PWR_SBIT = 1;} )
#define BSP_STATE_PERI_PWR()          (BSP_PERI_PWR_POLARITY (BSP_PERI_PWR_SBIT))

/* ----------- Beep Pin ---------- */
// Control the Beep Pin by bsp system, no need abs code for user developement

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
