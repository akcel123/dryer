/*
 * tm1637.h
 *
 *  Created on: 29 сент. 2019 г.
 *      Author: dima
 */
/*
 * ЭТОТ МОДУЛЬ НЕОБХОДИМО ПЕРЕДЕЛАТЬ СЛЕДУЮЩИМ ОБРАЗОМ:
 * СДЕЛАТЬ КЛАСС, КОТОРЫЙ НИЧЕГО НЕ БУДЕТ ПРИНИМАТЬ В КОНСТРУКТОР
 * СООТВЕТСТВЕННО СДЕЛАТЬ ВИРТУАЛЬНЫЕ ФУНКЦИИ, КОТОРЫЕ СЕЙЧАС ОПРЕДЕЛЕНЫ ДЕФАЙНАМИ
 * ТАКИМ ОБРАЗОМ ДЛЯ РАБОТЫ НЕОБХОДИМО УНАСЛЕДОВАТЬ ЭТОТ КЛАСС И ПЕРЕОПРЕДЕЛИТЬ ФУНКЦИИ ЗАПИСИ И ЧТЕНИЯ ПИНОВ
 * ПОДУМАТЬ ОБ НЕЙМИНГЕ ФУНКЦИЙ, ПЕРЕДЕЛАТЬ, СДЕЛАТЬ ФУНКЦИЮ update.
 */
#ifndef TM1637_H_
#define TM1637_H_

#include "stm32f0xx_hal.h"
#include "main.h"

#define DIO_Pin TM1637_DIO_Pin
#define DIO_GPIO_Port TM1637_DIO_GPIO_Port
#define CLK_Pin TM1637_CLK_Pin
#define CLK_GPIO_Port TM1637_CLK_GPIO_Port

#define CLK_HIGH	(HAL_GPIO_WritePin(CLK_GPIO_Port, CLK_Pin, GPIO_PIN_SET))
#define CLK_LOW		(HAL_GPIO_WritePin(CLK_GPIO_Port, CLK_Pin, GPIO_PIN_RESET))

#define DIO_HIGH	(HAL_GPIO_WritePin(DIO_GPIO_Port, DIO_Pin, GPIO_PIN_SET))
#define DIO_LOW		(HAL_GPIO_WritePin(DIO_GPIO_Port, DIO_Pin, GPIO_PIN_RESET))

#define DIO_READ	(HAL_GPIO_ReadPin(DIO_GPIO_Port, DIO_Pin))

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
