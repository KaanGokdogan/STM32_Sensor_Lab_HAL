/*
 * mpu6050.h
 *
 *  Created on: 25 May 2026
 *      Author: kaan_
 */

#ifndef MPU6050_H_
#define MPU6050_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx.h"



typedef enum
{
	MPU6050_OK,
	MPU6050_ERR,
}mpu6050_status_t;


typedef struct
{
	int16_t x;
	int16_t y;
	int16_t z;
}mpu6050_accel_data_t;



/*
 * MPU6050 Register Addresses
 */
#define MPU6050_I2C_ADDR 				((uint8_t) 0x68)
#define MPU6050_REG_WHOAMI 				(uint8_t) 117
#define MPU6050_REG_PWMGMT_1 			(uint8_t) 0x6B
#define MPU6050_REG_ACCEL_START 		(uint8_t) 59




/*
 * Functions
 */
mpu6050_status_t MPU6050_Init(I2C_HandleTypeDef *hi2c, uint8_t i2c_Dev_Addr);
mpu6050_status_t MPU6050_Read_Accelerometer_Data(I2C_HandleTypeDef *hi2c, uint8_t i2c_Dev_Addr, mpu6050_accel_data_t *accel_Data);






#endif /* MPU6050_H_ */
