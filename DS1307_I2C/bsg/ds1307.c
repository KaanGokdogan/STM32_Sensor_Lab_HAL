/*
 * ds1307.c
 *
 *  Created on: 7 May 2026
 *      Author: kaan_
 */

#include "ds1307.h"

I2C_HandleTypeDef ds1307I2cHandle;

static void DS1307_I2C_Pin_Config(void);
static void DS1307_I2C_Config(void);
static void DS1307_Write(uint8_t Value, uint8_t RegAddr);
static uint8_t DS1307_Read(uint8_t RegAddr);


// returns 1 : CH = 1 ; init failed
// returns 0 : CH = 0 ; init success
uint8_t ds1307_Init()
{
	// 1. Init the I2C pins
	DS1307_I2C_Pin_Config();

	// 2. Initialize the I2C peripheral
	DS1307_I2C_Config();

	// 3. Make clock halt = 0
	DS1307_Write(0x00, DS1307_ADDR_SEC);

	// 4. Read back clock halt bit
	uint8_t clock_State = DS1307_Read(DS1307_ADDR_SEC);

	return (clock_State >> 7) & 0x1;
}


void DS1307_Set_Current_Time(RTC_time_t *rtc_time)
{

}


void DS1307_Get_Current_Time(RTC_time_t *rtc_time)
{

}


void DS1307_Set_Current_Date(RTC_time_t *rtc_date)
{

}


void DS1307_Get_Current_Date(RTC_time_t *rtc_date)
{

}


static void DS1307_I2C_Pin_Config(void)
{
	GPIO_InitTypeDef ds1307_I2c_Sda, ds1307_I2c_Scl;

	memset(&ds1307_I2c_Sda, 0, sizeof(ds1307_I2c_Sda));
	memset(&ds1307_I2c_Scl, 0, sizeof(ds1307_I2c_Scl));

	// I2C1_SDA ---> PB7
	// I2C1_SCL ---> PB8

	__HAL_RCC_GPIOB_CLK_ENABLE();
	ds1307_I2c_Sda.Pin = GPIO_PIN_7;
	ds1307_I2c_Sda.Mode = GPIO_MODE_AF_PP;
	ds1307_I2c_Sda.Pull = GPIO_NOPULL;
	ds1307_I2c_Sda.Speed = GPIO_SPEED_FAST;
	ds1307_I2c_Sda.Alternate = GPIO_AF4_I2C1;
	HAL_GPIO_Init(GPIOB, &ds1307_I2c_Sda);

	ds1307_I2c_Scl.Pin = GPIO_PIN_8;
	ds1307_I2c_Scl.Mode = GPIO_MODE_AF_PP;
	ds1307_I2c_Scl.Pull = GPIO_NOPULL;
	ds1307_I2c_Scl.Speed = GPIO_SPEED_FAST;
	ds1307_I2c_Scl.Alternate = GPIO_AF4_I2C1;
	HAL_GPIO_Init(GPIOB, &ds1307_I2c_Scl);
}


static void ErrorHandler(void)
{
	while(1);
}


static void DS1307_I2C_Config(void)
{
	ds1307I2cHandle.Instance = I2C1;
	ds1307I2cHandle.Init.ClockSpeed = 100000;
	ds1307I2cHandle.Init.DutyCycle = I2C_DUTYCYCLE_2;
	ds1307I2cHandle.Init.OwnAddress1 = 0;
	ds1307I2cHandle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	ds1307I2cHandle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	ds1307I2cHandle.Init.OwnAddress2 = 0;
	ds1307I2cHandle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	ds1307I2cHandle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

	if( HAL_I2C_Init(&ds1307I2cHandle) != HAL_OK)
	{
		ErrorHandler();
	}



}


static void DS1307_Write(uint8_t Value, uint8_t RegAddr)
{
	uint8_t tx_Data[2];
	tx_Data[0] = RegAddr;
	tx_Data[1] = Value;

	HAL_I2C_Master_Transmit(&ds1307I2cHandle, DS1307_I2C_ADDRESS, tx_Data, 2, HAL_MAX_DELAY);
}


static uint8_t DS1307_Read(uint8_t RegAddr)
{
	uint8_t receive_Data;

	HAL_I2C_Master_Transmit(&ds1307I2cHandle, DS1307_I2C_ADDRESS, &RegAddr, 1, HAL_MAX_DELAY);
	HAL_I2C_Master_Receive(&ds1307I2cHandle, DS1307_I2C_ADDRESS, &receive_Data, 1, HAL_MAX_DELAY);

	return receive_Data;
}




















































