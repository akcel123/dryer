/*
 * tm1637.h
 *
 *  Created on: 29 сент. 2019 г.
 *      Author: dima
 */

#ifndef TM1637_H_
#define TM1637_H_

//#include "stm32f0xx_hal.h"
#include "main.h"

#define DIO_Pin LL_GPIO_PIN_1
#define DIO_GPIO_Port GPIOA
#define CLK_Pin LL_GPIO_PIN_2
#define CLK_GPIO_Port GPIOA

#define CLK_HIGH	(LL_GPIO_SetOutputPin(CLK_GPIO_Port, CLK_Pin))
#define CLK_LOW		(LL_GPIO_ResetOutputPin(CLK_GPIO_Port, CLK_Pin))

#define DIO_HIGH	(LL_GPIO_SetOutputPin(DIO_GPIO_Port, DIO_Pin))
#define DIO_LOW		(LL_GPIO_ResetOutputPin(DIO_GPIO_Port, DIO_Pin))

#define DIO_READ	(LL_GPIO_IsOutputPinSet(DIO_GPIO_Port, DIO_Pin))

#define ADDR_AUTO  0x40
#define ADDR_FIXED 0x44

#define STARTADDR  0xc0

void tm1637_init(int clk, int data);
void writeByte(int8_t wr_data); //write 8bit data to tm1637
void start(void); //send start bits
void stop(void);  //send stop bits
void display_mass(int8_t DispData[]);
void display(uint8_t BitAddr, int8_t DispData);
void clearDisplay(void);
void set_brightness(uint8_t brightness);  //To take effect the next time it displays.
void point(uint8_t cmd);  //whether to light the clock point ":".To take effect the next time it displays.
void coding_mass(int8_t DispData[]);
int8_t coding(int8_t DispData);

#endif /* TM1637_H_ */
