/*
 * bmp280.h
 *
 *  Created on: 28 May 2026
 *      Author: kaan_
 */

#ifndef BMP280_H_
#define BMP280_H_

#include "main.h"



/**
 * @brief BMP280 driver status codes.
 */
typedef enum
{
	BMP280_OK,
	BMP280_ERROR,
}bmp280_status_t;


/**
 * @brief IIR filter coefficient configuration.
 */
typedef enum
{
	BMP280_IIR_FILTER_OFF = 0x00U,
	BMP280_IIR_FILTER_2   = 0x01U,
	BMP280_IIR_FILTER_4   = 0x02U,
	BMP280_IIR_FILTER_8   = 0x03U,
	BMP280_IIR_FILTER_16  = 0x04U
}bmp280_iir_filter_t;


/**
 * @brief Oversampling configuration for temperature and pressure.
 */
typedef enum
{
	BMP280_OVERSAMPLING_OFF =	0x00U,
	BMP280_OVERSAMPLING_1X 	= 	0x01U,
	BMP280_OVERSAMPLING_2X 	= 	0x02U,
	BMP280_OVERSAMPLING_4X 	= 	0x03U,
	BMP280_OVERSAMPLING_8X 	= 	0x04U,
	BMP280_OVERSAMPLING_16X = 	0x05U
} bmp280_over_sampling_t;


/**
 * @brief Device mode of the BMP280.
 */
typedef enum
{
	BMP280_SLEEP_MODE  = 0x00U,
	BMP280_FORCED_MODE = 0x01U,
	BMP280_NORMAL_MODE = 0x03U
}bmp280_device_mode;


/**
 * @brief Standby time configuration in normal mode.
 */
typedef enum
{
	BMP280_STANDBY_500US   = 0x00U,
	BMP280_STANDBY_62500US = 0x01U,
	BMP280_STANDBY_125MS   = 0x02U,
	BMP280_STANDBY_250MS   = 0x03U,
	BMP280_STANDBY_500MS   = 0x04U,
	BMP280_STANDBY_1S	   = 0x05U,
	BMP280_STANDBY_2S	   = 0x06U,
	BMP280_STANDBY_4S	   = 0x07U,
}bmp280_standby_time_t;


/**
 * @brief Structure to hold the factory calibration parameters.
 * @note Must be packed to align exactly with the sensor's memory map.
 */
#pragma pack(1)
typedef struct
{
	uint16_t T1;
	int16_t T2;
	int16_t T3;
	uint16_t P1;
	int16_t P2;
	int16_t P3;
	int16_t P4;
	int16_t P5;
	int16_t P6;
	int16_t P7;
	int16_t P8;
	int16_t P9;
} bmp280_calibration_parameter_t;
#pragma pack()


/**
 * @brief Structure to hold calculated pressure and temperature data.
 */
typedef struct
{
	int32_t pressure;
	int32_t temperature;
}bmp280_sensor_data_t;



/*
 * BMP280 device addresses
 */
#define BMP280_CHIP_ID_VALUE	    ((uint8_t) 0x58U)
#define BMP280_I2C_ADDR				((uint8_t) 0x76U)
#define BMP280_CALIB_REG			((uint8_t) 0x88U)
#define BMP280_ID					((uint8_t) 0xD0U)
#define BMP280_RESET				((uint8_t) 0xE0U)
#define BMP280_STATUS				((uint8_t) 0xF3U)
#define BMP280_CTRL_MEAS			((uint8_t) 0xF4U)
#define BMP280_CONFIG				((uint8_t) 0xF5U)
#define BMP280_PRESS_MSB			((uint8_t) 0xF7U)
#define BMP280_PRESS_LSB			((uint8_t) 0xF8U)
#define BMP280_PRESS_XLSB			((uint8_t) 0xF9U)
#define BMP280_TEMP_MSB				((uint8_t) 0xFAU)
#define BMP280_TEMP_LSB				((uint8_t) 0xFBU)
#define BMP280_TEMP_XLSB			((uint8_t) 0xFCU)




/*
 * Functions
 */
bmp280_status_t BMP280_Init(I2C_HandleTypeDef *hi2c, uint8_t i2c_Dev_Addr);
bmp280_status_t BMP280_Set_Pressure_Sampling(I2C_HandleTypeDef *hi2c, bmp280_over_sampling_t press_Sampling);
bmp280_status_t BMP280_Set_Temperature_Sampling(I2C_HandleTypeDef *hi2c, bmp280_over_sampling_t temp_Sampling);
bmp280_status_t BMP280_Set_Iir_Filter(I2C_HandleTypeDef *hi2c, bmp280_iir_filter_t iir_Filter);
bmp280_status_t BMP280_Set_Device_Mode(I2C_HandleTypeDef *hi2c, bmp280_device_mode device_Mode);
bmp280_status_t BMP280_Set_Standby_Time(I2C_HandleTypeDef *hi2c, bmp280_standby_time_t standby_Time);
bmp280_status_t BMP280_Get_Pressure(I2C_HandleTypeDef *hi2c, uint32_t *pressure);
bmp280_status_t BMP280_Get_Temperature(I2C_HandleTypeDef *hi2c, float *temperature);
bmp280_status_t BMP280_Get_Altitude(I2C_HandleTypeDef *hi2c, float *altitude);



#endif /* BMP280_H_ */
