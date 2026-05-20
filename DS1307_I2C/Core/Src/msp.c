/*
 * msp.c
 *
 *  Created on: 4 May 2026
 *      Author: MONSTER
 */

#include "main.h"



/************************************************************************
 * @fn				- HAL_MspInit
 *
 * @brief			- Initializes the low-level system settings and
 * 					  exceptions of the ARM Cortex processor.
 *
 * @param[in]		- none
 *
 * @return			- none
 *
 * @Note			- Set to Priority Group 4 and basic fault exceptions
 * 					  are enabled.
 ************************************************************************/
void HAL_MspInit(void)
{
	// Here will do low level processor specific inits
	// 1. Set up the priority grouping of the ARM Cortex MX processor
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

	// 2. Enable the required system exceptions of the ARM Cortex MX processor
	SCB->SHCSR |= 0x7 << 16;		// Usage fault, memory fault and bus fault system exceptions

	// 3. Configure the priority for the system exceptions
	HAL_NVIC_SetPriority(MemoryManagement_IRQn,0,0);
	HAL_NVIC_SetPriority(BusFault_IRQn,0,0);
	HAL_NVIC_SetPriority(UsageFault_IRQn,0,0);
}


/************************************************************************
 * @fn				- HAL_I2C_MspInit
 *
 * @brief			- Initializes the low-level hardware clock for the I2C peripheral.
 *
 * @param[in]		- hi2c: Pointer to an I2C_HandleTypeDef structure that contains
 * 					  the configuration information for the specified I2C.
 *
 * @return			- none
 *
 * @Note			- Only enables the I2C1 peripheral clock. GPIO pin configurations
 * 					  are handled separately inside the driver's private helper function.
 ************************************************************************/
void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
{
	// 1. Enable the clock for the I2C1 peripheral
    __HAL_RCC_I2C1_CLK_ENABLE();
}


/************************************************************************
 * @fn				- HAL_TIM_Base_MspInit
 *
 * @brief			- Enables the clock signal of the timer peripheral
 * 					  and configures the NVIC interrupt priorities.
 *
 * @param[in]		- htimer: Handle structure of the Timer unit to be configured.
 *
 * @return			- none
 *
 * @Note			- Low priority (15,0) NVIC interrupt is enabled for
 * 					  the TIM6 unit.
 ************************************************************************/
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htimer)
{
	// 1. Enable the clock for the TIM6 peripheral
	__HAL_RCC_TIM6_CLK_ENABLE();

	// 2. Enable the IRQ of TIM6
	HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);

	// 3. Configure the priority for the TIM6_DAC_IRQ
	HAL_NVIC_SetPriority(TIM6_DAC_IRQn,15,0);
}






























