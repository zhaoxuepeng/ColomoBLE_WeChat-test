/**************************************************************************************************
  Filename:       bsp_led.c
  Revised:        $Date: 2015/6/16 $
  Revision:       $Revision: 1 $

  Description:    This file contains the interface to the Hal LED Service.


  Copyright 2015 - 2016 QijiTek. All rights reserved.
**************************************************************************************************/
#ifndef BSP_UART_H
#define BSP_UART_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "ioCC2541.h"
#include <stdio.h>
/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */
  
#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE __near_func int  putchar(int ch)
#endif /* __GNUC__ */
  
/*********************************************************************
 * GLOBAL VARIABLES
 */

/*
 * Initialize Debug Uart Service.
 */
extern void DbgUartInit( void );

/*
 * Debug Uart Send 1 Char
 */
extern void DbgUartSendChar(char Data);

extern void DbgUartPrint(char *Buf);

extern PUTCHAR_PROTOTYPE;

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif