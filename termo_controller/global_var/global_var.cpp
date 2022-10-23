#include "global_var.h"
#include "main.h"
#include "program_timer.h"
#include "config_global_var_values.h"

static void SetAllValue(globalVar* gl_p);


extern const uint8_t auchCRCHi[256];
extern const uint8_t auchCRCLo[256];



void globalVarInit(globalVar* gl_p, uint32_t callback_freq)
{
	SetAllValue(gl_p);
}





static void SetAllValue(globalVar* gl_p)
{


	//устанавливаем дефолтные значения
	for(int i = 0; i < NUM_OF_GLOBAL_VAR; i++)
		var_pointers[i]->value.all = global_var_default_value.arr[i];



}



/******************************************************************************/
