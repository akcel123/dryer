#ifndef __OUT_PINS_H__
#define __OUT_PINS_H__


#include "global_var.h"

#define UPDATE_OUT_PINS_FREQUENCY	1000.



typedef struct {
	IqPidStruct fast_out_pid_structure;
	uint16_t current_autoreload_fast;
	uint16_t prev_freq;
	union {
		uint32_t all;
		struct {
			uint32_t bit0:1;
			uint32_t bit1:1;
			uint32_t bit2:1;
			uint32_t bit3:1;
			uint32_t bit4:1;
			uint32_t bit5:1;
			uint32_t bit6:1;
			uint32_t bit7:1;
			uint32_t bit8:1;
			uint32_t bit9:1;
			uint32_t bit10:1;
			uint32_t bit11:1;
			uint32_t bit12:1;
			uint32_t bit13:1;
			uint32_t bit14:1;
			uint32_t bit15:1;
			uint32_t bit16:1;
			uint32_t bit17:1;
			uint32_t bit18:1;
			uint32_t bit19:1;
			uint32_t bit20:1;
			uint32_t bit21:1;
			uint32_t bit22:1;
			uint32_t bit23:1;
			uint32_t bit24:1;
			uint32_t bit25:1;
			uint32_t bit26:1;
			uint32_t bit27:1;
			uint32_t bit28:1;
			uint32_t bit29:1;
			uint32_t bit30:1;
			uint32_t bit31:1;
		} bit;
	} prev_dics_mask;					//	Включение/выключение реле




} OutPinsStruct;


//---------------------------------------------------------------------------------------------------
//	функция OutPinsInit()
//
//	Данная функция инициализирует необходимую периферию, связанную с выходами (например, таймеры)
//---------------------------------------------------------------------------------------------------
void outPinsInit(globalVar*, OutPinsStruct*);


//---------------------------------------------------------------------------------------------------
//	функция OutPinsUpdate()
//
//	Данная функция Обновляет значения глобальных переменных в соответствии с режимом.
//---------------------------------------------------------------------------------------------------
void outPinsUpdate(globalVar*, OutPinsStruct*);

void FastPinsUpdate(globalVar* gl_p, OutPinsStruct* out_p);



#endif
