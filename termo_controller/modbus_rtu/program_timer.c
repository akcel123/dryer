#include "program_timer.h"

void ProgramTimerUpdate(ProgramTimer *p)
{
	if(p->state == kTimerOff) return;
	p->scale++;
	if((p->scale >= p->callback_scale) && (p->state == kTimerOn))
	{
		p->timer_callback(p->arg);
		p->scale = 0;
	}
}

CalcTimingState ProgramTimerInit(ProgramTimer* p, uint32_t main_freq, uint32_t callback_freq, TimerCallback timer_callback, void* arg)
{
	p->state = kTimerOff;
	p->arg = arg;
	p->timer_callback = timer_callback;
	p->callback_scale = main_freq / callback_freq;
	CalcTimingState state = p->callback_scale ? kSucsess : kTimingsError;
	return state;
}


CalcTimingState ProgramTimerInitFloat(ProgramTimer* p, uint32_t main_freq, float callback_freq, TimerCallback timer_callback, void* arg)
{
	p->state = kTimerOff;
	p->arg = arg;
	p->timer_callback = timer_callback;
	p->callback_scale = (float)main_freq / callback_freq;
	CalcTimingState state = p->callback_scale ? kSucsess : kTimingsError;
	return state;
}

 void ProgramTimerOn(ProgramTimer* p)
 {
	 p->state = kTimerOn;
 }


 void ProgramTimerOff(ProgramTimer* p)
 {
	 p->state = kTimerOff;
 }
