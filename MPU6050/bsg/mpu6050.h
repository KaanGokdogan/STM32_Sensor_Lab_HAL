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


typedef struct
{
	int16_t x;
	int16_t y;
	int16_t z;
}mpu6050_accel_offset_t;


typedef enum
{
	DLPF_CFG_260HZ = 0,
	DLPF_CFG_184HZ = 1,
	DLPF_CFG_94HZ =  2,
	DLPF_CFG_44HZ =  3,
	DLPF_CFG_21HZ =  4,
	DLPF_CFG_10HZ =  5,
	DLPF_CFG_5HZ =   6,
}mpu6050_dlpf_config_t;


typedef enum
{
	FS_SEL_250_DEGREES =  0,
	FS_SEL_500_DEGREES =  1,
	FS_SEL_1000_DEGREES = 2,
	FS_SEL_2000_DEGREES = 3,
}mpu6050_gyro_range_t;


typedef enum
{
	AFS_SEL_2G =  0,
	AFS_SEL_4G =  1,
	AFS_SEL_8G =  2,
	AFS_SEL_16G = 3,
}mpu6050_accel_range_t;


/*
 * MPU6050 Register Addresses
 */
#define MPU6050_I2C_ADDR 				((uint8_t) 0x68U)
#define MPU6050_REG_WHOAMI 				((uint8_t) 0x75U)
#define MPU6050_REG_PWMGMT_1 			((uint8_t) 0x6BU)
#define MPU6050_REG_ACCEL_START 		((uint8_t) 0x3BU)
#define MPU6050_REG_CONFIG     		 	((uint8_t) 0x1AU)
#define MPU6050_REG_GYRO_CONFIG    	 	((uint8_t) 0x1BU)
#define MPU6050_REG_ACCEL_CONFIG     	((uint8_t) 0x1CU)




/*
 * Functions
 */
mpu6050_status_t MPU6050_Init(I2C_HandleTypeDef *hi2c, uint8_t i2c_Dev_Addr);
mpu6050_status_t MPU6050_Read_Accelerometer_Data(I2C_HandleTypeDef *hi2c, uint8_t i2c_Dev_Addr, mpu6050_accel_data_t *accel_Data);
mpu6050_accel_data_t MPU6050_Accelerometer_Calibration_Data(mpu6050_accel_offset_t *error_Offset, mpu6050_accel_data_t *raw_Data);
void MPU6050_Calibrate(I2C_HandleTypeDef *hi2c, uint8_t i2c_Dev_Addr, mpu6050_accel_offset_t *error_Offset, uint16_t num_Samples);
mpu6050_status_t MPU6050_Configure_Low_Pass_Filter(I2C_HandleTypeDef *hi2c, mpu6050_dlpf_config_t dlpf);
mpu6050_status_t MPU6050_Configure_Gyro_Range(I2C_HandleTypeDef *hi2c, mpu6050_gyro_range_t gr);
mpu6050_status_t MPU6050_Configure_Accel_Range(I2C_HandleTypeDef *hi2c, mpu6050_accel_range_t ar);





#endif /* MPU6050_H_ */
