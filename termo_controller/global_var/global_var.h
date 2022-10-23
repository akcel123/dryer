#ifndef __GLOBAL_VAR_H__
#define __GLOBAL_VAR_H__
#include <stdbool.h>
#include <stdint.h>

typedef enum
{
	kNumOutputs = 7,
	kNumInputs =  8,
	kNumGlobalRegistersForModbus = 180,		//количество СЛОВ глобальных переменных
}deviceParameters;

typedef enum{
   kModbusRead = 0x1,
   kModbusWrite  = 0x2
}modbusAccess;


typedef enum {
	kOpenSystemVar = 0x44454E00,
	kSaveAllInEEprom = 0x53415645,
	kSaveOnChange = 0x53414D4F,
	kEraceEeprom = 0x11111111
} Comands;

typedef enum {
	kEepromSavingNotUse,
	kSavingAllVar,
	kVarsSaved,
	kSavingOnChange,
	kEracingEeprom

} eepromState;


typedef struct{

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
	} value;
	uint32_t max_value;
	uint32_t min_value;
	modbusAccess modbus_access;
}var;

typedef struct {
	var baudrate;
	var id;
	var pairy;
} modbusPar;

typedef struct {
	struct{
		var kp;
		var ki;
		var kc;
		var kd;
		var min_out;
		var max_out;
		var out;
	} pid;

	struct{
		var temperature;

	} fb;
	var pwm_frequency;
	var choice_of_duty_setter;
	var duty;
} output5;

typedef struct {
	var gain;
	var offset;
	var filt_tf;
	var adc_value;
	var phys_value;
	var inter_value;
	var inter_table_1;
	var inter_table_2;
	var inter_table_3;
	var inter_table_4;
	var inter_table_5;
	var inter_table_6;
	var inter_table_7;
	var inter_table_8;
	var inter_table_9;
	var inter_table_10;

} analogIn;

typedef struct {
	var cmd;							//	В данную переменную записываются команды
	var analog_in_1_value;			//	преобразованное значение (не знаю еще во что) аналогового входа
	var analog_in_2_value;			//	преобразованное значение (не знаю еще во что) аналогового входа
	var discrete_in_state;			//	данная переменная ххранит состояние дискретных входов (бит 0 - вход 0  и т.д.)
	var disctere_out_state;			//	данная переменная управляет состояниями дискретных выходов.
	var output_5_current_reff;		//	данная переменная хранит управляющее воздействие канала 5 (18 кГц)
	var relay_load;					//	данная переменная отвечает за контроль нагрузки на выходе (бит0 - выход 1, бит1 - выход 2 и т.д.)
	var devices_over_current;			//	данная переменная хранит дискретное значение наличия превышения по току (бит 0 выход 1, бит1- Выход 2, … , бит 5 - выходы 6,7; бит 6 - потенциометры)
	var current_supply_voltage;		//текущий заряд аккумулятора в абсолютных вольтах в формате Q24
} userVar;


typedef struct {
	var tf;
	var duty;
	var voltage;
	var pull_in_voltage;
	var holding_voltage;
	var pull_in_time;

}oneSlowOut;
typedef struct {
	var frequency;
	var max_current_supply_voltage;
	var min_current_supply_voltage;
	var max_output_voltage;
	var min_output_voltage;
	oneSlowOut output_1;
	oneSlowOut output_2;
	oneSlowOut output_3;
	oneSlowOut output_4;
} slowOutputs;

typedef struct {
	modbusPar modbus_parameters;//	параметры модбаса (см тип)
	output5 output_5;			//	выход 5 (управление по току)
	struct{
		analogIn analog_in_1;
		analogIn analog_in_2;
	}analog_in;//	аналоговые входа (потенциометры)
	slowOutputs slow_outputs;
	var choise_supply_voltage;
	var supply_voltage;
} systemVar;

typedef struct {
	var temperature;		//температура, которая поддерживается регулятором

	output5 regulator_pin;
} globalVar;


#define PAGE_DATA_OFFSET        8
#define PAGE_DATA_SIZE          8




#define PAGE_0_ADDRESS        	0x0800F800
#define PAGE_1_ADDRESS     		0x0800FC00
#define PAGE_SIZE              	4096



typedef enum {
  PAGE_CLEARED = 0xFFFFFFFF,
  PAGE_ACTIVE = 0x00000000,
  PAGE_RECEIVING_DATA = 0x55555555,
} PageState;

typedef enum {
  PAGE_0 = 0,
  PAGE_1 = 1,
  PAGES_NUM = 2,
} PageIdx;

typedef enum {
  EEPROM_OK = 0,
  EEPROM_ERROR = 1,
} EepromResult;



/* Functions -----------------------------------------------------------------*/
//---------------------------------------------------------------------------------------------------
//	функция globalVarInit()
//
//	Данная функция инициализирует все глобальные переменные нулями, все флаги сброшены,
//	режим настроен в соответствии с требованиями (устанавливается в данной функции и не меняется)
//---------------------------------------------------------------------------------------------------
void globalVarInit(globalVar* , uint32_t callback_freq);
//---------------------------------------------------------------------------------------------------
//	функция globalVarWriteInFlash()
//
//	Данная функция записывает глобальные переменные во флеш память
//---------------------------------------------------------------------------------------------------
void globalVarWriteInFlash(globalVar*);
//---------------------------------------------------------------------------------------------------
//	функция globalVarUpdate()
//
//	Данная функция производит обработку команд и записывает в EEPROM необходимые значения
//---------------------------------------------------------------------------------------------------
void globalVarUpdate(globalVar*);











#endif
