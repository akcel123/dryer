/*
 * button.h
 *
 *  Created on: Aug 2, 2022
 *      Author: Denis
 */

#ifndef PERIPH_BUTTON_H_
#define PERIPH_BUTTON_H_

#include <stdio.h>

class button {

	bool _pres_px;
	bool _prev_px;
	uint8_t _cnt_px;
	uint32_t _GPIOx_IDR;
	uint32_t _GPIO_IDR_IDx;
public:
	button(uint32_t GPIOx_IDR, uint32_t GPIO_IDR_IDx)
	{
		this->_GPIO_IDR_IDx = GPIO_IDR_IDx;
		this->_GPIOx_IDR = GPIOx_IDR;

	}
	void update();

	bool isPressed() {
		if(this->_pres_px)
		{
			this->_pres_px = false;
			return true;
		}

		return this->_pres_px;
	}

};


#endif /* PERIPH_BUTTON_H_ */
