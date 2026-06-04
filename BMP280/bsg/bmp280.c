/*
 * bmp280.c
 *
 *  Created on: 28 May 2026
 *      Author: kaan_
 */

#include "bmp280.h"
#include <math.h>


#define I2C_TIMEOUT 500UL
#define STANDARD_SEA_LEVEL_PRESSURE_PA  101325.0f
#define ALTITUDE_FORMULA_CONSTANT_1     44330.0f
#define ALTITUDE_FORMULA_CONSTANT_2     0.190295f


static uint8_t BMP280_I2c_Addr;
static int32_t t_fine;
static bmp280_calibration_parameter_t dig;

/*
 * Private helper function prototypes
 */
static bmp280_status_t _BMP280_Get_Raw_Pressure(I2C_HandleTypeDef *hi2c, int32_t *adc_P);
static bmp280_status_t _BMP280_Get_Raw_Temperature(I2C_HandleTypeDef *hi2c, int32_t *adc_T);
static bmp280_status_t _BMP280_Read_Byte(I2C_HandleTypeDef *hi2c, uint8_t reg_Addr, uint8_t *data);
static bmp280_status_t _BMP280_Write_Byte(I2C_HandleTypeDef *hi2c, uint8_t reg_Addr, uint8_t data);
static bmp280_status_t _BMP280_Read(I2C_HandleTypeDef *hi2c, uint8_t reg_Base_Addr, uint8_t *buffer, uint32_t n_Bytes);
static bmp280_status_t _BMP280_Write(I2C_HandleTypeDef *hi2c, uint8_t reg_Addr, uint8_t *data, uint32_t len);



/************************************************************************
 * @fn				- BMP280_Init
 *
 * @brief			- Initializes the BMP280 sensor over I2C.
 *
 * @param[in]		- hi2c: Pointer to the I2C handle.
 * @param[in]		- i2c_Dev_Addr: I2C device address of the BMP280.
 *
 * @return			- bmp280_status_t: BMP280_OK on success, BMP280_ERROR on fail.
 *
 * @Note			- Verifies chip ID, sets default sampling/mode, and
 * 					  reads factory calibration parameters.
 ************************************************************************/
bmp280_status_t BMP280_Init(I2C_HandleTypeDef *hi2c, uint8_t i2c_Dev_Addr)
{
	BMP280_I2c_Addr = i2c_Dev_Addr;

	// Read 1 byte from the sensor
	uint8_t read_Byte  = 0U;
	if (_BMP280_Read_Byte(hi2c, BMP280_ID, &read_Byte) != BMP280_OK)
	{
		return BMP280_ERROR;
	}

	// Check device address
	if ( read_Byte != BMP280_CHIP_ID_VALUE)
	{
		return BMP280_ERROR;
	}

	BMP280_Set_Temperature_Sampling(hi2c, BMP280_OVERSAMPLING_1X);
	BMP280_Set_Pressure_Sampling(hi2c, BMP280_OVERSAMPLING_1X);
	BMP280_Set_Device_Mode(hi2c, BMP280_NORMAL_MODE);

	// Read calibration parameters
	if(_BMP280_Read(hi2c, BMP280_CALIB_REG, (uint8_t*)&dig, 24) != BMP280_OK)
	{
		return BMP280_ERROR;
	}

	return BMP280_OK;
}


/************************************************************************
 * @fn				- BMP280_Get_Temperature
 *
 * @brief			- Reads and calculates the compensated temperature.
 *
 * @param[in]		- hi2c: Pointer to the I2C handle.
 * @param[out]		- temperature: Pointer to store the calculated temperature in Celsius.
 *
 * @return			- bmp280_status_t: Status of the read operation.
 *
 * @Note			- Updates the global 't_fine' variable required for pressure calculation.
 ************************************************************************/
