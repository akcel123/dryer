#pragma once
/*
 * delay_micros.h
 *
 *  Created on: 29 ����. 2019 �.
 *      Author: dima
 */

#ifndef DELAY_MICROS_H_
#define DELAY_MICROS_H_

//#include "main.h"
#include "stm32f0xx_hal.h"
	
void DWT_Init(void);
void delay_micros(uint32_t us);

#endif /* DELAY_MICROS_H_ */
