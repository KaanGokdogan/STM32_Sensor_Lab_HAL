/*
 * ds1307.c
 *
 *  Created on: 7 May 2026
 *      Author: kaan_
 */

#include "ds1307.h"

#define SYSTICK_TIM_CLK 16000000UL

I2C_HandleTypeDef ds1307I2cHandle;

static void DS1307_I2C_Pin_Config(void);
static void DS1307_I2C_Config(void);
static void DS1307_Write(uint8_t Value, uint8_t RegAddr);
static uint8_t DS1307_Read(uint8_t RegAddr);


// returns 1 : CH = 1 ; init failed
// returns 0 : CH = 0 ; init success
uint8_t DS1307_Init()
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
	uint8_t seconds, hours;

	seconds = Binary_To_BCD(rtc_time->sec);
	seconds &= ~( 1 << 7);
	DS1307_Write(seconds, DS1307_ADDR_SEC);

	DS1307_Write(Binary_To_BCD(rtc_time->min), DS1307_ADDR_MIN);

	hours = Binary_To_BCD(rtc_time->hour);
	if(rtc_time->time_Format == TIME_FORMAT_24HR)
	{
		hours &= ~( 1 << 6);
	}
	else
	{
		hours |= ( 1 << 6);
		hours = (rtc_time->time_Format == TIME_FORMAT_12HRS_PM) ? hours | ( 1 << 5) : hours & ~( 1 << 5);
	}
	DS1307_Write(hours, DS1307_ADDR_HOUR);
}


void DS1307_Get_Current_Time(RTC_time_t *rtc_time)
{
	uint8_t seconds, hours;

	seconds = DS1307_Read(DS1307_ADDR_SEC);
	seconds &= ~( 1 << 7);
	rtc_time->sec = BCD_To_Binary(seconds);

	rtc_time->min = BCD_To_Binary(DS1307_Read(DS1307_ADDR_MIN));

	hours = DS1307_Read(DS1307_ADDR_HOUR);
	if( hours & ( 1 << 6))
	{
		// 12 hour format
		rtc_time->time_Format = !( ( hours & ( 1 << 5)) == 0);
		hours &= ~(0x3 << 5);	// clear 5 and 6
	}
	else
	{
		// 24 hour format
		rtc_time->time_Format = TIME_FORMAT_24HR;

	}
	rtc_time->hour = BCD_To_Binary(hours);


}


void DS1307_Set_Current_Date(RTC_date_t *rtc_date)
{
	DS1307_Write(Binary_To_BCD(rtc_date->date), DS1307_ADDR_DATE);
	DS1307_Write(Binary_To_BCD(rtc_date->day), DS1307_ADDR_DAY);
	DS1307_Write(Binary_To_BCD(rtc_date->month), DS1307_ADDR_MONTH);
	DS1307_Write(Binary_To_BCD(rtc_date->year), DS1307_ADDR_YEAR);
}


void DS1307_Get_Current_Date(RTC_date_t *rtc_date)
{
	rtc_date->date = BCD_To_Binary(DS1307_Read(DS1307_ADDR_DATE));
	rtc_date->day = BCD_To_Binary(DS1307_Read(DS1307_ADDR_DAY));
	rtc_date->month = BCD_To_Binary(DS1307_Read(DS1307_ADDR_MONTH));
	rtc_date->year = BCD_To_Binary(DS1307_Read(DS1307_ADDR_YEAR));
}


static void DS1307_I2C_Pin_Config(void)
{
	GPIO_InitTypeDef ds1307_I2c_Sda, ds1307_I2c_Scl;

	memset(&ds1307_I2c_Sda, 0, sizeof(ds1307_I2c_Sda));
	memset(&ds1307_I2c_Scl, 0, sizeof(ds1307_I2c_Scl));

	// I2C1_SDA ---> PB9
	// I2C1_SCL ---> PB6

	__HAL_RCC_GPIOB_CLK_ENABLE();
	ds1307_I2c_Sda.Pin = DS1307_I2C_SDA_PIN;
	ds1307_I2c_Sda.Mode = GPIO_MODE_AF_OD;
	ds1307_I2c_Sda.Pull = GPIO_NOPULL;
	ds1307_I2c_Sda.Speed = GPIO_SPEED_FREQ_HIGH;
	ds1307_I2c_Sda.Alternate = GPIO_AF4_I2C1;
	HAL_GPIO_Init(GPIOB, &ds1307_I2c_Sda);

	ds1307_I2c_Scl.Pin = DS1307_I2C_SCL_PIN;
	ds1307_I2c_Scl.Mode = GPIO_MODE_AF_OD;
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
	__HAL_RCC_I2C1_CLK_ENABLE();
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


uint8_t Binary_To_BCD(uint8_t Value)
{
	uint8_t first_Digit, second_Digit, bcd;

	bcd = Value;
	if(Value >= 10)
	{
		first_Digit = Value / 10;
		second_Digit = Value % 10;
		bcd = ( first_Digit << 4) | second_Digit;
	}

	return bcd;
}


uint8_t BCD_To_Binary(uint8_t Value)
{
	uint8_t first_Digit, second_Digit;

	first_Digit = ( Value >> 4) * 10;
	second_Digit = Value & (uint8_t)0x0F;

	return first_Digit + second_Digit;
}




















































