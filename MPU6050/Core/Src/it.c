/*
 * it.c
 *
 *  Created on: 25 May 2026
 *      Author: kaan_
 */

#include "it.h"


extern TIM_HandleTypeDef htim4;


/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  HAL_IncTick();

}


void TIM4_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim4);
}
