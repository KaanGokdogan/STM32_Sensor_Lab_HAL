/*
 * main1.c
 *
 *  Created on: 11 May 2026
 *      Author: kaan_
 */


#include "main.h"


int main(void)
{
	RTC_time_t current_Time;
	RTC_date_t current_Date;

	HAL_Init();
	SystemClock_Config();

	printf("Program working.");

	if(DS1307_Init())
	{
		printf("rtc inity has failed");
		while(1);
	}

	current_Date.day = TUESDAY;
	current_Date.date = 12;
	current_Date.month = 5;
	current_Date.year = 26;

	current_Time.hour = 7;
	current_Time.min = 31;
	current_Time.sec = 47;
	current_Time.time_Format = TIME_FORMAT_12HRS_PM;

	DS1307_Set_Current_Date(&current_Date);
	DS1307_Set_Current_Time(&current_Time);

	while (1)
	{
		DS1307_Get_Current_Time(&current_Time);

		char *am_Pm;
		if(current_Time.time_Format != TIME_FORMAT_24HR)
		{
			am_Pm = (current_Time.time_Format) ? "PM" : "AM";
			printf("Current time = %s %s\n",Time_To_String(&current_Time),am_Pm); // 04:25:43 PM
		}
		else
		{
			printf("Current time = %s\n",Time_To_String(&current_Time)); // 04:25:43
		}

		DS1307_Get_Current_Date(&current_Date);
		// 04/02/26 <Tuesday>
		printf("Current date = %s <%s>\n",Date_To_String(&current_Date),Get_Day_Of_Week(current_Date.day));
	}

	return 0;
}


void SystemClock_Config(void)
{

}


char* Get_Day_Of_Week(uint8_t i)
{
	char* days[] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", };

	return days[i - 1];
}


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


// hh:mm:ss
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


// dd:mm:yy
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

