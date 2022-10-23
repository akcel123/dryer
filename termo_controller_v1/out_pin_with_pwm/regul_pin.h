/*
 * regul_pin.h
 *
 *  Created on: Sep 3, 2022
 *      Author: akcel
 */

#ifndef REGUL_PIN_H_
#define REGUL_PIN_H_
#include <stdint.h>

#include "pid.h"
class regulPin : pid
{

	uint32_t prev_freq;
	uint32_t* cur_freq;
	uint32_t autoreload;
	uint32_t timer_freq;
	void pwmInit(uint32_t timer_freq, uint32_t freq);
public:
	void init() {this->pwmInit(this->timer_freq, *(this->cur_freq));}
	regulPin(uint32_t timer_freq, uint32_t* cur_freq)
	{
		this->timer_freq = timer_freq;
		this->cur_freq = cur_freq;
	}
	void setCurFreqPointer(uint32_t* fr_p) {this->cur_freq = fr_p;}

	uint32_t regulate(float ref, float fb);
	uint32_t regulate(float duty);

};

#endif /* REGUL_PIN_H_ */
