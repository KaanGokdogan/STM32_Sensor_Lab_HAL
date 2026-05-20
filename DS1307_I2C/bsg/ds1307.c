/*
 * ds1307.c
 *
 *  Created on: 7 May 2026
 *      Author: kaan_
 */

#include "ds1307.h"


I2C_HandleTypeDef ds1307I2cHandle;

/*
 * Private helper function prototypes
 */
static void _DS1307_I2C_Pin_Config(void);
static void _DS1307_I2C_Config(void);
static void _DS1307_Write(uint8_t Value, uint8_t RegAddr);
static uint8_t _DS1307_Read(uint8_t RegAddr);



/************************************************************************
 * @fn				- DS1307_Init
 *
 * @brief			- Initializes the DS1307 RTC module and I2C peripheral.
 *
 * @param[in]		- none
 *
 * @return			- uint8_t: 0 if initialization is successful,
 * 							   1 if clock halt bit is set (init failed).
 *
 * @Note			- Clears the Clock Halt (CH) bit to start the oscillator.
 ************************************************************************/
uint8_t DS1307_Init()
{
	// 1. Init the I2C pins
	_DS1307_I2C_Pin_Config();

	// 2. Initialize the I2C peripheral
	_DS1307_I2C_Config();

	// 3. Make clock halt = 0
	_DS1307_Write(0x00, DS1307_ADDR_SEC);

	// 4. Read back clock halt bit
	uint8_t clock_State = _DS1307_Read(DS1307_ADDR_SEC);

	return (clock_State >> 7) & 0x1;
}


/************************************************************************
 * @fn				- DS1307_Set_Current_Time
 *
 * @brief			- Sets the current time in the DS1307 RTC registers.
 *
 * @param[in]		- rtc_time: Pointer to the RTC_time_t structure containing
 * 					  the time to be set.
 *
 * @return			- none
 *
 * @Note			- Handles the conversion from binary to BCD format and
 * 					  configures 12-hour or 24-hour mode accordingly.
 ************************************************************************/
void DS1307_Set_Current_Time(RTC_time_t *rtc_time)
{
	uint8_t seconds, hours;

	seconds = Binary_To_BCD(rtc_time->sec);
	seconds &= ~( 1 << 7);
	_DS1307_Write(seconds, DS1307_ADDR_SEC);

	_DS1307_Write(Binary_To_BCD(rtc_time->min), DS1307_ADDR_MIN);

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
	_DS1307_Write(hours, DS1307_ADDR_HOUR);
}


/************************************************************************
 * @fn				- DS1307_Get_Current_Time
 *
 * @brief			- Reads the current time from the DS1307 RTC registers.
 *
 * @param[in]		- rtc_time: Pointer to the RTC_time_t structure where
 * 					  the read time will be stored.
 *
 * @return			- none
 *
 * @Note			- Handles the conversion from BCD format to binary.
 ************************************************************************/
