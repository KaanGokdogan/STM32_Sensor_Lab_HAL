/*
 * bmp280.h
 *
 *  Created on: 28 May 2026
 *      Author: kaan_
 */

#ifndef BMP280_H_
#define BMP280_H_

#include "main.h"



typedef enum
{
	BMP280_OK,
	BMP280_ERROR,
}bmp280_status_t;


typedef struct
{
	int32_t pressure;
	int32_t temperature;
}bmp280_sensor_data_t;
/*
 * BMP280 device addresses
 */
#define BMP280_I2C_ADDR				((uint8_t) 0x76U)

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
//bmp280_status_t BMP280_Sensor_Data(I2C_HandleTypeDef *hi2c, uint8_t i2c_Dev_Addr, bmp280_sensor_data_t *sensor_Data);

#endif /* BMP280_H_ */