bmp280_status_t BMP280_Get_Temperature(I2C_HandleTypeDef *hi2c, float *temperature)
{
	if (hi2c == NULL || temperature == NULL)
	{
		return BMP280_ERROR;
	}

	int32_t var1 = 0, var2 = 0, adc_T = 0;

	if( _BMP280_Get_Raw_Temperature(hi2c, &adc_T) != BMP280_OK)
	{
		 return BMP280_ERROR;
	}

	var1 = ((((adc_T >> 3) - ((int32_t)dig.T1 << 1))) * ((int32_t)dig.T2)) >> 11;
	var2 = (((((adc_T >> 4) - ((int32_t)dig.T1)) * ((adc_T >> 4) - ((int32_t)dig.T1))) >> 12) * ((int32_t)dig.T3)) >> 14;

	t_fine = var1 + var2;
	int32_t temp_temperature = (t_fine * 5 + 128) >> 8;
	*temperature = ((float)temp_temperature) / 100.0f;

	return BMP280_OK;
}


/************************************************************************
 * @fn				- _BMP280_Get_Raw_Temperature
 *
 * @brief			- Reads the raw, uncompensated temperature data from registers.
 *
 * @internal
 * This is a private helper function.
 *
 * @param[in]		- hi2c: Pointer to the I2C handle.
 * @param[out]		- adc_T: Pointer to store the 20-bit raw temperature value.
 *
 * @return			- bmp280_status_t: Status of the I2C read.
 ************************************************************************/
static bmp280_status_t _BMP280_Get_Raw_Temperature(I2C_HandleTypeDef *hi2c, int32_t *adc_T)
{
    uint8_t buffer[3] = {0};

    if (_BMP280_Read(hi2c, BMP280_TEMP_MSB, buffer, 3) != BMP280_OK)
    {
        return BMP280_ERROR;
    }

    *adc_T = (int32_t)( ( ( (uint32_t)buffer[0]) << 12) |
                        ( ( (uint32_t)buffer[1]) << 4)  |
                        ( ( (uint32_t)buffer[2]) >> 4) );

    return BMP280_OK;
}


/************************************************************************
 * @fn				- BMP280_Get_Pressure
 *
 * @brief			- Reads and calculates the compensated pressure.
 *
 * @param[in]		- hi2c: Pointer to the I2C handle.
 * @param[out]		- pressure: Pointer to store the calculated pressure in Pascals.
 *
 * @return			- bmp280_status_t: Status of the read operation.
 *
 * @Note			- Requires BMP280_Get_Temperature() to be called first
 * to generate a valid 't_fine'.
 ************************************************************************/
bmp280_status_t BMP280_Get_Pressure(I2C_HandleTypeDef *hi2c, uint32_t *pressure)
{
	if (hi2c == NULL || pressure == NULL)
	{
		return BMP280_ERROR;
	}

	int64_t var1 = 0, var2 = 0, pressure_Temp = 0;
	int32_t adc_P = 0;

	if( _BMP280_Get_Raw_Pressure(hi2c, &adc_P) != BMP280_OK)
	{
		 return BMP280_ERROR;
	}

	var1 = ( (int64_t)t_fine) - 128000;
	var2 = (var1 * var1 * (int64_t)dig.P6);
	var2 = var2 + ( (var1 * ( (int64_t)dig.P5) ) << 17 );
	var2 = var2 + ( (int64_t)dig.P4 << 35 );
	var1 = (( var1 * var1 * (int64_t)dig.P3) >> 8) + ( (var1 * (int64_t)dig.P2) << 12);
	var1 = ( ( (( (int64_t) 1) << 47 ) + var1) ) *  ( (uint64_t) dig.P1) >> 33;

	if(var1 == 0)
	{
		*pressure = 0;
		return BMP280_ERROR;
	}

	pressure_Temp = 1048576 - adc_P;
	pressure_Temp = ( ((pressure_Temp << 31) - var2) * 3125 ) / var1;
	var1 = ( ((int64_t) dig.P9) * (pressure_Temp >> 13) * (pressure_Temp >> 13) ) >> 25;
	var2 = ( ((int64_t) dig.P8) * pressure_Temp ) >> 19;
	pressure_Temp = ( (pressure_Temp + var1 + var2) >> 8) + ( ((int64_t) dig.P7) << 4);

	*pressure = (uint32_t) (pressure_Temp / 256);

	return BMP280_OK;
}


