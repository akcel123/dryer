/*
 * delay_micros.c
 *
 *  Created on: 29 ����. 2019 �.
 *      Author: dima
 */
#include "delay_micros.h"

#define DWT_CONTROL *(volatile unsigned long *)0xE0001000
#define SCB_DEMCR   *(volatile unsigned long *)0xE000EDFC



void delay_micros(__IO uint32_t micros)
{
	micros *= (SystemCoreClock / 1000000) / 9;
	/* Wait till done */
	while (micros--) ;
}

