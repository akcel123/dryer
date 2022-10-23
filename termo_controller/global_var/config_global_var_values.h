/*
 * config_global_var_values.h
 *
 * Created on: 6.8.2022
 * 	 Author: Denis Pavlov
 */
#ifndef GLOBAL_VAR_CONFIG_GLOBAL_VAR_VALUES_H_
#define GLOBAL_VAR_CONFIG_GLOBAL_VAR_VALUES_H_
#include <stdbool.h>
#include <stdint.h>
#include "main.h"
#include "global_var.h"

#define NUM_OF_GLOBAL_VAR 	 12

typedef union{
	struct {
		uint32_t c_temperature;
		uint32_t c_regulator_pin_pid_kp;
		uint32_t c_regulator_pin_pid_ki;
		uint32_t c_regulator_pin_pid_kc;
		uint32_t c_regulator_pin_pid_kd;
		uint32_t c_regulator_pin_pid_min_out;
		uint32_t c_regulator_pin_pid_max_out;
		uint32_t a_regulator_pin_pid_out;
		uint32_t a_regulator_pin_fb_temperature;
		uint32_t c_regulator_pin_pwm_frequency;
		uint32_t c_regulator_pin_choice_of_duty_setter;
		uint32_t c_regulator_pin_duty;
	} var;
	uint32_t arr[NUM_OF_GLOBAL_VAR];
} globalVarValues;

typedef union{
	struct {
		modbusAccess c_temperature;
		modbusAccess c_regulator_pin_pid_kp;
		modbusAccess c_regulator_pin_pid_ki;
		modbusAccess c_regulator_pin_pid_kc;
		modbusAccess c_regulator_pin_pid_kd;
		modbusAccess c_regulator_pin_pid_min_out;
		modbusAccess c_regulator_pin_pid_max_out;
		modbusAccess a_regulator_pin_pid_out;
		modbusAccess a_regulator_pin_fb_temperature;
		modbusAccess c_regulator_pin_pwm_frequency;
		modbusAccess c_regulator_pin_choice_of_duty_setter;
		modbusAccess c_regulator_pin_duty;
	} var;
	modbusAccess arr[NUM_OF_GLOBAL_VAR];
} globalVarModbusAccess;

//� ������ ������������ ������������ ��� ����������, ��� ������� ��������� ������, ��������� ������� ���������� ����� ����������.
typedef enum {
	kTemperature = 0,
	kRegulatorPinPidKp = 1,
	kRegulatorPinPidKi = 2,
	kRegulatorPinPidKc = 3,
	kRegulatorPinPidKd = 4,
	kRegulatorPinPidMinOut = 5,
	kRegulatorPinPidMaxOut = 6,
	kRegulatorPinPwmFrequency = 7,

	kVarNum = 8
}VarName;

extern const globalVarValues global_var_default_value;
extern const globalVarValues global_var_max_value;
extern const globalVarValues global_var_min_value;
extern var* var_pointers[NUM_OF_GLOBAL_VAR];
extern uint16_t varIdList[kVarNum];
extern globalVar global_var;

uint32_t GetDefaultValue(uint16_t varId);

#endif /* GLOBAL_VAR_CONFIG_GLOBAL_VAR_VALUES_H_ */