/************************************************************************
 * @fn				- _BMP280_Get_Raw_Pressure
 *
 * @brief			- Reads the raw, uncompensated pressure data from registers.
 *
 * @internal
 * This is a private helper function.
 *
 * @param[in]		- hi2c: Pointer to the I2C handle.
 * @param[out]		- adc_P: Pointer to store the 20-bit raw pressure value.
 *
 * @return			- bmp280_status_t: Status of the I2C read.
 ************************************************************************/
static bmp280_status_t _BMP280_Get_Raw_Pressure(I2C_HandleTypeDef *hi2c, int32_t *adc_P)
{
    uint8_t buffer[3] = {0};

    if (_BMP280_Read(hi2c, BMP280_PRESS_MSB, buffer, 3) != BMP280_OK)
    {
        return BMP280_ERROR;
    }

    *adc_P = (int32_t)( ( ( (uint32_t)buffer[0]) << 12) |
                        ( ( (uint32_t)buffer[1]) << 4)  |
                        ( ( (uint32_t)buffer[2]) >> 4) );

    return BMP280_OK;
}


/************************************************************************
 * @fn				- BMP280_Get_Altitude
 *
 * @brief			- Calculates altitude based on current pressure and sea-level pressure.
 *
 * @param[in]		- hi2c: Pointer to the I2C handle.
 * @param[out]		- altitude: Pointer to store the calculated altitude in meters.
 *
 * @return			- bmp280_status_t: Status of the operation.
 *
 * @Note			- Uses the international barometric formula. Standard sea-level
 * pressure is assumed to be 101325.0 Pa.
 ************************************************************************/
bmp280_status_t BMP280_Get_Altitude(I2C_HandleTypeDef *hi2c, float *altitude)
{
    uint32_t current_pressure = 0;

    // 1. Yazdığın hatasız basınç fonksiyonunu kullanarak güncel basıncı çekiyoruz
    if (BMP280_Get_Pressure(hi2c, &current_pressure) != BMP280_OK)
    {
        return BMP280_ERROR;
    }

    // 2. Sensörden veri okunamadıysa sıfıra bölünme hatasını engellemek için güvenlik kontrolü
    if (current_pressure == 0)
    {
        *altitude = 0.0f;
        return BMP280_ERROR;
    }

    // 3. İrtifa Formülü (Standart Deniz Seviyesi Basıncı: 101325.0 Pa kabul edilmiştir)
    float p_ratio = (float)current_pressure / STANDARD_SEA_LEVEL_PRESSURE_PA;

    // h = 44330 * (1 - (p / p0)^(1/5.255)) formülünün C dilindeki hali
    *altitude = ALTITUDE_FORMULA_CONSTANT_1 * (1.0f - powf(p_ratio, ALTITUDE_FORMULA_CONSTANT_2));

    return BMP280_OK;
}


/************************************************************************
 * @fn				- BMP280_Set_Pressure_Sampling
 *
 * @brief			- Sets the oversampling rate for pressure measurements.
 *
 * @param[in]		- hi2c: Pointer to the I2C handle.
 * @param[in]		- press_Sampling: Desired pressure oversampling setting.
 *
 * @return			- bmp280_status_t: Status of the configuration.
 ************************************************************************/
bmp280_status_t BMP280_Set_Pressure_Sampling(I2C_HandleTypeDef *hi2c, bmp280_over_sampling_t press_Sampling)
{
	uint8_t value = 0;

	if (_BMP280_Read_Byte(hi2c, BMP280_CTRL_MEAS, &value) != BMP280_OK)
	{
		return BMP280_ERROR;
	}

	value &= ~( (0x7U) << 2);
	value |= ( (uint8_t)press_Sampling << 2);
	if (_BMP280_Write_Byte(hi2c, BMP280_CTRL_MEAS, value))
	{
		return BMP280_ERROR;
	}

	return BMP280_OK;
}


/************************************************************************
 * @fn				- BMP280_Set_Temperature_Sampling
 *
 * @brief			- Sets the oversampling rate for temperature measurements.
 *
 * @param[in]		- hi2c: Pointer to the I2C handle.
 * @param[in]		- temp_Sampling: Desired temperature oversampling setting.
 *
 * @return			- bmp280_status_t: Status of the configuration.
 ************************************************************************/
