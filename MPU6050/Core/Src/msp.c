/*
 * msp.c
 *
 *  Created on: 25 May 2026
 *      Author: kaan_
 */
#include "main.h"

void HAL_MspInit(void)
{
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_0);
}



void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
{
  GPIO_InitTypeDef i2c_GPIO = {0};

    /* USER CODE BEGIN I2C1_MspInit 0 */

    /* USER CODE END I2C1_MspInit 0 */

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**I2C1 GPIO Configuration
    PB6     ------> I2C1_SCL
    PB9     ------> I2C1_SDA
    */
    i2c_GPIO.Pin = Audio_SCL_Pin|Audio_SDA_Pin;
    i2c_GPIO.Mode = GPIO_MODE_AF_OD;
    i2c_GPIO.Pull = GPIO_NOPULL;
    i2c_GPIO.Speed = GPIO_SPEED_FREQ_LOW;
    i2c_GPIO.Alternate = GPIO_AF4_I2C1;

    HAL_GPIO_Init(GPIOB, &i2c_GPIO);

    /* Peripheral clock enable */
    __HAL_RCC_I2C1_CLK_ENABLE();

 }



void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
    __HAL_RCC_TIM6_CLK_ENABLE();
}


