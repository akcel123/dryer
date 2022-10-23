/*
 * ds18b20.h
 *
 *  Created on: 11 сент. 2022 г.
 *      Author: akcel
 */

#ifndef DS18B20_DS18B20_H_
#define DS18B20_DS18B20_H_

#include "main.h"

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
//--------------------------------------------------
enum class skipRom {
	kNotSkip = 0,
	kSkip = 1
};

//--------------------------------------------------
enum resolution{
	k9Bit = 0x1F,
	k10Bit = 0x3F,
	k11Bit = 0x5F,
	k12Bit = 0x7F
};

//--------------------------------------------------

#define SET_PIN 		(GPIOA->ODR |= GPIO_ODR_4)
#define RESET_PIN 		(GPIOA->ODR &= ~GPIO_ODR_4)
#define READ_BIT_DS		(GPIOA->IDR & GPIO_IDR_4)
class ds18b20 {
public:

	uint8_t Reset(void);
	uint8_t ReadBit(void);
	uint8_t ReadByte(void);
	void WriteBit(uint8_t bit);
	void WriteByte(uint8_t dt);
	uint8_t init(skipRom mode);
	void MeasureTemperCmd(skipRom mode, uint8_t DevNum);
	void ReadStratcpad(skipRom mode, uint8_t *Data, uint8_t DevNum);
	uint8_t GetSign(uint16_t dt);

	float Convert(uint16_t dt);


	ds18b20();
	void update();

};

#endif /* DS18B20_DS18B20_H_ */
