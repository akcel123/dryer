#ifndef __GLOBAL_VAR_H__
#define __GLOBAL_VAR_H__
#include <stdbool.h>
#include <stdint.h>


typedef struct {
	struct{
		float kp;
		float ki;
		float kc;
		float kd;
		float min_out;
		float max_out;
		float out;
	} pid;
	struct{
		float temperature;
	} fb;
	uint32_t pwm_frequency;
	bool choice_of_duty_setter;
	float duty;
} pwmOut;



class globalVar {
public:
	float temperature;		//температура, которая поддерживается регулятором

	pwmOut regulator_pin;


	globalVar()
	{
		this->temperature = 0.;
		this->regulator_pin.pwm_frequency = 100;
		this->regulator_pin.choice_of_duty_setter = false;
		this->regulator_pin.duty = 0.;
		this->regulator_pin.fb.temperature = 0.;
		this->regulator_pin.pid.kc = 0.;
		this->regulator_pin.pid.kd = 0.;
		this->regulator_pin.pid.ki = 0.;
		this->regulator_pin.pid.kp = 0.;
		this->regulator_pin.pid.max_out = 0.;
		this->regulator_pin.pid.min_out = 0.;
		this->regulator_pin.pid.out = 0.;
	}
};




/* Functions -----------------------------------------------------------------*/











#endif