void DS1307_Get_Current_Time(RTC_time_t *rtc_time)
{
	uint8_t seconds, hours;

	seconds = _DS1307_Read(DS1307_ADDR_SEC);
	seconds &= ~( 1 << 7);
	rtc_time->sec = BCD_To_Binary(seconds);

	rtc_time->min = BCD_To_Binary(_DS1307_Read(DS1307_ADDR_MIN));

	hours = _DS1307_Read(DS1307_ADDR_HOUR);
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


/************************************************************************
 * @fn				- DS1307_Set_Current_Date
 *
 * @brief			- Sets the current date in the DS1307 RTC registers.
 *
 * @param[in]		- rtc_date: Pointer to the RTC_date_t structure containing
 * 					  the date to be set.
 *
 * @return			- none
 *
 * @Note			- Converts the date, month, year, and day to BCD before writing.
 ************************************************************************/
void DS1307_Set_Current_Date(RTC_date_t *rtc_date)
{
	_DS1307_Write(Binary_To_BCD(rtc_date->date), DS1307_ADDR_DATE);
	_DS1307_Write(Binary_To_BCD(rtc_date->day), DS1307_ADDR_DAY);
	_DS1307_Write(Binary_To_BCD(rtc_date->month), DS1307_ADDR_MONTH);
	_DS1307_Write(Binary_To_BCD(rtc_date->year), DS1307_ADDR_YEAR);
}


/************************************************************************
 * @fn				- DS1307_Get_Current_Date
 *
 * @brief			- Reads the current date from the DS1307 RTC registers.
 *
 * @param[in]		- rtc_date: Pointer to the RTC_date_t structure where
 * 					  the read date will be stored.
 *
 * @return			- none
 *
 * @Note			- Converts the BCD format data back to binary.
 ************************************************************************/
void DS1307_Get_Current_Date(RTC_date_t *rtc_date)
{
	rtc_date->date = BCD_To_Binary(_DS1307_Read(DS1307_ADDR_DATE));
	rtc_date->day = BCD_To_Binary(_DS1307_Read(DS1307_ADDR_DAY));
	rtc_date->month = BCD_To_Binary(_DS1307_Read(DS1307_ADDR_MONTH));
	rtc_date->year = BCD_To_Binary(_DS1307_Read(DS1307_ADDR_YEAR));
}

/************************************************************************
 *						PRIVATE HELPER FUNCTIONS
 ************************************************************************/
/************************************************************************
 * @fn				- _DS1307_I2C_Pin_Config
 *
 * @brief			- Configures the GPIO pins for I2C communication.
 *
 * @internal
 * This is a private helper function. It is not exposed to the user application.
 * It isolates the low-level hardware pin initialization from the main RTC logic.
 *
 * @param[in]		- none
 *
 * @return			- none
 *
 * @Note			- Configures PB6 (SCL) and PB9 (SDA) in Alternate
 * 					 Function Open-Drain mode.
 ************************************************************************/
static void _DS1307_I2C_Pin_Config(void)
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


/************************************************************************
 * @fn				- _DS1307_I2C_Config
 *
 * @brief			- Configures the I2C peripheral settings for the RTC.
 *
 * @internal
 * This is a private helper function. It hides the HAL-specific I2C
 * configuration structures from the high-level RTC driver API.
 *
 * @param[in]		- none
 *
 * @return			- none
 *
 * @Note			- Configures I2C1 with 100 kHz standard mode speed.
 ************************************************************************/
static void _DS1307_I2C_Config(void)
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


/************************************************************************
 * @fn				- _DS1307_Write
 *
 * @brief			- Writes a 1-byte value to the specified DS1307 register.
 *
 * @internal
 * This is a private helper function. It is not exposed to the user application.
 * It encapsulates the raw HAL I2C transmission logic to prevent direct hardware
 * manipulation from higher layers.
 *
 * @param[in]		- Value: The 8-bit data to be written.
 * @param[in]		- RegAddr: The target register address inside the RTC.
 *
 * @return			- none
 *
 * @Note			- Uses HAL I2C Master Transmit function in polling mode.
 ************************************************************************/
static void _DS1307_Write(uint8_t Value, uint8_t RegAddr)
{
	uint8_t tx_Data[2];
	tx_Data[0] = RegAddr;
	tx_Data[1] = Value;

	HAL_I2C_Master_Transmit(&ds1307I2cHandle, DS1307_I2C_ADDRESS, tx_Data, 2, HAL_MAX_DELAY);
}


/************************************************************************
 * @fn				- _DS1307_Read
 *
 * @brief			- Reads a 1-byte value from the specified DS1307 register.
 *
 * @internal
 * This is a private helper function. It hides the sequential I2C transmission
 * and reception process (writing the register address, then reading the data)
 * from the high-level API.
 *
 * @param[in]		- RegAddr: The target register address to read from.
 *
 * @return			- uint8_t: The 8-bit data read from the register.
 *
 * @Note			- Sends the register address first, then reads the data.
 ************************************************************************/
static uint8_t _DS1307_Read(uint8_t RegAddr)
{
	uint8_t receive_Data;

	HAL_I2C_Master_Transmit(&ds1307I2cHandle, DS1307_I2C_ADDRESS, &RegAddr, 1, HAL_MAX_DELAY);
	HAL_I2C_Master_Receive(&ds1307I2cHandle, DS1307_I2C_ADDRESS, &receive_Data, 1, HAL_MAX_DELAY);

	return receive_Data;
}


/*************************************************************************
 * 							UTILITY FUNCTIONS
 ************************************************************************/
/************************************************************************
 * @fn				- Binary_To_BCD
 *
 * @brief			- Converts a standard binary/decimal value to Binary Coded Decimal (BCD).
 *
 * @param[in]		- Value: The binary value to be converted.
 *
 * @return			- uint8_t: The BCD formatted result.
 *
 * @Note			- RTC hardware registers expect data in BCD format.
 ************************************************************************/
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


/************************************************************************
 * @fn				- BCD_To_Binary
 *
 * @brief			- Converts a Binary Coded Decimal (BCD) value to standard binary.
 *
 * @param[in]		- Value: The BCD value to be converted.
 *
 * @return			- uint8_t: The standard binary/decimal formatted result.
 *
 * @Note			- Used when reading raw values from RTC registers.
 ************************************************************************/
uint8_t BCD_To_Binary(uint8_t Value)
{
	uint8_t first_Digit, second_Digit;

	first_Digit = ( Value >> 4) * 10;
	second_Digit = Value & (uint8_t)0x0F;

	return first_Digit + second_Digit;
}
