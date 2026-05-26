/*
 * MPU6050.c
 *
 *  Created on: 25 May 2026
 *      Author: kaan_
 */


#include "mpu6050.h"
#include <stdio.h>
#include <string.h>


#define I2C_TIMEOUT 500UL

static uint8_t mpu6050_I2c_Addr;

mpu6050_status_t MPU6050_Read_Byte(I2C_HandleTypeDef *hi2c, uint8_t reg_Addr, uint8_t *data);
mpu6050_status_t MPU6050_Read(I2C_HandleTypeDef *hi2c, uint8_t reg_Base_Addr, uint8_t *buffer, uint32_t n_Bytes);
mpu6050_status_t MPU6050_Write_Byte(I2C_HandleTypeDef *hi2c, uint8_t reg_Addr, uint8_t data);
mpu6050_status_t MPU6050_Write(I2C_HandleTypeDef *hi2c, uint8_t reg_Addr, uint8_t *data, uint32_t len);

mpu6050_status_t MPU6050_Init(I2C_HandleTypeDef *hi2c, uint8_t i2c_Dev_Addr)
{
	mpu6050_I2c_Addr = i2c_Dev_Addr;

	//CHECK_AND_HANDLE_ERROR(MPU6050_reset(hi2c));

	HAL_Delay(100);

	// Read 1 byte from the sensor
	uint8_t read_Byte  = 0;
	if (MPU6050_Read_Byte(hi2c, MPU6050_REG_WHOAMI, &read_Byte) != MPU6050_OK)
	{
		return MPU6050_ERR;
	}

#if 0
	if (read_byte == 0x68 || read_byte == 0x98)
	{
		printf("Valid MPU6050 found at address %X\n", MPU6050_I2C_ADDR);
	}
	else
	{
		printf("Invalid device found at address %X\n", MPU6050_I2C_ADDR);
		return MPU6050_ERR;
	}
#endif

	// Check device address
	if ( !(read_Byte == 0x68 || read_Byte == 0x98))
	{
		return MPU6050_ERR;
	}

	// Write 1 byte to the sensor
	uint8_t data = 0x00;
	if (MPU6050_Write_Byte(hi2c, MPU6050_REG_PWMGMT_1, data) != MPU6050_OK)
	{
		return MPU6050_ERR;
	}

	return MPU6050_OK;
}


mpu6050_status_t MPU6050_Read_Byte(I2C_HandleTypeDef *hi2c, uint8_t reg_Addr, uint8_t *data)
{
	HAL_StatusTypeDef status = HAL_I2C_Mem_Read(hi2c, (mpu6050_I2c_Addr << 1), reg_Addr, I2C_MEMADD_SIZE_8BIT , data, 1, I2C_TIMEOUT);

	return (status == HAL_OK) ? MPU6050_OK : MPU6050_ERR;
}


mpu6050_status_t MPU6050_Read(I2C_HandleTypeDef *hi2c, uint8_t reg_Base_Addr, uint8_t *buffer, uint32_t n_Bytes)
{
	HAL_StatusTypeDef status = HAL_I2C_Mem_Read(hi2c, mpu6050_I2c_Addr << 1, reg_Base_Addr, I2C_MEMADD_SIZE_8BIT , buffer, n_Bytes, HAL_MAX_DELAY);

	return (status == HAL_OK) ? MPU6050_OK : MPU6050_ERR;
}


mpu6050_status_t MPU6050_Write_Byte(I2C_HandleTypeDef *hi2c, uint8_t reg_Addr, uint8_t data)
{
	HAL_StatusTypeDef status = \
			HAL_I2C_Mem_Write(hi2c, mpu6050_I2c_Addr << 1, reg_Addr, I2C_MEMADD_SIZE_8BIT , &data, 1, I2C_TIMEOUT);

	return (status == HAL_OK) ? MPU6050_OK : MPU6050_ERR;
}


mpu6050_status_t MPU6050_Write(I2C_HandleTypeDef *hi2c, uint8_t reg_Addr, uint8_t *data, uint32_t len)
{
	HAL_StatusTypeDef status = HAL_I2C_Mem_Write(hi2c, mpu6050_I2c_Addr << 1, reg_Addr, I2C_MEMADD_SIZE_8BIT , data, len, I2C_TIMEOUT);

	return (status == HAL_OK) ? MPU6050_OK : MPU6050_ERR;
}


mpu6050_status_t MPU6050_Read_Accelerometer_Data(I2C_HandleTypeDef *hi2c, uint8_t i2c_Dev_Addr, mpu6050_accel_data_t *accel_Data)
{
	uint8_t raw_Data[6];
	mpu6050_status_t status = MPU6050_Read(hi2c, MPU6050_REG_ACCEL_START, raw_Data, sizeof(raw_Data));
	if (status != MPU6050_OK)
		return status;

	accel_Data->x = (int16_t)(raw_Data[0] << 8 | raw_Data[1]);
	accel_Data->y = (int16_t)(raw_Data[2] << 8 | raw_Data[3]);
	accel_Data->z = (int16_t)(raw_Data[4] << 8 | raw_Data[5]);

	return MPU6050_OK;
}

mpu6050_accel_data_t MPU6050_Accelerometer_Calibration_Data(mpu6050_accel_offset_t *error_Offset, mpu6050_accel_data_t *raw_Data)
{
	mpu6050_accel_data_t accel_Calibrated;
	accel_Calibrated.x = raw_Data->x - error_Offset->x;
	accel_Calibrated.y = raw_Data->y - error_Offset->y;
	accel_Calibrated.z = raw_Data->z - error_Offset->z;

	return accel_Calibrated;

}



void MPU6050_Calibrate(I2C_HandleTypeDef *hi2c, uint8_t i2c_Dev_Addr, mpu6050_accel_offset_t *error_Offset, uint16_t num_Samples)
{
    printf("Kalibrasyon basliyor... Lutfen sensoru HAREKETSİZ ve DUZ tutun!\r\n");
    HAL_Delay(1000);

    int32_t sum_x = 0;
    int32_t sum_y = 0;
    int32_t sum_z = 0;
    mpu6050_accel_data_t temp_data;

    for (uint16_t i = 0; i < num_Samples; i++)
    {
        MPU6050_Read_Accelerometer_Data(hi2c, i2c_Dev_Addr, &temp_data);
        sum_x += temp_data.x;
        sum_y += temp_data.y;
        sum_z += temp_data.z;
        HAL_Delay(3);
    }

    error_Offset->x = (int16_t)((sum_x / num_Samples) - 0);
	error_Offset->y = (int16_t)((sum_y / num_Samples) - 0);
	error_Offset->z = (int16_t)((sum_z / num_Samples) - 16384);

    printf("Kalibrasyon Bitti!\r\n");
    printf("Offset X: %d | Y: %d | Z: %d\r\n", error_Offset->x, error_Offset->y, error_Offset->z);
}