bmp280_status_t BMP280_Set_Temperature_Sampling(I2C_HandleTypeDef *hi2c, bmp280_over_sampling_t temp_Sampling)
{
	uint8_t value = 0;

	if (_BMP280_Read_Byte(hi2c, BMP280_CTRL_MEAS, &value) != BMP280_OK)
	{
		return BMP280_ERROR;
	}

	value &= ~( (0x7U) << 5);
	value |= ( (uint8_t)temp_Sampling << 5);
	if (_BMP280_Write_Byte(hi2c, BMP280_CTRL_MEAS, value))
	{
		return BMP280_ERROR;
	}

	return BMP280_OK;
}


/************************************************************************
 * @fn				- BMP280_Set_Iir_Filter
 *
 * @brief			- Configures the Internal Infinite Impulse Response (IIR) filter.
 *
 * @param[in]		- hi2c: Pointer to the I2C handle.
 * @param[in]		- iir_Filter: Desired IIR filter coefficient.
 *
 * @return			- bmp280_status_t: Status of the configuration.
 ************************************************************************/
bmp280_status_t BMP280_Set_Iir_Filter(I2C_HandleTypeDef *hi2c, bmp280_iir_filter_t iir_Filter)
{
	uint8_t value = 0;

	if (_BMP280_Read_Byte(hi2c, BMP280_CONFIG, &value) != BMP280_OK)
	{
		return BMP280_ERROR;
	}

	value &= ~( (0x7U) << 2);
	value |= ( (uint8_t)iir_Filter << 2);
	if (_BMP280_Write_Byte(hi2c, BMP280_CONFIG, value))
	{
		return BMP280_ERROR;
	}

	return BMP280_OK;
}


/************************************************************************
 * @fn				- BMP280_Set_Device_Mode
 *
 * @brief			- Sets the power mode of the sensor (Sleep, Forced, Normal).
 *
 * @param[in]		- hi2c: Pointer to the I2C handle.
 * @param[in]		- device_Mode: Desired operational mode.
 *
 * @return			- bmp280_status_t: Status of the configuration.
 ************************************************************************/
bmp280_status_t BMP280_Set_Device_Mode(I2C_HandleTypeDef *hi2c, bmp280_device_mode device_Mode)
{
	uint8_t value = 0;

	if (_BMP280_Read_Byte(hi2c, BMP280_CTRL_MEAS, &value) != BMP280_OK)
	{
		return BMP280_ERROR;
	}

	value &= ~(0x3U);
	value |= (uint8_t)device_Mode;
	if (_BMP280_Write_Byte(hi2c, BMP280_CTRL_MEAS, value))
	{
		return BMP280_ERROR;
	}

	return BMP280_OK;
}


/************************************************************************
 * @fn				- BMP280_Set_Standby_Time
 *
 * @brief			- Configures the inactive duration in normal mode.
 *
 * @param[in]		- hi2c: Pointer to the I2C handle.
 * @param[in]		- standby_Time: Desired standby time between measurements.
 *
 * @return			- bmp280_status_t: Status of the configuration.
 ************************************************************************/
bmp280_status_t BMP280_Set_Standby_Time(I2C_HandleTypeDef *hi2c, bmp280_standby_time_t standby_Time)
{
	uint8_t value = 0;

	if (_BMP280_Read_Byte(hi2c, BMP280_CONFIG, &value) != BMP280_OK)
	{
		return BMP280_ERROR;
	}

	value &= ~( (0x7U) << 5);
	value |= (uint8_t)standby_Time;
	if (_BMP280_Write_Byte(hi2c, BMP280_CONFIG, value))
	{
		return BMP280_ERROR;
	}

	return BMP280_OK;
}


/************************************************************************
 * @fn				- _BMP280_Read_Byte
 *
 * @brief			- Reads a single byte from a specific BMP280 register.
 *
 * @internal
 * This is a private helper function used to isolate HAL I2C dependencies.
 *
 * @param[in]		- hi2c: Pointer to the I2C handle.
 * @param[in]		- reg_Addr: The internal register address of the BMP280.
 * @param[out]		- data: Pointer to the variable where the read byte will be stored.
 *
 * @return			- bmp280_status_t: BMP280_OK on success, BMP280_ERROR on failure.
 *
 * @Note			- Uses a predefined I2C_TIMEOUT for blocking protection.
 ************************************************************************/
