#include "hal_mcu.h"
#include "hal_defs.h"
#include "hal_types.h"
#include "hal_drivers.h"
#include "osal.h"
#include "hal_board.h"
#include "hal_i2c.h"
#include "bsp_color_9300.h"
#include "hal_board_cfg.h"

uint16 all;
uint8  BspCsDelegateTaskID = 0xff;
uint16 BspCsDelegateEvent  = 0x0000;

void BspColorSensorDelayMS(uint8 msec)
{ 
    volatile uint16 i,j;
    
    for (i=0; i<msec; i++)
        for (j=0; j<475; j++);
}

/***************************************************************************************************
 * @fn      BspColorSensorInit
 *
 * @brief   Initialize TCS3414 (call this function at hal initial)
 *
 * @param   None        
 *
 * @return  None
 ***************************************************************************************************/
void BspColorSensorInit(void)
{
  uint8 value = 0x03;
  all=0;
  BSP_CLRLED_SBIT = 0;
  
  HalI2CInit(APDS_I2C_ADDR, i2cClock_123KHZ);
  HalI2CWrites(0x80, 1, &value); //sensor power up
}

/***************************************************************************************************
 * @fn      BspColorSensorDelegate
 * @brief   Delegate task id to bsp cs service (Need to call in ColomoBLE task initial session)
 * @param   None          
 *
 * @return  None
 ***************************************************************************************************/
uint8 BspColorSensorDelegate(uint8 task_id, uint16 event)
{
  if ( BspCsDelegateTaskID == 0xff )
  {
    BspCsDelegateTaskID = task_id;
    BspCsDelegateEvent  = event;
    
    return ( true );
  }
  else
    return ( false );
}

/***************************************************************************************************
 * @fn      BspColorSensorColorStart
 *
 * @brief   Read the color data from four channels
 *
 * @param   red, green, blue and all are the 4 channels          
 *
 * @return  None
 ***************************************************************************************************/
void BspColorSensorColorStart(void)
{
  uint8 value = 0x1B;
  HalI2CInit(APDS_I2C_ADDR, i2cClock_123KHZ);
  HalI2CWrites(0x81, 1, &value);
}

/***************************************************************************************************
 * @fn      BspColorSensorColorStop
 *
 * @brief   Read the color data from four channels
 *
 * @param   red, green, blue and all are the 4 channels          
 *
 * @return  None
 ***************************************************************************************************/
void BspColorSensorColorStop(void)
{
  uint8 value = 0x13;
  HalI2CInit(APDS_I2C_ADDR, i2cClock_123KHZ);
  HalI2CWrites(0x81, 1, &value);
}

/***************************************************************************************************
 * @fn      BspColorSensorColorRead
 *
 * @brief   Read the color data from four channels
 *
 * @param   red, green, blue and all are the 4 channels          
 *
 * @return  None
 ***************************************************************************************************/
void BspColorSensorColorRead(void)
{
  uint8 pBufL=0, pBufH=0;
  
  HalI2CInit(APDS_I2C_ADDR, i2cClock_123KHZ);
  HalI2CReads(0x8C, 1, &pBufL);
  //HalI2CInit(APDS_I2C_ADDR, i2cClock_123KHZ);
  HalI2CReads( 0x8D, 1, &pBufH);
  all = ((uint16)pBufH << 8) | pBufL;
}