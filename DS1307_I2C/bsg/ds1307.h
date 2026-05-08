/*
 * ds1307.h
 *
 *  Created on: 7 May 2026
 *      Author: kaan_
 */

#ifndef DS1307_H_
#define DS1307_H_

#include "stm32f4xx_hal.h"
#include "stm32f407xx.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>



/*****		Device Address		*****/
#define DS1307_I2C_ADDRESS      	0x68


/*****		Register Addresses	*****/
#define DS1307_ADDR_SEC				0X00
#define DS1307_ADDR_MIN				0X01
#define DS1307_ADDR_HOUR			0X02
#define DS1307_ADDR_DAY				0X03
#define DS1307_ADDR_DATE			0X04
#define DS1307_ADDR_MONTH			0X05
#define DS1307_ADDR_YEAR			0X06
#define DS1307_ADDR_CR				0X07

#define TIME_FORMAT_12HRS_AM		0
#define TIME_FORMAT_12HRS_PM		1
#define TIME_FORMAT_24HR			2

#define SUNDAY 						1
#define MONDAY 						2
#define TUESDAY						3
#define WEDNESDAY					4
#define THURSDAY					5
#define FRIDAY 						6
#define SATURDAY 					7

/*****		Application Configurable Items	*****/
#define DS1307_I2C					I2C1
#define DS1307_I2C_GPIO_PORT		GPIOB
#define DS1307_I2C_SDA_PIN			GPIO_PIN_NO_7
#define DS1307_I2C_SCL_PIN			GPIO_PIN_NO_6
#define DS1307_I2C_SPEED			I2C_SCL_SPEED_SM
#define DS1307_I2C_PUPD				GPIO_NO_PU			// No internal pullups


typedef struct
{
	uint8_t date;
	uint8_t day;
	uint8_t month;
	uint8_t year;
}RTC_date_t;


typedef struct
{
	uint8_t sec;
	uint8_t min;
	uint8_t hour;
	uint8_t time_Format;
}RTC_time_t;


/*****		Functions	*****/
uint8_t ds1307_Init(void);
void DS1307_Set_Current_Time(RTC_time_t *rtc_time);
void DS1307_Get_Current_Time(RTC_time_t *rtc_time);
void DS1307_Set_Current_Date(RTC_time_t *rtc_date);
void DS1307_Get_Current_Date(RTC_time_t *rtc_date);


#endif /* DS1307_H_ */
