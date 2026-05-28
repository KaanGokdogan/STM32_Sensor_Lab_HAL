/*
 * bmp280.c
 *
 *  Created on: 28 May 2026
 *      Author: kaan_
 */

#include "bmp280.h"



#define I2C_TIMEOUT 500UL
static uint8_t BMP280_I2c_Addr;


bmp280_status_t _BMP280_Read_Byte(I2C_HandleTypeDef *hi2c, uint8_t reg_Addr, uint8_t *data);
bmp280_status_t _BMP280_Write_Byte(I2C_HandleTypeDef *hi2c, uint8_t reg_Addr, uint8_t data);
bmp280_status_t _BMP280_Read(I2C_HandleTypeDef *hi2c, uint8_t reg_Base_Addr, uint8_t *buffer, uint32_t n_Bytes);
bmp280_status_t _BMP280_Write(I2C_HandleTypeDef *hi2c, uint8_t reg_Addr, uint8_t *data, uint32_t len);



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
	if ( read_Byte != 0x58U)
	{
		return BMP280_ERROR;
	}

	// Write 1 byte to the sensor
	uint8_t data = 0x3U;
	if (_BMP280_Write_Byte(hi2c, BMP280_CTRL_MEAS, data) != BMP280_OK)
	{
		return BMP280_ERROR;
	}

	return BMP280_OK;
}


//bmp280_status_t BMP280_Sensor_Data(I2C_HandleTypeDef *hi2c, uint8_t i2c_Dev_Addr, bmp280_sensor_data_t *sensor_Data)
//{
//	uint8_t raw_Data[6] = {0U};
//
//	if (_BMP280_Read(hi2c, BMP280_PRESS_MSB, raw_Data, sizeof(raw_Data)) != BMP280_OK)
//	{
//    return BMP280_ERROR;
//	}
//
//	sensor_Data->pressure = 	;
//	sensor_Data->temperature = 	;
//
//
//
//	return BMP280_OK;
//}




bmp280_status_t _BMP280_Read_Byte(I2C_HandleTypeDef *hi2c, uint8_t reg_Addr, uint8_t *data)
{
	HAL_StatusTypeDef status = HAL_I2C_Mem_Read(hi2c, (BMP280_I2c_Addr << 1), reg_Addr, I2C_MEMADD_SIZE_8BIT , data, 1, I2C_TIMEOUT);

	return (status == HAL_OK) ? BMP280_OK : BMP280_ERROR;
}


bmp280_status_t _BMP280_Read(I2C_HandleTypeDef *hi2c, uint8_t reg_Base_Addr, uint8_t *buffer, uint32_t n_Bytes)
{
	HAL_StatusTypeDef status = HAL_I2C_Mem_Read(hi2c, BMP280_I2c_Addr << 1, reg_Base_Addr, I2C_MEMADD_SIZE_8BIT , buffer, n_Bytes, I2C_TIMEOUT);

	return (status == HAL_OK) ? BMP280_OK : BMP280_ERROR;
}


bmp280_status_t _BMP280_Write_Byte(I2C_HandleTypeDef *hi2c, uint8_t reg_Addr, uint8_t data)
{
	HAL_StatusTypeDef status = HAL_I2C_Mem_Write(hi2c, BMP280_I2c_Addr << 1, reg_Addr, I2C_MEMADD_SIZE_8BIT , &data, 1, I2C_TIMEOUT);

	return (status == HAL_OK) ? BMP280_OK : BMP280_ERROR;
}


bmp280_status_t _BMP280_Write(I2C_HandleTypeDef *hi2c, uint8_t reg_Addr, uint8_t *data, uint32_t len)
{
	HAL_StatusTypeDef status = HAL_I2C_Mem_Write(hi2c, BMP280_I2c_Addr << 1, reg_Addr, I2C_MEMADD_SIZE_8BIT , data, len, I2C_TIMEOUT);

	return (status == HAL_OK) ? BMP280_OK : BMP280_ERROR;
}
