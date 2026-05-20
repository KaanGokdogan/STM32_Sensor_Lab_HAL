/*
 * it.c
 *
 *  Created on: 4 May 2026
 *      Author: MONSTER
 */

#include "it.h"


extern TIM_HandleTypeDef htimer6;



/************************************************************************
 * @fn				- SysTick_Handler
 *
 * @brief			- Handles the default system tick (systick) interrupt
 * 					  in ARM Cortex processors.
 *
 * @param[in]		- none
 *
 * @return			- none
 *
 * @Note			- Mandatory for the HAL library's delay and time
 * 					  calculations.
 ************************************************************************/
void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}


/************************************************************************
 * @fn				- TIM6_DAC_IRQHandler
 *
 * @brief			- Captures interrupt requests from the Timer 6 hardware
 * 					  and routes them to the relevant handler function.
 *
 * @param[in]		- none
 *
 * @return			- none
 *
 * @Note			- Passes the execution to the HAL_TIM_IRQHandler function.
 ************************************************************************/
void TIM6_DAC_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&htimer6);
}

