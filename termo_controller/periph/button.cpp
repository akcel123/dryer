/*
 * button.cpp
 *
 *  Created on: Aug 2, 2022
 *      Author: Denis
 */

#include "button.h"


void button::update()
{
	/* Read current state */
	bool cur_px = (~this->_GPIOx_IDR & this->_GPIO_IDR_IDx) != 0;
	/* If level has changed  */
	if (cur_px != this->_prev_px) {
		/* Increase counter */
		this->_cnt_px++;
		/* If consecutive 4*10 = 40ms approved */
		if (this->_cnt_px >= 4) {
			/* The button have not bounced for four checks, change state */
			this->_prev_px = cur_px;
			/* If the button was pressed (not released), tell main so */
			if (cur_px != false)
			{
				this->_pres_px = true;
			}
			this->_cnt_px = 0;
		}
	}
	else {
		/* Reset counter */
		this->_cnt_px = 0;
	}
}
