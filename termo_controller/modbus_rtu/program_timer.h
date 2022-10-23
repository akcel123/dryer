#ifndef __PROGRAM_TIMER_H__
#define __PROGRAM_TIMER_H__

#include <stdint.h>

typedef enum {
	kTimerOn = 0,
	kTimerOff
}ProgrammTimerState;

typedef enum {
	kSucsess = 0,
	kTimingsError			//возвращает данное значение при неправильно заданных параметрах, на основании которых невозможно реализовать программный таймер
} CalcTimingState;


typedef void (*TimerCallback)(void*);

typedef struct {
	uint32_t scale;						//счетчик программного таймера
	uint32_t callback_scale;			//значение, до которого необходимо вести счет, как досчитает, вызывается колбек
	ProgrammTimerState state; 			//состояние программного таймера

	TimerCallback timer_callback;		//колбек таймера (задается пользователем)
	void *arg;

} ProgramTimer;

//---------------------------------------------------------------------------------------------------
//	функция ProgramTimerInit()
//
//	Данная функция Инициализирует все необходимые задержки таймера
///		main_freq - частота основного колбека, в котором крутится функция ProgramTimerUpdate
///		callback_freq - Негобходимая частота программного таймера
/// 	timer_callback - ссылка на функцию-колбек, которая вызывается по достижению необходимой задержки
/// 	arg - аргумент функции timer_callback
//---------------------------------------------------------------------------------------------------
CalcTimingState ProgramTimerInit(ProgramTimer*, uint32_t main_freq, uint32_t callback_freq, TimerCallback timer_callback, void* arg);
CalcTimingState ProgramTimerInitFloat(ProgramTimer* p, uint32_t main_freq, float callback_freq, TimerCallback timer_callback, void* arg);

//---------------------------------------------------------------------------------------------------
//	функция ProgramTimerUpdate()
//
//	Данную функию необходимо помещать в основной колбек, она высчитывает необходимую задержку
//	и вызывает функцию колбека
//---------------------------------------------------------------------------------------------------
void ProgramTimerUpdate(ProgramTimer*);

//---------------------------------------------------------------------------------------------------
//	функция ProgramTimerOn()
//
//	Данная функция включает программный таймер
//---------------------------------------------------------------------------------------------------
void ProgramTimerOn(ProgramTimer*);

//---------------------------------------------------------------------------------------------------
//	функция ProgramTimerOff()
//
//	Данная функция выключает програмнный таймер
//---------------------------------------------------------------------------------------------------
void ProgramTimerOff(ProgramTimer*);




#endif
