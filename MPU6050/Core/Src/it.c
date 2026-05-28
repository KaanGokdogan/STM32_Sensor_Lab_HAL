/*
 * it.c
 *
 *  Created on: 25 May 2026
 *      Author: kaan_
 */

#include "it.h"
/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  HAL_IncTick();

}
