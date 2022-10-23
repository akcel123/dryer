/*
 * periph_functions.cpp
 *
 *  Created on: Aug 2, 2022
 *      Author: Denis
 */

#include "periph_functions.h"


void slowFlowInit()
{

	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);

	/*Configure GPIO pin : PA5 */
	GPIO_InitStruct.Pin = GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


	/* Clock Tim11 */
	RCC->APB2ENR |= RCC_APB2ENR_TIM16EN;
	/* T=1ms */
	TIM16->PSC = 4800 - 1;
	TIM16->ARR = 1000 - 1;
	/* Update Interrupt Enable */
	TIM16->DIER |= TIM_DIER_UIE;
	/* Counter enable */
	TIM16->CR1 |= TIM_CR1_CEN;
	/* Tim11 Interrupt enable */
	NVIC_EnableIRQ(TIM16_IRQn);
}