static bmp280_status_t _BMP280_Read_Byte(I2C_HandleTypeDef *hi2c, uint8_t reg_Addr, uint8_t *data)
{
	HAL_StatusTypeDef status = HAL_I2C_Mem_Read(hi2c, (BMP280_I2c_Addr << 1), reg_Addr, I2C_MEMADD_SIZE_8BIT , data, 1, I2C_TIMEOUT);

	return (status == HAL_OK) ? BMP280_OK : BMP280_ERROR;
}


/************************************************************************
 * @fn				- _BMP280_Read
 *
 * @brief			- Reads a burst of multiple bytes starting from a specific register.
 *
 * @internal
 * This is a private helper function used to read multi-byte data.
 *
 * @param[in]		- hi2c: Pointer to the I2C handle.
 * @param[in]		- reg_Base_Addr: The starting register address for the burst read.
 * @param[out]		- buffer: Pointer to the array where the read bytes will be stored.
 * @param[in]		- n_Bytes: The number of bytes to read sequentially.
 *
 * @return			- bmp280_status_t: BMP280_OK on success, BMP280_ERROR on failure.
 *
 * @Note			- Uses HAL_MAX_DELAY to ensure the entire burst completes.
 ************************************************************************/
static bmp280_status_t _BMP280_Read(I2C_HandleTypeDef *hi2c, uint8_t reg_Base_Addr, uint8_t *buffer, uint32_t n_Bytes)
{
	HAL_StatusTypeDef status = HAL_I2C_Mem_Read(hi2c, BMP280_I2c_Addr << 1, reg_Base_Addr, I2C_MEMADD_SIZE_8BIT , buffer, n_Bytes, I2C_TIMEOUT);

	return (status == HAL_OK) ? BMP280_OK : BMP280_ERROR;
}


/************************************************************************
 * @fn				- _BMP280_Write_Byte
 *
 * @brief			- Writes a single byte to a specific BMP280 register.
 *
 * @internal
 * This is a private helper function used to configure the sensor.
 *
 * @param[in]		- hi2c: Pointer to the I2C handle.
 * @param[in]		- reg_Addr: The target internal register address of the BMP280.
 * @param[in]		- data: The 8-bit value to write into the register.
 *
 * @return			- bmp280_status_t: BMP280_OK on success, BMP280_ERROR on failure.
 *
 * @Note			- Uses a predefined I2C_TIMEOUT.
 ************************************************************************/
static bmp280_status_t _BMP280_Write_Byte(I2C_HandleTypeDef *hi2c, uint8_t reg_Addr, uint8_t data)
{
	HAL_StatusTypeDef status = HAL_I2C_Mem_Write(hi2c, BMP280_I2c_Addr << 1, reg_Addr, I2C_MEMADD_SIZE_8BIT , &data, 1, I2C_TIMEOUT);

	return (status == HAL_OK) ? BMP280_OK : BMP280_ERROR;
}


/************************************************************************
 * @fn				- _BMP280_Write
 *
 * @brief			- Writes a burst of bytes to the BMP280 starting from a register.
 *
 * @internal
 * This is a private helper function.
 *
 * @param[in]		- hi2c: Pointer to the I2C handle.
 * @param[in]		- reg_Addr: The starting register address.
 * @param[in]		- data: Pointer to the array of data to be written.
 * @param[in]		- len: The number of bytes to write.
 *
 * @return			- bmp280_status_t: BMP280_OK on success, BMP280_ERROR on failure.
 *
 * @Note			- Handled with a standard I2C_TIMEOUT.
 ************************************************************************/
static bmp280_status_t _BMP280_Write(I2C_HandleTypeDef *hi2c, uint8_t reg_Addr, uint8_t *data, uint32_t len)
{
	HAL_StatusTypeDef status = HAL_I2C_Mem_Write(hi2c, BMP280_I2c_Addr << 1, reg_Addr, I2C_MEMADD_SIZE_8BIT , data, len, I2C_TIMEOUT);

	return (status == HAL_OK) ? BMP280_OK : BMP280_ERROR;
}
