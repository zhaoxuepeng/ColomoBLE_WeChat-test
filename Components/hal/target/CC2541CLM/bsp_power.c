/**************************************************************************************************
  Filename:       bsp_power.c
  Revised:        $Date: 2015/6/18 $
  Revision:       $Revision: 1 $

  Description:    This file contains the interface to the Colomo Power Control Service.


  Copyright 2015 - 2016 QijiTek. All rights reserved.
**************************************************************************************************/

/***************************************************************************************************
 *                                             INCLUDES
 ***************************************************************************************************/
#include "hal_mcu.h"
#include "hal_defs.h"
#include "hal_types.h"
#include "hal_drivers.h"
#include "bsp_power.h"
#include "bsp_color_sensor.h"
#include "osal.h"
#include "hal_board.h"
#include "bsp_cp.h"

#if defined DBG_UART
#include "bsp_uart.h"
#endif


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
static uint8 BspUsbCurrentState; 
static uint8 BspChrgFinishState;

/***************************************************************************************************
 *                                            LOCAL FUNCTION
 ***************************************************************************************************/

/***************************************************************************************************
 *                                            FUNCTIONS - API
 ***************************************************************************************************/

/***************************************************************************************************
 * @fn      BspPwrCtrlInit
 *
 * @brief   Initialize Colomo Power Control Service (call at hal initial)
 *
 * @param   None
 *
 * @return  None
 ***************************************************************************************************/

void BspPwrCtrlInit() 
{
  BSP_PERI_PWR_OFF();
}

/***************************************************************************************************
 * @fn      BspPwrCtrl
 *
 * @brief   Control the Colomo Peripheral Power (call this function by application
 *          instead of hal)
 *
 * @param   None
 *
 * @return  None
 ***************************************************************************************************/
// use marco instead of function

/***************************************************************************************************
 * @fn      BspUsbChargerInit
 *
 * @brief   Control the Colomo Peripheral Power (call this function by application
 *          instead of hal)
 *
 * @param   None
 *
 * @return  None
 ***************************************************************************************************/
void BspUsbChargerInit(void)
{
  BSP_USB_IN_SEL &= ~BSP_USB_IN_BIT;      /* Set pin function to GPIO */
  BSP_USB_IN_DIR &= ~BSP_USB_IN_BIT;      /* Set pin direction to Input */

  BSP_USB_IN_ICTL |= BSP_USB_IN_ICTLBIT;  /* enable interrupt generation at port 0 */
  BSP_USB_IN_IEN  |= BSP_USB_IN_IENBIT;   /* enable CPU interrupt for port 0 */
  BSP_USB_IN_PXIFG = 0;                   /* Clear any pending interrupt */
  
  BspUsbCurrentState = BSP_USB_OUT;
  BspChrgFinishState = BSP_CHRG_NOT_FINISH;
  
  /* If usb in when initial, start polling */
  if( BSP_USB_PIN_STATE == BSP_USB_IN )   
  {
    osal_set_event (Hal_TaskID, BSP_USB_IN_EVENT);
  }

}

/***************************************************************************************************
 * @fn      BspUsbChargerProcess
 * @brief   This function processes the usb charger state
 * 
 * @param   None          
 *
 * @return  None
 ***************************************************************************************************/
void BspUsbChargerProcess(uint8 pin_state, uint8 chrg_state)
{
  if (BspUsbCurrentState != pin_state) // smart cover state changed
  {
    BspUsbCurrentState = pin_state;

    if(BspUsbCurrentState == BSP_USB_IN)
    {
#if (defined DBG_UART)
      printf("[BSP USB] Plug in\r\n");
#endif
      /* power up the peripheral */                                
      BSP_PERI_PWR_ON();                                           
      /* reset coprocessor */                                      
      BspCpCommand( BSP_CP_RESET );                                
      /* delay 100ms for STM32 resetting */                        
      BspColorSensorDelayMS(100);                                  
      /* Set System Led Command */                                 
      BspCpSetLed(CP_SYS_LED_ON, CP_SYS_LED_BL_15, CP_SYS_LED_BL_NFD, CP_SYS_LEDG);
    }
    else
    {
#if (defined DBG_UART)
      printf("[BSP USB] Plug out\r\n");
#endif
      BspCpSetLed(CP_SYS_LED_OFF, CP_SYS_LED_NO_PARA, CP_SYS_LED_NO_PARA, CP_SYS_LED_NO_PARA);
      BSP_PERI_PWR_OFF();
    }
  }
  
  if (BspChrgFinishState != chrg_state )
  {
    BspChrgFinishState = chrg_state;
    
    if(BspChrgFinishState == BSP_CHRG_FINISH)
    {
#if (defined DBG_UART)
      printf("[BSP USB] Charge Finish\r\n");
#endif
    }
  }
}

/***************************************************************************************************
***************************************************************************************************/




