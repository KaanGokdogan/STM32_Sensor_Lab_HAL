/*
 * main.c
 *
 *  Created on: 11 May 2026
 *      Author: kaan_
 */


#include "main.h"


TIM_HandleTypeDef htimer6;
volatile uint8_t rtc_Timer_Flag = 0;


int main(void)
{
	RTC_time_t current_Time;
	RTC_date_t current_Date;

	HAL_Init();
	SystemClock_Config();
	Timer6_Init();

	// Start the timer
	HAL_TIM_Base_Start_IT(&htimer6);

	if(DS1307_Init())
	{
		printf("RTC init has failed");
		while(1);
	}

	current_Date.day = TUESDAY;
	current_Date.date = 5;
	current_Date.month = 5;
	current_Date.year = 26;

	current_Time.hour = 11;
	current_Time.min = 59;
	current_Time.sec = 55;
	current_Time.time_Format = TIME_FORMAT_12HRS_PM;

	DS1307_Set_Current_Date(&current_Date);
	DS1307_Set_Current_Time(&current_Time);

	while (1)
	{
		if(rtc_Timer_Flag)
		{
			rtc_Timer_Flag = 0;

			DS1307_Get_Current_Time(&current_Time);

			char *am_Pm;
			if(current_Time.time_Format != TIME_FORMAT_24HR)
			{
				am_Pm = (current_Time.time_Format) ? "PM" : "AM";
				printf("Current time = %s %s\n",Time_To_String(&current_Time),am_Pm); // 04:44:44 PM
			}
			else
			{
				printf("Current time = %s\n",Time_To_String(&current_Time)); // 04:44:44
			}

			DS1307_Get_Current_Date(&current_Date);
			// 04/02/26 <Tuesday>
			printf("Current date = %s <%s>\n",Date_To_String(&current_Date),Get_Day_Of_Week(current_Date.day));
		}
	}
	return 0;
}

/************************************************************************
 * @fn				- SystemClock_Config
 *
 * @brief			- Configures the system clock settings of the microcontroller.
 *
 * @param[in]		- none
 *
 * @return			- none
 *
 * @Note			- Currently empty as HSI (16 MHz) is used by default.
 ************************************************************************/
void SystemClock_Config(void)
{

}


/************************************************************************
 * @fn				- Timer6_Init
 *
 * @brief			- Initializes the Timer 6 peripheral to generate a
 * 					  1-second interrupt.
 *
 * @param[in]		- none
 *
 * @return			- none
 *
 * @Note			- Prescaler and Period are calculated based on the
 * 					  HSI (16MHz) clock source.
 ************************************************************************/
void Timer6_Init(void)
{
	htimer6.Instance = TIM6;
	htimer6.Init.Prescaler = 16000 - 1;
	htimer6.Init.Period = 1000 - 1;

	if(HAL_TIM_Base_Init(&htimer6) != HAL_OK)
	{
		printf("Timer init failed");
	}
}


/************************************************************************
 * @fn				- HAL_TIM_PeriodElapsedCallback
 *
 * @brief			- Hardware interrupt callback function that is
 * 					  automatically called when the timer period elapses.
 *
 * @param[in]		- htim: Pointer to a TIM_HandleTypeDef structure.
 *
 * @return			- none
 *
 * @Note			- Sets a flag here instead of processing I2C
 * 					  operations directly to prevent bus locking.
 ************************************************************************/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	rtc_Timer_Flag = 1;
}


/************************************************************************
 * @fn				- Get_Day_Of_Week
 *
 * @brief			- Converts the numerical day information from 1 to 7
 * into a string format.
 *
 * @param[in]		- i: Day index from 1 to 7 (1=Sunday ... 7=Saturday)
 *
 * @return			- char*: The starting address of the string containing
 * the day name.
 *
 * @Note			- none
 ************************************************************************/
char* Get_Day_Of_Week(uint8_t i)
{
	char* days[] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", };

	return days[i - 1];
}


/************************************************************************
 * @fn				- Number_To_String
 *
 * @brief			- Converts integers between 0 and 99 into a two-digit
 * 					  ASCII (String) format.
 *
 * @param[in]		- Number: The 8-bit number to be converted.
 * @param[in]		- buf: The target memory address where the converted
 * 					  characters will be written.
 *
 * @return			- none
 *
 * @Note			- Automatically adds a leading '0' to single-digit
 * 					  numbers (e.g., 9 -> "09").
 ************************************************************************/
void Number_To_String(uint8_t Number, char* buf)
{
	if(Number < 10)
	{
		buf[0] = '0';
		buf[1] = Number + 48;
	}
	else if( Number >= 10 && Number < 100)
	{
		buf[0] = (Number / 10) + 48;
		buf[1] = (Number % 10) + 48;
	}
}


/************************************************************************
 * @fn				- Time_To_String
 *
 * @brief			- Converts the time data read from the RTC into
 * 					  "hh:mm:ss" format.
 *
 * @param[in]		- rtc_time: Pointer to the time structure obtained
 * 					  from the RTC.
 *
 * @return			- char*: The address of the string containing the
 * 					  formatted time data.
 *
 * @Note			- Safe to return as the memory is allocated as "static".
 ************************************************************************/
char* Time_To_String(RTC_time_t *rtc_time)
{
	static char buf[9];

	buf[2] = ':';
	buf[5] = ':';

	Number_To_String(rtc_time->hour,buf);
	Number_To_String(rtc_time->min,&buf[3]);
	Number_To_String(rtc_time->sec,&buf[6]);

	buf[8] = '\0';

	return buf;
}


/************************************************************************
 * @fn				- Date_To_String
 *
 * @brief			- Converts the date data read from the RTC into
 * 					  "dd/mm/yy" format.
 *
 * @param[in]		- rtc_date: Pointer to the date structure obtained
 * 					  from the RTC.
 *
 * @return			- char*: The address of the string containing the
 * 					  formatted date data.
 *
 * @Note			- Safe to return as the memory is allocated as "static".
 ************************************************************************/
char* Date_To_String(RTC_date_t *rtc_date)
{
	static char buf[9];

	buf[2] = '/';
	buf[5] = '/';

	Number_To_String(rtc_date->date,buf);
	Number_To_String(rtc_date->month,&buf[3]);
	Number_To_String(rtc_date->year,&buf[6]);

	buf[8] = '\0';

	return buf;
}

