/*
 * regul_pin.cpp
 *
 *  Created on: Sep 3, 2022
 *      Author: akcel
 */

#include "regul_pin.h"
#include "main.h"

#define   _IQ24(A)      (long) ((A) * 16777216.0L)
#define __IQmpy(A, B, Q)         (long) (((long long) (A) * (long long) (B)) >> Q)

uint32_t regulPin::regulate(float ref, float fb)
{
	float result = 0.;
	long result_iq = 0;
	if(*(this->cur_freq) != this->prev_freq)
		this->pwmInit(this->timer_freq, *(this->cur_freq));

	result = this->calculatePid(false, ref, fb);
	result_iq = _IQ24(result);
	return __IQmpy(result_iq, (int)this->autoreload, 24);

}

uint32_t regulPin::regulate(float duty)
{
	long result_iq = 0;
	if(duty > 1.) duty = 1.;
	if(*(this->cur_freq) != this->prev_freq)
		this->pwmInit(this->timer_freq, *(this->cur_freq));


	result_iq = _IQ24(duty);
	return __IQmpy(result_iq, (int)this->autoreload, 24);

}

void regulPin::pwmInit(uint32_t timer_freq, uint32_t freq)
{
	this->prev_freq = freq;
	uint32_t Autoreload;
	uint16_t Prescaler;
	if(freq == 0)
	{
		return;
	}
	else
	{
		//Autoreload = _IQ1div(kTimerFreq, (gl_p->system_var.output_5.pwm_frequency.value.all << 1));
		Autoreload = timer_freq / freq;
		Prescaler = 1;

		while (Autoreload > 0xFFFF)
		{
			Autoreload >>= 1;
			timer_freq >>= 1;
			Prescaler <<= 1;
		}

		Autoreload = Autoreload - 1;
		Prescaler = Prescaler - 1;
	}


	this->autoreload = Autoreload;


	LL_TIM_InitTypeDef TIM_InitStruct = {0};
	LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};

	TIM_InitStruct.Prescaler = Prescaler;
	TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
	TIM_InitStruct.Autoreload = Autoreload;
	TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
	LL_TIM_Init(TIM14, &TIM_InitStruct);
	LL_TIM_DisableARRPreload(TIM14);
	LL_TIM_OC_EnablePreload(TIM14, LL_TIM_CHANNEL_CH1);
	TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_PWM1;
	TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
	TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
	TIM_OC_InitStruct.CompareValue = 0;
	TIM_OC_InitStruct.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
	LL_TIM_OC_Init(TIM14, LL_TIM_CHANNEL_CH1, &TIM_OC_InitStruct);
	LL_TIM_OC_DisableFast(TIM14, LL_TIM_CHANNEL_CH1);
	TIM14->CR1 |= TIM_CR1_CEN;
	TIM14->CCR1 = 0;


}
