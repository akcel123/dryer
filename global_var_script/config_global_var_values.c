/*
 * config_global_var_values.h
 *
 * Created on: 6.8.2022
 * 	 Author: Denis Pavlov
 */
#include "config_global_var_values.h"

uint16_t varIdList[kVarNum];

//данный экземпляр структуры определяет значения по умолчанию всех глобальных переменных.
const globalVarValues global_var_default_value = {
	.var.c_temperature = 0,
	.var.c_regulator_pin_pid_kp = 0,
	.var.c_regulator_pin_pid_ki = 0,
	.var.c_regulator_pin_pid_kc = 0,
	.var.c_regulator_pin_pid_kd = 0,
	.var.c_regulator_pin_pid_min_out = 0,
	.var.c_regulator_pin_pid_max_out = 0,
	.var.a_regulator_pin_pid_out = 0,
	.var.a_regulator_pin_fb_temperature = 0,
	.var.c_regulator_pin_pwm_frequency = 18000,
	.var.c_regulator_pin_choice_of_duty_setter = 0,
	.var.c_regulator_pin_duty = 0,
};

//данный экземпляр структуры определяет максимальное значение всех глобальных переменных.
const globalVarValues global_var_max_value = {
	.var.c_temperature = 2147483647,
	.var.c_regulator_pin_pid_kp = 2147483659,
	.var.c_regulator_pin_pid_ki = 2147483647,
	.var.c_regulator_pin_pid_kc = 2147483647,
	.var.c_regulator_pin_pid_kd = 2147483647,
	.var.c_regulator_pin_pid_min_out = 2147483647,
	.var.c_regulator_pin_pid_max_out = 2147483659,
	.var.a_regulator_pin_pid_out = 2147483647,
	.var.a_regulator_pin_fb_temperature = 2147483647,
	.var.c_regulator_pin_pwm_frequency = 20000,
	.var.c_regulator_pin_choice_of_duty_setter = 1,
	.var.c_regulator_pin_duty = 2147483647,
};

//данный экземпляр структуры определяет минимальное значение всех глобальных переменных.
const globalVarValues global_var_min_value = {
	.var.c_temperature = 0,
	.var.c_regulator_pin_pid_kp = 0,
	.var.c_regulator_pin_pid_ki = 0,
	.var.c_regulator_pin_pid_kc = 0,
	.var.c_regulator_pin_pid_kd = 0,
	.var.c_regulator_pin_pid_min_out = 0,
	.var.c_regulator_pin_pid_max_out = 0,
	.var.a_regulator_pin_pid_out = 0,
	.var.a_regulator_pin_fb_temperature = 0,
	.var.c_regulator_pin_pwm_frequency = 0,
	.var.c_regulator_pin_choice_of_duty_setter = 0,
	.var.c_regulator_pin_duty = 0,
};

//данный экземпляр структуры определяет доступ к всем глобальным переменным.
const globalVarModbusAccess global_var_modbus_access = {
	.var.c_temperature = kModbusWrite,
	.var.c_regulator_pin_pid_kp = kModbusWrite,
	.var.c_regulator_pin_pid_ki = kModbusWrite,
	.var.c_regulator_pin_pid_kc = kModbusWrite,
	.var.c_regulator_pin_pid_kd = kModbusWrite,
	.var.c_regulator_pin_pid_min_out = kModbusWrite,
	.var.c_regulator_pin_pid_max_out = kModbusWrite,
	.var.a_regulator_pin_pid_out = kModbusRead,
	.var.a_regulator_pin_fb_temperature = kModbusRead,
	.var.c_regulator_pin_pwm_frequency = kModbusWrite,
	.var.c_regulator_pin_choice_of_duty_setter = kModbusWrite,
	.var.c_regulator_pin_duty = kModbusWrite,
};

//данный массив сформирован из ссылок на значения глобальных переменных, которые записывются в ПЗУ (в соответствие с перечислением)
uint32_t* cur_var_value[kVarNum] = {
	&global_var.temperature.value.all,
	&global_var.regulator_pin.pid.kp.value.all,
	&global_var.regulator_pin.pid.ki.value.all,
	&global_var.regulator_pin.pid.kc.value.all,
	&global_var.regulator_pin.pid.kd.value.all,
	&global_var.regulator_pin.pid.min_out.value.all,
	&global_var.regulator_pin.pid.max_out.value.all,
	&global_var.regulator_pin.pwm_frequency.value.all,
};

//данный массив содержит ссылки на ВСЕ глобальные переменные.
//В соответствие с этим массивом раздаются адреса пееменным по порядку с увеличением адреса с увеличением элемента массива
var* var_pointers[NUM_OF_GLOBAL_VAR] = {
	&global_var.temperature,
	&global_var.regulator_pin.pid.kp,
	&global_var.regulator_pin.pid.ki,
	&global_var.regulator_pin.pid.kc,
	&global_var.regulator_pin.pid.kd,
	&global_var.regulator_pin.pid.min_out,
	&global_var.regulator_pin.pid.max_out,
	&global_var.regulator_pin.pid.out,
	&global_var.regulator_pin.fb.temperature,
	&global_var.regulator_pin.pwm_frequency,
	&global_var.regulator_pin.choice_of_duty_setter,
	&global_var.regulator_pin.duty,
};

//данная функция возвращает значение по умолчанию для переменных, записываемых в ПЗУ
uint32_t GetDefaultValue(uint16_t varId)
{

	uint32_t res = 0;

	if (varId == varIdList[kTemperature])	{
		res = global_var_default_value.var.c_temperature;
	}
	else if (varId == varIdList[kRegulatorPinPidKp])	{
		res = global_var_default_value.var.c_regulator_pin_pid_kp;
	}
	else if (varId == varIdList[kRegulatorPinPidKi])	{
		res = global_var_default_value.var.c_regulator_pin_pid_ki;
	}
	else if (varId == varIdList[kRegulatorPinPidKc])	{
		res = global_var_default_value.var.c_regulator_pin_pid_kc;
	}
	else if (varId == varIdList[kRegulatorPinPidKd])	{
		res = global_var_default_value.var.c_regulator_pin_pid_kd;
	}
	else if (varId == varIdList[kRegulatorPinPidMinOut])	{
		res = global_var_default_value.var.c_regulator_pin_pid_min_out;
	}
	else if (varId == varIdList[kRegulatorPinPidMaxOut])	{
		res = global_var_default_value.var.c_regulator_pin_pid_max_out;
	}
	else if (varId == varIdList[kRegulatorPinPwmFrequency])	{
		res = global_var_default_value.var.c_regulator_pin_pwm_frequency;
	}
	return res;
}
