#include "out_pins.h"
#include "main.h"
#include "program_timer.h"



extern TIM_HandleTypeDef htim14;

typedef enum{
	kSlow = 0,
	kFast = 1
} TypeOut;

bool timer_on[4];	//флаги работы/не работы таймера для задержки

const uint32_t kTimerFreq = 48000000;
const float kSampleTime = 1. / (float)UPDATE_OUT_PINS_FREQUENCY;


static void FastTim1Init(globalVar* gl_p, OutPinsStruct* out_p);


void outPinsInit(globalVar* gl_p, OutPinsStruct* out_p)
{
	FastTim1Init(gl_p,out_p);

	for(int i = 0; i < 4; i++)
	{


		timer_on[i] = false;
	}
	//float a = kSampleTime;


}

void outPinsUpdate(globalVar* gl_p, OutPinsStruct* out_p)
{
	FastPinsUpdate(gl_p, out_p);
}



void FastPinsUpdate(globalVar* gl_p, OutPinsStruct* out_p)
{
	if(out_p->prev_freq != gl_p->regulator_pin.pwm_frequency.value.all)
	{
		FastTim1Init(gl_p,out_p);
	}
	_iq pid_out = 0;
	out_p->fast_out_pid_structure.Ref = gl_p->temperature.value.all;
	out_p->fast_out_pid_structure.Fdb = gl_p->regulator_pin.fb.temperature.value.all;
	out_p->fast_out_pid_structure.Ki = gl_p->regulator_pin.pid.ki.value.all;
	out_p->fast_out_pid_structure.Kc = gl_p->regulator_pin.pid.kc.value.all;
	out_p->fast_out_pid_structure.Kd = gl_p->regulator_pin.pid.kd.value.all;
	out_p->fast_out_pid_structure.Kp = gl_p->regulator_pin.pid.kp.value.all;
	out_p->fast_out_pid_structure.Min = gl_p->regulator_pin.pid.min_out.value.all;
	out_p->fast_out_pid_structure.Max = gl_p->regulator_pin.pid.max_out.value.all;
	if(gl_p->regulator_pin.choice_of_duty_setter.value.all)
		pid_out = gl_p->regulator_pin.duty.value.all;
	else
	{
		pid_out = IqPid(&out_p->fast_out_pid_structure, true, Q24);
		gl_p->regulator_pin.pid.out.value.all = pid_out;
	}

	TIM14->CCR1 = __IQmpy(pid_out, (int)TIM14->ARR, Q24);
	//MDR_TIMER1->CCR1 = __IQmpy(pid_out, (int)MDR_TIMER1->ARR, Q24);

}



static void FastTim1Init(globalVar* gl_p, OutPinsStruct* out_p)
{
	out_p->prev_freq = gl_p->regulator_pin.pwm_frequency.value.all;
	uint32_t Autoreload;
	uint16_t Prescaler;
	uint32_t timer_freq = kTimerFreq;
	if(gl_p->regulator_pin.pwm_frequency.value.all == 0)
	{
		return;
	}
	else
	{

		Autoreload = _IQ1div(kTimerFreq, (gl_p->regulator_pin.pwm_frequency.value.all << 1));

		Prescaler = 1;

		while (Autoreload > 0xFFFF)
		{
			Autoreload >>= 1;
			timer_freq >>= 1;
			Prescaler <<= 1;
		}

		Autoreload = Autoreload - 1;
		Prescaler = Prescaler - 1;
	}

	out_p->current_autoreload_fast = Autoreload;

	//ТУТ НУЖНО ОБЯЗАТЕЛЬНО ИСПРАВИТЬ НА РЕЖИМ СЧЕТА ВВЕРХ И ВНИЗ
	//добавить инициализацию таймера
	htim14.Instance = TIM14;
	htim14.Init.Prescaler = Prescaler;
	htim14.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim14.Init.Period = Autoreload;
	htim14.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim14.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim14) != HAL_OK)
	{
		Error_Handler();
	}


}






