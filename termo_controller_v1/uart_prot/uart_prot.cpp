/*
 * uart_prot.cpp
 *
 *  Created on: Sep 3, 2022
 *      Author: akcel
 */

#include "uart_prot.h"
#include <stdlib.h>
#include <stdio.h>
uartProt::uartProt(globalVar* global_var_p, uint32_t timer_freq) {
	// TODO Auto-generated constructor stub
	this->global_var_p = global_var_p;
	this->rx_len = 0;
	this->tx_len = 0;
	this->res_ready = false;
	this->com_err = errors::kNoError;
	this->get_all_graphs_mode = false;
	timerInit(5, timer_freq);
	for(uint8_t i = 0; i <kSizeOfBuffs; i++)
	{
		this->rx_buf[i] = 0;
		this->tx_buf[i] = 0;
	}
}

uint32_t pow(uint32_t a, uint8_t s)
{
	uint32_t result = a;
	if(s == 0) return 1;
	if(s == 1) return a;
	for(int i = 1; i < s; i++) result*= a;
	return result;
}



float uartProt::stringToFloat(uint8_t start, uint8_t end)
{
	uint32_t int_part = 0.;
	uint32_t fract_part = 0.;
	//найдем точку
	uint8_t point_pos = 0;
	uint8_t int_part_len = 0;
	uint8_t fract_part_len = 0;
	for (int i = 0; i < end; i++)
		if (this->rx_buf[i] == '.')
			point_pos = i;
	//нет точки, значит отсутстует дробная часть (a3.01)
	int_part_len = point_pos - start;
	fract_part_len = end - point_pos - 1; //-1 для перого симола
	if (point_pos == 0) { fract_part = 0.; int_part_len = end - 1; }


	float temper = 0.;
	//10.01
	//отделение дробной и целой частей
	uint8_t k = 0;
	for (int i = start; i < int_part_len + start; i++)
	{
		int_part += (this->rx_buf[i] - '0') * pow(10, int_part_len - 1 - k++);
	}
	k = 0;
	if (point_pos >= 3)
		for (int i = point_pos + 1; i < end; i++)
		{
			fract_part += (this->rx_buf[i] - '0') * pow(10, fract_part_len - 1 - k++);
		}
	else
	{
		this->com_err = errors::kSyntacsisError;
		//ошибка BBода
	}


	temper = fract_part;
	for (int i = 0; i < fract_part_len; i++)temper /= 10;
	temper += int_part;

	return temper;
}
void uartProt::parseErrors()
{
//	if(this->com_err == errors::kSyntacsisError)
//	{
//		this->tx_buf[0] = 'e';
//		this->tx_buf[1] = 'r';
//		this->tx_buf[2] = '1';
//		this->tx_buf[3] = '\r';
//		//this->tx_buf[4] = '\n';
//		this->tx_len = 5;
//		this->res_ready = true;
//	}
}

void uartProt::okRes()
{
	this->tx_buf[0] = 'O';
	this->tx_buf[1] = 'K';
	this->tx_buf[2] = '\r';
	this->tx_buf[3] = '\n';
	this->tx_len = 4;
	this->res_ready = true;
}

void uartProt::parse()
{
	switch (this->rx_buf[0])
	{
	case commands::kSetRefTemp:
	{
		float temp_float_res = stringToFloat(1, this->rx_len);
		if(this->com_err == errors::kNoError)
		{
			this->okRes();

			this->global_var_p->temperature = temp_float_res;
		}
		else
		{
			this->parseErrors();
			//обработка ошибок
		}
		break;
	}
	case commands::kSetParam:
	{
		float temp_float_res;
		if(this->rx_buf[1] != parameters::kChoiceOfutySetter)
			temp_float_res = stringToFloat(2, this->rx_len);

		if (this->com_err == errors::kNoError)
		{
			this->okRes();



			switch (this->rx_buf[1])
			{
			case parameters::kChoiceOfutySetter:
				this->global_var_p->regulator_pin.choice_of_duty_setter = true;
				break;
			case parameters::kduty:
				this->global_var_p->regulator_pin.duty = temp_float_res;
				break;
			case parameters::kPidC:
				this->global_var_p->regulator_pin.pid.kc = temp_float_res;
				break;
			case parameters::kPidd:
				this->global_var_p->regulator_pin.pid.kd = temp_float_res;
				break;
			case parameters::kPidI:
				this->global_var_p->regulator_pin.pid.ki = temp_float_res;
				break;
			case parameters::kPidP:
				this->global_var_p->regulator_pin.pid.kp = temp_float_res;
				break;
			case parameters::kPidMaxOut:
				this->global_var_p->regulator_pin.pid.max_out = temp_float_res;
				break;
			case parameters::kPidMinOut:
				this->global_var_p->regulator_pin.pid.min_out = temp_float_res;
				break;
			case parameters::kPwmFreq:
				this->global_var_p->regulator_pin.pwm_frequency = int(temp_float_res);
				break;
			default:
				//ошибка отсутсти параметра
				break;
			}
		}
		else
		{
			//обработка ошибок
		}

		break;
	}
	case commands::kGetAllGraphs:
	{
		this->okRes();
		this->get_all_graphs_mode = !this->get_all_graphs_mode;
		break;
	}

	}




}

void uartProt::rxCallback(uint8_t new_rx_byte)
{
	this->rx_buf[this->rx_len++] = new_rx_byte;
	this->timerStart();
}
