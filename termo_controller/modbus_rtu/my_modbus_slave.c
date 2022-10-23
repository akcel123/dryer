#include "my_modbus_slave.h"
#include "global_var.h"
#include "config_global_var_values.h"
extern globalVar global_var;
//extern var* var_pointers[NUM_OF_GLOBAL_VAR];



static MbState MB_InitStr(MbStr *m, MbMode role);
#ifndef USE_PROGRAMM_TIMER
static void MB_SetTimerFunctions(MbStr *m, timer_start timer_start, timer_stop timer_stop, timer_set_new_period timer_set_new_period, void* arg);
static MbState MB_CalculateTimingsForHardwareTimer(MbStr *m, uint8_t timer_frequency, uint32_t baudrate);
#else
static MbState MB_CalculateTimingsForSoftwareTimer(MbStr *m, uint16_t callback_frequency, uint16_t baudrate);
#endif

static void MB_SetInterfaceDataLinks(MbStr *m, uint8_t *rx_buf,uint8_t *rx_size, uint8_t *tx_buf, uint8_t *tx_size);
static void MB_SetInterfaceFunctions(MbStr *m, rx_callback_function rx_callback_function, send_bytes send_bytes, void* arg);
static void MB_SetServerId(MbStr *m, char sizeOfServerId, char *serverId);
static void MB_SetGlVarPointer(MbStr *, uint32_t **);



static void MB_15tCallback(MbStr *m);
static void MB_35tCallback(MbStr *m);
static void MB_ProgrammTimerUpdate(MbStr *m);
static void MB_SlaveUpdate(MbStr *m);
static void MB_Reset(MbStr* m);
static void MB_SlaveRxTrueCallback(MbStr *m);
static void MB_RxCorruptCallback(MbStr *m);
static void MB_ResetProgrammTimer(MbStr *m);
static uint16_t MB_CrcCalc(uint8_t BytesBeforeCrc, uint8_t *Buf4Check);
static uint8_t MB_CheckCrcRx(MbStr *m);



static void MB_SlaveResponse0x01(MbStr *m);
static void MB_SlaveResponse0x02(MbStr *m);
static void MB_SlaveResponse0x03(MbStr *m);
static void MB_SlaveResponse0x05(MbStr *m);
static void MB_SlaveResponse0x0F(MbStr *m);
static void MB_SlaveResponse0x10(MbStr *m);
static void MB_SlaveResponse0x11(MbStr *m);
static void MB_SlaveError(MbStr *m);
static void MB_SlaveResponce(MbStr *m);
static MbError MB_SlaveCheckData(MbStr *m);


static void MB_MasterUpdate(MbStr *m);
static void MB_RxMasterCallback(MbStr *m);
static void MB_MasterSendRequest0x01(MbStr *m);
static void MB_MasterSendRequest0x02(MbStr *m);
static void MB_MasterSendRequest0x03(MbStr *m);
static void MB_MasterSendRequest0x05(MbStr *m);
static void MB_MasterSendRequest0x0F(MbStr *m);
static void MB_MasterSendRequest0x10(MbStr *m);
static void MB_MasterSendRequest0x11(MbStr *m);

static void MB_MasterCheckResponce0x01(MbStr *m);
static void MB_MasterCheckResponce0x02(MbStr *m);
static void MB_MasterCheckResponce0x03(MbStr *m);
static void MB_MasterCheckResponce0x05(MbStr *m);
static void MB_MasterCheckResponce0x0F(MbStr *m);
static void MB_MasterCheckResponce0x10(MbStr *m);
static void MB_MasterCheckResponce0x11(MbStr *m);



const MbFunc commands_responses[kAllFunctions] = {
		&MB_SlaveResponse0x01,
		&MB_SlaveResponse0x02,
		&MB_SlaveResponse0x03,
		&MB_SlaveResponse0x05,
		&MB_SlaveResponse0x0F,
		&MB_SlaveResponse0x10,
		&MB_SlaveResponse0x11
};

const MbFunc commands_requests[kAllFunctions] = {
		&MB_MasterSendRequest0x01,
		&MB_MasterSendRequest0x02,
		&MB_MasterSendRequest0x03,
		&MB_MasterSendRequest0x05,
		&MB_MasterSendRequest0x0F,
		&MB_MasterSendRequest0x10,
		&MB_MasterSendRequest0x11
};


const MbFunc commands_responces_master[kAllFunctions] = {
		&MB_MasterCheckResponce0x01,
		&MB_MasterCheckResponce0x02,
		&MB_MasterCheckResponce0x03,
		&MB_MasterCheckResponce0x05,
		&MB_MasterCheckResponce0x0F,
		&MB_MasterCheckResponce0x10,
		&MB_MasterCheckResponce0x11
};

const uint8_t kFunctionCode[kAllFunctions] = {
		0x01,
		0x02,
		0x03,
		0x05,
		0x0F,
		0x10,
		0x11

};


#define CRC16_SIZE									2

const uint8_t auchCRCHi[256]=
{
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
	0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
	0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40
};

const uint8_t auchCRCLo[256]=
{
	0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
	0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
	0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
	0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
	0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
	0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
	0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
	0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
	0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
	0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
	0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
	0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
	0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
	0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
	0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
	0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
	0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
	0x40
};


MbState MB_Init(MbStr *m, mbInitStruct *mb_init_p)
{
	MbState error = kNotInit;
	MB_SetInterfaceDataLinks(m,mb_init_p->interface_data_links.rx_buf,mb_init_p->interface_data_links.rx_size, mb_init_p->interface_data_links.tx_buf,mb_init_p->interface_data_links.tx_size);
	MB_SetInterfaceFunctions(m,mb_init_p->interface_functions.rx_callback_function, mb_init_p->interface_functions.send_bytes, mb_init_p->interface_functions.arg);
	MB_SetServerId(m, mb_init_p->server_id.size_of_server_id, mb_init_p->server_id.server_id);
	MB_SetGlVarPointer(m,mb_init_p->gl_var_pointer);

#ifndef USE_PROGRAMM_TIMER
	MB_SetTimerFunctions(m, mb_init_p->hardware_timer.functions.timer_start, mb_init_p->hardware_timer.functions.timer_stop, mb_init_p->hardware_timer.functions.timer_set_new_period, mb_init_p->hardware_timer.functions.arg);
	error = MB_CalculateTimingsForHardwareTimer(m, mb_init_p->hardware_timer.frequency, mb_init_p->baudrate);
	m->timer_frequency = mb_init_p->hardware_timer.frequency;
#else
	error = MB_CalculateTimingsForSoftwareTimer(m, mb_init_p->callback_frequency, *(mb_init_p->baudrate));

#endif
	m->connection_parameters.current_baudrate = mb_init_p->baudrate;
	m->connection_parameters.old_baudrate = *(mb_init_p->baudrate);
	m->callback_frequency = mb_init_p->callback_frequency;
	m->connection_parameters.modbus_id = mb_init_p->modbus_id;

	error = MB_InitStr(m, mb_init_p->role);
	m->state = error;
	return error;

}



static MbState MB_InitStr(MbStr *m, MbMode role)
{

	MbState error = kZeroState;

	m->mb_params.addr = 0;
	m->mb_params.command = 0;
	m->mb_params.id = 0;
	m->mb_params.numBytes = 0;
	m->mb_params.numReg = 0;
	m->mb_params.len = 0;
	m->mb_params.val = 0;

	*(m->counters.corrupted_packages) = 0;
	*(m->counters.overflow_packages) = 0;
	*(m->counters.unresponded_packages) = 0;

	m->interface_state = kSucsecc;

	*m->data.rx_size = 0;
	*m->data.tx_size = 0;
	m->data.state = kClearPackage;

#ifdef USE_PROGRAMM_TIMER
	//данные значения подобраны для периода основного коллбека 1мс и скорости передачи данных >= 19200
	m->programm_timer.scale = 0;
	m->programm_timer.state = kOff; //ПОТОМ НАДО ИСПРАВлЯТЬ НА АВТОРАСЧЕТ
#else
	m->timer_callback = MB_15tCallback;
#endif

	m->mode = role;
	switch(role)
	{
		case kSlave: // !!!
			m->update = MB_SlaveUpdate;
			m->rx_callback = MB_SlaveRxTrueCallback;
			break;
		case kMaster:
			m->update = MB_MasterUpdate;
			m->rx_callback = MB_RxMasterCallback;
			break;
	}



	return error;
}

static void MB_SetInterfaceFunctions(MbStr *m, rx_callback_function rx_callback_function, send_bytes send_bytes, void* arg)
{
	m->interface_functions.rx_callback_function = rx_callback_function;
	m->interface_functions.send_bytes = send_bytes;
	m->interface_functions.arg = arg;

}


static void MB_SetInterfaceDataLinks(MbStr *m, uint8_t *rx_buf,uint8_t *rx_size, uint8_t *tx_buf, uint8_t *tx_size)
{
	m->data.rx_buf = rx_buf;
	m->data.tx_buf = tx_buf;
	m->data.rx_size = rx_size;
	m->data.tx_size = tx_size;
}

#ifndef USE_PROGRAMM_TIMER
static void MB_SetTimerFunctions(MbStr *m, timer_start timer_start, timer_stop timer_stop, timer_set_new_period timer_set_new_period, void* arg)
{

	m->tim_func.timer_set_new_period = timer_set_new_period;
	m->tim_func.timer_start = timer_start;
	m->tim_func.timer_stop = timer_stop;
	m->tim_func.arg = arg;
}
#endif

static void MB_SetServerId(MbStr *m, char sizeOfServerId, char *serverId)
{
	m->size_of_server_id = sizeOfServerId;
	m->server_id = serverId;
}

void MB_Update(MbStr *m)
{
	m->update(m);
}

void Mb_RxCallback(MbStr *m)
{
	m->rx_callback(m);
}

static void MB_SetGlVarPointer(MbStr *m, uint32_t **glVar)
{
	m->global_var = glVar;
}

#ifndef USE_PROGRAMM_TIMER
void MB_TimerCallback(MbStr *m)
{
	m->timer_callback(m);
}
#endif

static void MB_ProgrammTimerUpdate(MbStr *m)
{
	//ВРЕМЕННО ПИШУ ТАК ЧТОБЫ КОМПИЛЯТОР НЕ РУГАЛсЯ, ПОТОМ ИСПРАВИТЬ!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


	if(m->programm_timer.state == kOn)
		{
			if(m->programm_timer.scale == m->programm_timer.scale_15t)
				MB_15tCallback(m);
			else if (m->programm_timer.scale >= m->programm_timer.scale_35t)
				MB_35tCallback(m);

			++m->programm_timer.scale;
		}
}




//колбек, возникающий при превышении времени ожидания между пакетами 1.5 чара
static void MB_15tCallback(MbStr *m)
{
	if(m->mode == kMaster) return;
#ifndef USE_PROGRAMM_TIMER
	m->tim_func.timer_stop(m->tim_func.arg);
#endif

	if(!(m->data.state == kOverflow))
		m->rx_callback = MB_RxCorruptCallback;

#ifndef USE_PROGRAMM_TIMER
	m->tim_func.timer_set_new_period(m->tim_func.arg,(m->delay_table[kPosDelay35t] - m->delay_table[kPosDelay15t]));
	m->timer_callback = MB_35tCallback;
	m->tim_func.timer_start(m->tim_func.arg);
#endif
}

//колбек, возникающий при превышении времени ожидания между пакетами 3.5 чара
static void MB_35tCallback(MbStr *m)
{
#ifndef USE_PROGRAMM_TIMER
	m->tim_func.timer_stop(m->tim_func.arg);
#endif

	if(m->mode == kMaster)
	{
		if(m->state == kNeedSendReq)
		{
			m->rx_callback = MB_RxMasterCallback;
			m->state = kReseivingResp;
		}
		else if(m->state == kReseivingResp)
		{
			m->state = kResResp;
		}

		m->programm_timer.state = kOff;
		return;
	}








	if (m->state == kNeedResp)
	{
		m->state = kNeedSendResp;
		m->programm_timer.state = kOff;
		return;
	}


	switch (m->data.state)
	{
	case kOverflow:
		++(*m->counters.overflow_packages);
		m->state = kZeroState;
		break;
	case kCorrupted:
		++(*m->counters.corrupted_packages);
		m->state = kZeroState;
		break;
	case kClearPackage:
		//устанавливаем флаг успешного приема пакета
		m->state = kResReq;
		break;
	}


	m->programm_timer.state = kOff;
}





static void MB_Reset(MbStr* m)
{
	if(m->mode == kSlave)
	{
		m->rx_callback = MB_SlaveRxTrueCallback;
		*(m->data.rx_size) = 0;
		*(m->data.tx_size) = 0;
		m->state = kWaitData;
		m->data.state = kClearPackage;
		m->programm_timer.scale = 0;
		m->programm_timer.state = kOff;

#ifndef USE_PROGRAMM_TIMER
		m->timer_callback = MB_15tCallback;
		m->tim_func.timer_stop(m->tim_func.arg);
#endif

	}
	else if (m->mode == kMaster)
	{
		m->rx_callback = MB_RxMasterCallback;
		*(m->data.rx_size) = 0;
		*(m->data.tx_size) = 0;
		m->state = kWaitData;
		m->data.state = kClearPackage;
		m->programm_timer.scale = 0;
		m->programm_timer.state = kOff;
		m->reseive_timeout_counter = 0;
#ifndef USE_PROGRAMM_TIMER
		m->timer_callback = MB_35tCallback;
		m->tim_func.timer_stop(m->tim_func.arg);
#endif

	}

}


static MbError MB_AccReq(MbStr *m)
{

	uint8_t crcMatch = MB_CheckCrcRx(m);
	MbError error = kNotError;
	if(crcMatch)
	{
		error = MB_SlaveCheckData(m);
		if(m->mb_params.id == 0x00)
		{
			//вот тут нужно сделать общий запрос!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			if(error == kNotError)
			{

			}
			else
				MB_SlaveError(m);
		}
		else if(m->mb_params.id == *(m->connection_parameters.modbus_id))
		{
			m->state = kNeedResp;
			if(error != kNotError)
				MB_SlaveError(m);
		}
		else
		{
			MB_SlaveError(m);
		}

	}
	else
	{
		m->state = kZeroState;	//вот тут надо подумать какое состояние устанавливать
		error = kCrcError;
	}
	return error;
}



void MB_SetTransmitCompleteFlag(MbStr *m)
{
	if(m->mode == kMaster)
	{
		m->interface_state = kSucsecc;
#ifndef USE_PROGRAMM_TIMER
		m->timer_callback = MB_35tCallback;
		m->tim_func.timer_set_new_period(m->tim_func.arg,m->delay_table[kPosDelay35t]);
		m->tim_func.timer_start(m->tim_func.arg);
#else
		MB_ResetProgrammTimer(m);
#endif
	}
	else
	{
		m->state = kZeroState;
		m->interface_state = kSucsecc;
#ifndef USE_PROGRAMM_TIMER
		m->tim_func.timer_stop(m->tim_func.arg);
#else
		m->programm_timer.state = kOff;
#endif
	}


}


//основная функция апдейта с проверкой состояний драйвера





static void MB_RxCorruptCallback(MbStr *m)
{

	if((m->data.state != kClearPackage) || ((m->state == kError))) return;
#ifndef USE_PROGRAMM_TIMER
	m->tim_func.timer_stop(m->tim_func.arg);
#endif
	m->data.state = kCorrupted;
	m->data.rx_buf[*(m->data.rx_size)++] = m->interface_functions.rx_callback_function(m->interface_functions.arg);
	*m->data.rx_size = 0;
	m->programm_timer.state = kOn;
	//если аппаратный таймер, нужно попасть в коллбек по 3.5t
}

//данная функция обновляет и включает программный таймер таймер
static void MB_ResetProgrammTimer(MbStr *m)
{
	m->programm_timer.scale = 0;
	m->programm_timer.state = kOn;
}

/**
 * @brief   CRC,
 *				      CRC
 *				     .
 * @param ByteBeforeCrc -      CRC;
 * @param *Buf4Check -   ,    ;
 * @retval CRC -   CRC   uint16_t.
 */
static uint16_t MB_CrcCalc(uint8_t BytesBeforeCrc, uint8_t *Buf4Check)
{
	uint8_t CRC_Hi_Table = 0xFF, CRC_Lo_Table = 0xFF, CRC_Counter = 0;
	for(uint8_t i = 0; i < BytesBeforeCrc; ++i)
	{
		CRC_Counter = CRC_Hi_Table ^ Buf4Check[i];
		CRC_Hi_Table = CRC_Lo_Table ^ auchCRCHi[CRC_Counter];
		CRC_Lo_Table = auchCRCLo[CRC_Counter];
	}
	return (CRC_Hi_Table << 8) | (CRC_Lo_Table);
}

static uint8_t MB_CheckCrcRx(MbStr *m)
{
	uint8_t match;
	uint16_t CRC16fromTable = MB_CrcCalc((*m->data.rx_size - 2), m->data.rx_buf);
	uint16_t CRC16fromPckg = ((m->data.rx_buf[*m->data.rx_size - 2] << 8) | m->data.rx_buf[*m->data.rx_size - 1]);
	if(CRC16fromTable == CRC16fromPckg)
	{
		match = 1;
	}
	else
	{
		match = 0;
	}
	return match;
}









static void MB_SlaveUpdate(MbStr *m)
{

	//проверка изменения бодрейта для пересчета таймингов
	if(*(m->connection_parameters.current_baudrate) != m->connection_parameters.old_baudrate)
	{
#ifndef USE_PROGRAMM_TIMER
		error = MB_CalculateTimingsForHardwareTimer(m, m->timer_frequency, *(m->connection_parameters.current_baudrate));
#else
		//эта функция возвращает значение, можем проверять его и закидывать в общий стейт
		m->state = MB_CalculateTimingsForSoftwareTimer(m, m->callback_frequency, *(m->connection_parameters.current_baudrate));
		if(m->state == kError) return;
#endif
		*(m->connection_parameters.current_baudrate) = m->connection_parameters.old_baudrate;
	}

	//сначала реализация программного таймера
#ifdef USE_PROGRAMM_TIMER
	MB_ProgrammTimerUpdate(m);
	if(m->programm_timer.state == kOn) return;
#endif


    switch (m->state)
	{
	case kNotInit: 			//what are u doing here?
		return;
		break;
	case kZeroState:
		MB_Reset(m);
		break;
	case kWaitData:
		return;
		break;
	case kNeedResp:			//нужен ответ


		break;
	case kResReq:			//успешно принят запрос
		//m->interface_functions.rx_it_disable(m->interface_functions.arg);
		m->error_state = MB_AccReq(m);
		if(m->error_state == kNotError)
		{
			MB_SlaveResponce(m);
		}
		else if (m->error_state == kCrcError) return;
#ifndef USE_PROGRAMM_TIMER
		m->timer_callback = MB_35tCallback;
		m->tim_func.timer_set_new_period(m->tim_func.arg,m->delay_table[kPosDelay35t]);
		m->tim_func.timer_start(m->tim_func.arg);
#else
		MB_ResetProgrammTimer(m);
#endif

		break;
	case kNeedSendResp:		//нужно отправить ответ
		if(!(m->interface_state == kBusy))
		{
			m->interface_state = kBusy;
			m->interface_functions.send_bytes(m->interface_functions.arg, m->data.tx_buf, m->data.tx_size);
		}

		//устанавливаем флаг занятости периферии, передающей данные.
		break;
	default:
		return;//???

	}



}





static void MB_SlaveRxTrueCallback(MbStr *m)
{
	// если мы попали сюда в ненужный стейт или если пакет сломан, а мы все равно оказались по какой то причине в данной функции
	if((m->state != kWaitData) || (m->data.state != kClearPackage) || ((m->state == kError))) return;
#ifndef USE_PROGRAMM_TIMER
	m->tim_func.timer_stop(m->tim_func.arg);
#endif
	m->data.rx_buf[(*m->data.rx_size)++] = m->interface_functions.rx_callback_function(m->interface_functions.arg); //эта функция должна принимать байт с периферии и записывать в свою структуру

	//если количество принятых байт равно максимальному значению
	if(*m->data.rx_size >= MAX_NUM_PACK)
	{
		//устанавливаем флаг переполнения
		m->data.state = kOverflow;
		*m->data.rx_size = 0;
		//отключаем прием, чтобы обновить драйвер
	}
	else
	{

#ifndef USE_PROGRAMM_TIMER
		//Устанавливаем период 1.5t
		m->tim_func.timer_set_new_period(m->tim_func.arg, m->delay_table[kPosDelay15t]);
		m->timer_callback = MB_15tCallback;
		m->tim_func.timer_start(m->tim_func.arg);
#else
		MB_ResetProgrammTimer(m);
#endif

	}


}







static void MB_SlaveResponse0x01(MbStr *m)
{
	*m->data.tx_size = 3;
	uint16_t crc16 = 0;

	m->data.tx_buf[0] = m->mb_params.id;
	m->data.tx_buf[1] = m->mb_params.command;
	m->data.tx_buf[2] = (m->mb_params.len % 8 > 0) ? m->mb_params.len / 8 + 1 : m->mb_params.len / 8; //число байт

	//uint16_t out_state = (uint16_t)(global_var.user_var.disctere_out_state.value.all & 0x0000FFFF);

//	out_state = out_state >> (m->mb_params.addr);
//	out_state = out_state << (16 - m->mb_params.len);
//	out_state = out_state >> (16 - m->mb_params.len);

//	m->data.tx_buf[(*m->data.tx_size)++] = out_state & 0x00FF;
//	if (m->mb_params.len > 8)
//		m->data.tx_buf[(*m->data.tx_size)++] = out_state >> 8;



	crc16 = MB_CrcCalc(*m->data.tx_size, m->data.tx_buf);
	m->data.tx_buf[*m->data.tx_size + 0] = (crc16 >> 8) & 0xFF;
	m->data.tx_buf[*m->data.tx_size + 1] = (crc16) & 0xFF;
	*m->data.tx_size += 2;
}

static void MB_SlaveResponse0x02(MbStr *m)
{
	*m->data.tx_size = 3;
	uint16_t crc16 = 0;

	m->data.tx_buf[0] = m->mb_params.id;
	m->data.tx_buf[1] = m->mb_params.command;
	m->data.tx_buf[2] = (m->mb_params.len % 8 > 0) ? m->mb_params.len / 8 + 1 : m->mb_params.len / 8; //число байт

//	uint16_t out_state = (uint16_t)(global_var.user_var.discrete_in_state.value.all & 0x0000FFFF);
//
//	out_state = out_state >> (m->mb_params.addr);
//	out_state = out_state << (16 - m->mb_params.len);
//	out_state = out_state >> (16 - m->mb_params.len);

//	m->data.tx_buf[(*m->data.tx_size)++] = out_state & 0x00FF;
//	if (m->mb_params.len / 8 > 0)
//		m->data.tx_buf[(*m->data.tx_size)++] = out_state >> 8;

	crc16 = MB_CrcCalc(*m->data.tx_size, m->data.tx_buf);
	m->data.tx_buf[*m->data.tx_size + 0] = (crc16 >> 8) & 0xFF;
	m->data.tx_buf[*m->data.tx_size + 1] = (crc16) & 0xFF;
	*m->data.tx_size += 2;
}
static void MB_SlaveResponse0x05(MbStr *m)
{
	*m->data.tx_size = 6;
	uint16_t crc16 = 0;

	m->data.tx_buf[0] = m->mb_params.id;
	m->data.tx_buf[1] = m->mb_params.command;

	m->data.tx_buf[2] = (m->mb_params.addr >> 8) & 0xFF;
	m->data.tx_buf[3] = (m->mb_params.addr) & 0xFF;

//	if(m->mb_params.val == 0x0000) global_var.user_var.disctere_out_state.value.all &= ~(1 << m->mb_params.addr) ;		//выключить
//	else if(m->mb_params.val == 0xFF00) global_var.user_var.disctere_out_state.value.all |= (1 << m->mb_params.addr) ;		//включить

	m->data.tx_buf[4] = (m->mb_params.val >> 8) & 0xFF;
	m->data.tx_buf[5] = (m->mb_params.val) & 0xFF;

	crc16 = MB_CrcCalc(*m->data.tx_size, m->data.tx_buf);
	m->data.tx_buf[*m->data.tx_size + 0] = (crc16 >> 8) & 0xFF;
	m->data.tx_buf[*m->data.tx_size + 1] = (crc16) & 0xFF;
	*m->data.tx_size += 2;
}

static void MB_SlaveResponse0x0F(MbStr *m)
{
	*m->data.tx_size = 6;
	uint16_t crc16 = 0;

	m->data.tx_buf[0] = m->mb_params.id;
	m->data.tx_buf[1] = m->mb_params.command;
	m->data.tx_buf[2] = (m->mb_params.addr >> 8) & 0xFF;
	m->data.tx_buf[3] = (m->mb_params.addr) & 0xFF;

	uint16_t k = 0;
	for(int i = m->mb_params.addr; i < m->mb_params.addr + m->mb_params.len; i++)
	{
		uint16_t temp_mask = (m->mb_params.val >> k++) & 1;
//		if(temp_mask)
//			global_var.user_var.disctere_out_state.value.all |= (1 << i);
//		else
//			global_var.user_var.disctere_out_state.value.all &= ~(1 << i);
	}

	m->data.tx_buf[4] = (m->mb_params.len >> 8) & 0xFF;
	m->data.tx_buf[5] = (m->mb_params.len) & 0xFF;


	crc16 = MB_CrcCalc(*m->data.tx_size, m->data.tx_buf);
	m->data.tx_buf[*m->data.tx_size + 0] = (crc16 >> 8) & 0xFF;
	m->data.tx_buf[*m->data.tx_size + 1] = (crc16) & 0xFF;
	*m->data.tx_size += 2;
}


static void ReadGlobalVar16b(uint32_t **p_var16, uint16_t addr, uint16_t len, uint16_t *p_user_arr)
{
		//пусть начальный адрес равен 3, длина равна 4.
		//тогда вычисляем с какого двойного слова (с какого элемента массива указателей) нужно начать
		uint16_t first_world = addr/2;
		//далее шагаем до этого элемента, заводим указатель
		uint32_t *p;
		//в цикле в котором количество итераций равно first_world инкрементируем указатель
		for(int i = 0; i < first_world; i++)
			p = *p_var16++;
		p = *p_var16++;
		//далее вычисляем с какой части двойного слова начать чтение
		uint8_t part_world = addr % 2;
		//Если part_world == 1, начинаем со второй части слова, если 0 - с первой
		//заподим переменную счетчик, промежуточный буфер для хранения
		uint32_t temp;
		uint16_t temp1[2];
		uint16_t i = 0;
		//проверяем, если считывание начинается со второго слова
		if(part_world == 1)
		{
			temp = *p;
			temp1[0] = (temp >> 8) & 0x00FF;
			temp1[1] = (temp << 8) & 0xFF00;
			temp = temp1[0] | temp1[1];
			p_user_arr[i++] = (temp >> 16) & 0xFFFF;
			p = *p_var16++;
			if(i == len) goto FINISH; 	//тут подумать как заканчивать алгоритм
		}

		while(1)
		{


			temp = *p;
			temp1[0] = (temp >> 8) & 0x00FF;
			temp1[1] = (temp << 8) & 0xFF00;
			temp = temp1[0] | temp1[1];
			p_user_arr[i++] = temp & 0xFFFF;
			if(i == len) goto FINISH; 	//тут подумать как заканчивать алгоритм

			temp = *p;
			temp = (temp >> 16) & 0xFFFF;
			temp1[0] = (temp >> 8) & 0x00FF;
			temp1[1] = (temp << 8) & 0xFF00;
			temp = temp1[0] | temp1[1];
			p_user_arr[i++] = (temp) & 0xFFFF;
			if(i == len) goto FINISH; 	//тут подумать как заканчивать алгоритм

			p = *p_var16++;

		}

	FINISH:
		return;

}

//функция формирует ответ на запрос 0х03
static void MB_SlaveResponse0x03(MbStr *m)
{

	uint16_t crc16 = 0;

	m->data.tx_buf[0] = m->mb_params.id;
	m->data.tx_buf[1] = m->mb_params.command;
	m->data.tx_buf[2] = (uint8_t)(m->mb_params.numReg * 2);

	ReadGlobalVar16b(m->global_var, m->mb_params.addr, m->mb_params.numReg, (uint16_t*)&m->data.tx_buf[3]);
	*m->data.tx_size = 3;
	*m->data.tx_size += m->mb_params.numBytes;

	crc16 = MB_CrcCalc(*m->data.tx_size, m->data.tx_buf);
	m->data.tx_buf[*m->data.tx_size + 0] = (crc16 >> 8) & 0xFF;
	m->data.tx_buf[*m->data.tx_size + 1] = (crc16) & 0xFF;
	*m->data.tx_size += 2;

}



static void WriteGlobalVar16b(uint32_t **p_var16, uint16_t addr, uint16_t len, uint16_t *p_user_arr)
{
		//пусть начальный адрес равен 3, длина равна 4.
		//тогда вычисляем с какого двойного слова (с какого элемента массива указателей) нужно начать
		uint16_t first_world = addr/2;
		//далее шагаем до этого элемента, заводим указатель
		uint32_t *p;
		//в цикле в котором количество итераций равно first_world инкрементируем указатель
		for(int i = 0; i < first_world; i++)
			p = *p_var16++;
		p = *p_var16++;
		//далее вычисляем с какой части двойного слова начать чтение
		uint8_t part_world = addr % 2;
		//Если part_world == 1, начинаем со второй части слова, если 0 - с первой
		//заподим переменную счетчик, промежуточный буфер для хранения
		uint16_t temp;
		uint16_t temp1[2];
		uint16_t i = 0;
		//проверяем, если запись начинается со второго слова
		if(part_world == 1)
		{
			temp = p_user_arr[i++];
			temp1[0] = (temp >> 8) & 0x00FF;
			temp1[1] = (temp << 8) & 0xFF00;
			temp = temp1[0] | temp1[1];
			*p = (*p & 0x0000FFFF) + (((uint32_t)temp << 16) & 0xFFFF0000);//(*p & 0xFFFF0000) + ((uint32_t)temp & 0x0000FFFF);
			p = *p_var16++;
			if(i == len) goto FINISH; 	//тут подумать как заканчивать алгоритм
		}

		while(1)
		{
			temp = p_user_arr[i++];
			temp1[0] = (temp >> 8) & 0x00FF;
			temp1[1] = (temp << 8) & 0xFF00;
			temp = temp1[0] | temp1[1];
			*p = (*p & 0xFFFF0000) + ((uint32_t)temp & 0x0000FFFF);
			if(i == len) goto FINISH; 	//тут подумать как заканчивать алгоритм

			temp = p_user_arr[i++];
			temp1[0] = (temp >> 8) & 0x00FF;
			temp1[1] = (temp << 8) & 0xFF00;
			temp = temp1[0] | temp1[1];
			*p = (*p & 0x0000FFFF) + (((uint32_t)temp << 16) & 0xFFFF0000);
			if(i == len) goto FINISH; 	//тут подумать как заканчивать алгоритм

			p = *p_var16++;

		}

	FINISH:
		return;

}


//функция формирует ответ на запрос 0х10
static void MB_SlaveResponse0x10(MbStr *m)
{
	*m->data.tx_size = 8;
	uint16_t crc16 = 0; //
	m->data.tx_buf[0] = m->mb_params.id;
	m->data.tx_buf[1] = m->mb_params.command;
	m->data.tx_buf[2] = (m->mb_params.addr >> 8) & 0xFF;
	m->data.tx_buf[3] = (m->mb_params.addr) & 0xFF;

	WriteGlobalVar16b(m->global_var, m->mb_params.addr, m->mb_params.numReg, (uint16_t*)&m->data.rx_buf[7]);

	m->data.tx_buf[4] = (m->mb_params.numReg >> 8) & 0xFF;
	m->data.tx_buf[5] = (m->mb_params.numReg) & 0xFF;

	crc16 = MB_CrcCalc(*m->data.tx_size - 2, m->data.tx_buf);
	m->data.tx_buf[*m->data.tx_size - 2] = (crc16 >> 8) & 0xFF;
	m->data.tx_buf[*m->data.tx_size - 1] = (crc16) & 0xFF;
}

//функция формирует ответ на запрос 0х11
static void MB_SlaveResponse0x11(MbStr *m)
{
	*m->data.tx_size = 4;
	uint16_t crc16 = 0; //
	m->data.tx_buf[0] = m->mb_params.id;
	m->data.tx_buf[1] = m->mb_params.command;
	m->data.tx_buf[2] = m->size_of_server_id;
	for(int i = 0; i < m->size_of_server_id; i++) m->data.tx_buf[3 + i] = m->server_id[i];
	*m->data.tx_size += m->size_of_server_id;

	if(m->state != kError)
		m->data.tx_buf[3 + m->size_of_server_id] = 0xFF;
	else
		m->data.tx_buf[3 + m->size_of_server_id] = 0x00;
	//???????????????????

	crc16 = MB_CrcCalc(*m->data.tx_size - 2, m->data.tx_buf);
	m->data.tx_buf[*m->data.tx_size - 2] = (crc16 >> 8) & 0xFF;
	m->data.tx_buf[*m->data.tx_size - 1] = (crc16) & 0xFF;
}

//данная функция формирует ответ "ошибка"
static void MB_SlaveError(MbStr *m)
{
	uint16_t crc16;
	m->data.tx_buf[0] = m->mb_params.id;
	m->data.tx_buf[1] = m->mb_params.command | 0x80;
	m->data.tx_buf[2] = m->exception_code;
	crc16 = MB_CrcCalc(3, m->data.tx_buf);
	m->data.tx_buf[3] = (crc16 >> 8) & 0xFF;
	m->data.tx_buf[4] = (crc16) & 0xFF;
	*m->data.tx_size = 5;
}


//данная функция формирует ответ на запрос master'a
static void MB_SlaveResponce(MbStr *m)
{
	commands_responses[m->current_command](m);
}


//данная функция просто парсит принятые данные, выделяя правильную команду и проверяя на отсутствие ошибок
static MbError MB_SlaveCheckData(MbStr *m)
{
	uint8_t temp_id = m->data.rx_buf[0];
	uint8_t tempFunc = m->data.rx_buf[1];
	uint16_t tempAddr = (m->data.rx_buf[2] << 8) | (m->data.rx_buf[3]);
	uint16_t tempNumReg = (m->data.rx_buf[4] << 8) | (m->data.rx_buf[5]);
	uint8_t tempNumBytes = 0;
	MbError error = kNotError;

	m->mb_params.addr = 0;
	m->mb_params.numReg = 0;
	m->mb_params.numBytes = 0;
	m->mb_params.command = 0;
	m->mb_params.len = 0;
	m->mb_params.val = 0;

	m->mb_params.id = temp_id;



	switch(tempFunc)
	{
	case 0x01:

		m->mb_params.command = tempFunc;
		m->current_command = kReadCoild0x01;


		if((tempNumReg < 0x0001) || (tempNumReg > 0x07D0))
		{
			m->exception_code = kExceptionCode3;
			error = kDataValueError;
		}
		else if((tempAddr >= kNumOutputs) || (tempAddr + tempNumReg >= kNumOutputs))
		{
			m->exception_code = kExceptionCode2;
			error = kNotAvaibleAddrCode;
		}
		else
		{
			m->mb_params.addr = tempAddr;
			m->mb_params.len = tempNumReg;
		}


		break;
	case 0x02:
		m->mb_params.command = tempFunc;
		m->current_command = kReadDiscreteInputs0x02;

		if((tempNumReg < 0x0001) || (tempNumReg > 0x07D0))
		{
			m->exception_code = kExceptionCode3;
			error = kDataValueError;
		}
		else if((tempAddr >= kNumInputs) || (tempAddr + tempNumReg >= kNumInputs))
		{
			m->exception_code = kExceptionCode2;
			error = kNotAvaibleAddrCode;
		}
		else
		{
			m->mb_params.addr = tempAddr;
			m->mb_params.len = tempNumReg;
		}



		break;

	case 0x05:
		m->mb_params.command = tempFunc;
		m->current_command = kWriteSingleCoil0x05;

		if((tempNumReg != 0x0000) && (tempNumReg != 0xFF00))
		{
			m->exception_code = kExceptionCode3;
			error = kDataValueError;
		}
		else if(tempAddr >= kNumOutputs)
		{
			m->exception_code = kExceptionCode2;
			error = kNotAvaibleAddrCode;
		}
		else
		{
			m->mb_params.addr = tempAddr;
			m->mb_params.val = tempNumReg;
		}




		break;

	case 0x0F:
		m->mb_params.command = tempFunc;
		m->current_command = kWriteMultipleCoils0x0F;
		tempNumBytes = m->data.rx_buf[6];
		if(((tempNumReg < 0x0001) || (tempNumReg > 0x07B0)))
		{
			m->exception_code = kExceptionCode3;
			error = kDataValueError;
		}
		else if((tempAddr >= kNumOutputs) || (tempAddr + tempNumReg >= kNumOutputs))
		{
			m->exception_code = kExceptionCode2;
			error = kNotAvaibleAddrCode;
		}
		else
		{
			m->mb_params.addr = tempAddr;
			m->mb_params.len = tempNumReg;
			//m->mb_params.val = (m->data.rx_buf[7] << 8) | (m->data.rx_buf[8]);
			m->mb_params.val = m->data.rx_buf[7];
		}

		break;

	case 0x11:
		m->mb_params.command = tempFunc;
		m->current_command = kReportServerID0x11;

		break;

	case 0x03:

		m->mb_params.command = tempFunc;
		m->mb_params.numBytes = (uint8_t)(tempNumReg * 2);
		m->current_command = kReadHoldingRegisters0x03;

		if(((tempNumReg < 0x0001) || (tempNumReg > 0x007D)))
		{
			m->exception_code = kExceptionCode3;
			error = kDataValueError;
		}
		else if((tempAddr >= kNumGlobalRegistersForModbus) || (tempAddr + tempNumReg >= kNumGlobalRegistersForModbus))
		{
			m->exception_code = kExceptionCode2;
			error = kNotAvaibleAddrCode;
		}
//		else if((tempAddr >= kBeginSystemVar) && (!global_var.opened_system_var))		//иначе если это системные переменные и нельзя их читать
//		{
//			m->exception_code = kExceptionCode2;
//			error = kReadWriteSystemVarWithoutPassw;
//		}
		else
		{
			m->mb_params.addr = tempAddr;
			m->mb_params.numReg = tempNumReg;
		}

		break;
	case 0x10:

		m->current_command = kWriteMultipleRegisters0x10;
		m->mb_params.command = tempFunc;

		tempNumBytes = m->data.rx_buf[6];

		if(((tempNumReg < 0x0001) || (tempNumReg > 0x07D0)) && (tempNumBytes != tempNumReg * 2))
		{
			m->exception_code = kExceptionCode3;
			error = kDataValueError;
		}
		else if((tempAddr >= kNumGlobalRegistersForModbus) || (tempAddr + tempNumReg >= kNumGlobalRegistersForModbus))
		{
			m->exception_code = kExceptionCode2;
			error = kNotAvaibleAddrCode;
		}
		else
		{
			uint8_t check_addr = tempAddr / 2;
			uint8_t check_len = (tempNumReg % 2) ? tempNumReg / 2  + 1: tempNumReg / 2;

			for(int i = check_addr; i < (check_addr + check_len); i++)
				if(var_pointers[i]->modbus_access == kModbusRead)
				{
					m->exception_code = kExceptionCode2;
					error = kWriteOnlyReadVar;
					return error;
				}

			m->mb_params.addr = tempAddr;
			m->mb_params.numReg = tempNumReg;
			m->mb_params.numBytes = tempNumBytes;
		}


		if(((*m->data.rx_size) - CRC16_SIZE - 7) == tempNumBytes)
		{
			if(tempNumBytes == tempNumReg * 2)
			{
				m->mb_params.numBytes = tempNumBytes;
			}
			else
			{
				error = kNotAvaibleAddrCode; // NOT_AVAILIBLE_ADDR_CODE
			}
		}
		else
		{
			error = kNotAvaibleAddrCode; // NOT_AVAILIBLE_ADDR_CODE
		}
		break;

		default:
			error = kNotSupportedFunctionCode; //  NOT_SUPPORTED_FUNCTION_CODE
			m->exception_code = kExceptionCode1;
		break;
	}
	return error;
}


//при вызове данной функции пользователю необходимо проверять возвращенное состояние, главное чтобы оно не было равно kError, если равно, значит слишком низкая частота таймера
//timer_frequency - частота тиков таймера (после предделителя)
MbState MB_CalculateTimingsForHardwareTimer(MbStr *m, uint16_t timer_frequency, uint32_t baudrate)
{
#ifndef USE_PROGRAMM_TIMER
	MbState state = kNotInit;
	if(baudrate > 19200)
	{
		m->delay_table[kPosDelay15t] = ((75*baudrate/10+1000) * timer_frequency)/(baudrate * 1000) + 1;
		if (m->delay_table[kPosDelay15t]  == 0) state = kError;

		m->delay_table[kPosDelay35t] = ((175*baudrate/10+1000) * timer_frequency)/(baudrate * 1000) + 1;
		if (m->delay_table[kPosDelay35t] == 0) state = kError;

	}
	else
	{
		m->delay_table[kPosDelay15t] = (25*timer_frequency)/baudrate + 1;
		if (m->delay_table[kPosDelay15t]  == 0) state = kError;

		m->delay_table[kPosDelay35t] = (45*timer_frequency)/baudrate + 1;
		if (m->delay_table[kPosDelay35t] == 0) state = kError;

	}

	return state;
#else
	(void) m;
	(void) timer_frequency;
	(void) baudrate;
	return kError;
#endif

}

//при вызове данной функции пользователю необходимо проверять возвращенное состояние, главное чтобы оно не было равно kError, если равно, значит слишком низкая частота основного колбека
MbState MB_CalculateTimingsForSoftwareTimer(MbStr *m, uint16_t callback_frequency, uint16_t baudrate)
{
	MbState state = kNotInit;
	if(baudrate > 19200)
	{
		m->programm_timer.scale_15t = ((75*baudrate/10+1000) * callback_frequency)/(baudrate * 1000);
		if (m->programm_timer.scale_15t  == 0) m->programm_timer.scale_15t = 1;

		m->programm_timer.scale_35t = ((175*baudrate/10+1000) * callback_frequency)/(baudrate * 1000);
		if (m->programm_timer.scale_35t == 0)
		{
			m->programm_timer.scale_35t = 1;
			state = kError;
		}


	}
	else
	{

		m->programm_timer.scale_15t = (25*callback_frequency)/baudrate;
		uint8_t scale_15t = (15*callback_frequency)/baudrate;
		if (m->programm_timer.scale_15t == scale_15t) m->programm_timer.scale_15t++;
		if (m->programm_timer.scale_15t  == 0) m->programm_timer.scale_15t = 1; //можно этим показывать ошибку

		m->programm_timer.scale_35t = (45*callback_frequency)/baudrate;
		if (m->programm_timer.scale_35t == 0)
			{
				m->programm_timer.scale_35t = 1;
				state = kError;
			}
		uint8_t scale_35t = (35*callback_frequency)/baudrate;
		if (m->programm_timer.scale_35t == scale_35t) m->programm_timer.scale_35t++;
	}


	return state;

}




































//Функции, использующиеся Master'ом

//в данной функции мы формируем структуру modbus для запроса, а затем формируем массив tx_buf
MbError MB_Request(MbStr *m, void* request, MbCommands command)
{

	if(m->state != kWaitData) return kBusyError;
	MbError error = kNotError;


	switch (command)
	{
	case kReadCoild0x01:
	{
		mbRequest0x01 *req = (mbRequest0x01*) request;
		m->mb_params.id = req->id;
		m->mb_params.addr = req->addr;
		m->current_command = command;
		m->mb_params.command = kFunctionCode[kReadCoild0x01];
		m->mb_params.numReg = req->num_coils; //кол-во койлов
		break;
	}
	case kReadDiscreteInputs0x02:
	{
		mbRequest0x02 *req = (mbRequest0x02*) request;
		m->mb_params.id = req->id;
		m->mb_params.addr = req->addr;
		m->current_command = command;
		m->mb_params.command = kFunctionCode[kReadDiscreteInputs0x02];
		m->mb_params.numReg = req->num_inputs; //кол-во дискретных входов
		break;
	}
	case kReadHoldingRegisters0x03:
	{
		mbRequest0x03 *req = (mbRequest0x03*) request;
		m->mb_params.id = req->id;
		m->mb_params.addr = req->addr;
		m->current_command = command;
		m->mb_params.command = kFunctionCode[kReadHoldingRegisters0x03];
		m->mb_params.numReg = req->num_reg; //кол-во регистров
		break;
	}
	case kWriteSingleCoil0x05:
	{
		mbRequest0x05 *req = (mbRequest0x05*) request;
		m->mb_params.id = req->id;
		m->mb_params.addr = req->addr;
		m->current_command = command;
		m->mb_params.command = kFunctionCode[kWriteSingleCoil0x05];
		m->mb_params.val = req->coil_val;
		break;
	}
	case kWriteMultipleCoils0x0F:
	{
		mbRequest0x0F *req = (mbRequest0x0F*) request;
		m->mb_params.id = req->id;
		m->mb_params.addr = req->addr;
		m->current_command = command;
		m->mb_params.command = kFunctionCode[kWriteMultipleCoils0x0F];
		m->mb_params.numReg = req->num_coils; //кол-во койлов
		m->buf_for_rw = req->coil_value;
		break;
	}
	case kWriteMultipleRegisters0x10:
	{
		mbRequest0x10 *req = (mbRequest0x10*) request;
		m->mb_params.id = req->id;
		m->mb_params.addr = req->addr;
		m->current_command = command;
		m->mb_params.command = kFunctionCode[kWriteMultipleRegisters0x10];
		m->mb_params.numReg = req->num_reg; //кол-во регистров
		m->buf_for_rw = req->write_buf;
		break;
	}
	case kReportServerID0x11:
	{
		mbRequest0x11 *req = (mbRequest0x11*) request;
		m->mb_params.id = req->id;
		m->current_command = command;
		m->mb_params.command = kFunctionCode[kReportServerID0x11];
		break;
	}
	default:
		error = kNotSupportedFunctionCode;
		break;

	}
	commands_requests[command](m);
	m->state = kNeedSendReq;
	return error;
}

MbError MB_ReturnSlaveResponce(MbStr *m, void* response)
{
	if(m->state != kNeedReadResp) return kBusyError;



	if(m->error_state != kNotError) {
		m->state = kZeroState;
		return m->error_state;
	}

	MbError error = kNotError;


	switch (m->current_command)
	{
	case kReadCoild0x01:
	{
		mbResponse0x01 *res = (mbResponse0x01*) response;

		uint8_t temp_num_bytes = m->data.rx_buf[2];
		*(res->num_of_bytes) = temp_num_bytes;
		for(int i = 0; i < temp_num_bytes; i++)
			res->coils_values[i] = m->data.rx_buf[3 + i];

		break;
	}
	case kReadDiscreteInputs0x02:
	{
		mbResponse0x02 *res = (mbResponse0x02*) response;
		uint8_t temp_num_bytes = m->data.rx_buf[2];
		*(res->num_of_bytes) = temp_num_bytes;
		for(int i = 0; i < temp_num_bytes; i++)
			res->inputs_status[i] = m->data.rx_buf[3 + i];
		break;
	}
	case kReadHoldingRegisters0x03:
	{
		mbResponse0x03 *res = (mbResponse0x03*) response;
		uint8_t temp_num_bytes = m->data.rx_buf[2];
		*(res->num_of_registers) = temp_num_bytes / 2;
		uint8_t rx_counter = 3;
		for(int i = 0; i < temp_num_bytes / 2; i++)
		{
			res->read_buf[i] = ((uint16_t)m->data.rx_buf[rx_counter++] << 8) & 0xFF00;
			res->read_buf[i] |= m->data.rx_buf[rx_counter++];
		}
		break;
	}
	case kWriteSingleCoil0x05:
	{

		break;
	}
	case kWriteMultipleCoils0x0F:
	{

		break;
	}
	case kWriteMultipleRegisters0x10:
	{

		break;
	}
	case kReportServerID0x11:
	{
		mbResponse0x11 *res = (mbResponse0x11*) response;

		*(res->size_of_server_id) = m->data.rx_buf[2];
		for(int i = 0; i < *(res->size_of_server_id); i++) res->server_id[i] = m->data.rx_buf[3 + i];


	//	if(m->state != kError)
	//		m->data.tx_buf[3 + m->size_of_server_id] = 0xFF;
	//	else
	//		m->data.tx_buf[3 + m->size_of_server_id] = 0x00;
		//???????????????????
		break;
	}
	default:
		error = kNotSupportedFunctionCode;
		break;

	}
	m->state = kZeroState;
	return error;
}




static void MB_RxMasterCallback(MbStr *m)
{
	// если мы попали сюда в ненужный стейт или если пакет сломан, а мы все равно оказались по какой то причине в данной функции
	if((m->state != kReseivingResp) || (m->data.state != kClearPackage) || ((m->state == kError))) return;
#ifndef USE_PROGRAMM_TIMER
	m->tim_func.timer_stop(m->tim_func.arg);
#endif
	m->data.rx_buf[(*m->data.rx_size)++] = m->interface_functions.rx_callback_function(m->interface_functions.arg); //эта функция должна принимать байт с периферии и записывать в свою структуру

	//если количество принятых байт равно максимальному значению
	if(*m->data.rx_size >= MAX_NUM_PACK)
	{
		//устанавливаем флаг переполнения
		m->data.state = kOverflow;
		*m->data.rx_size = 0;
		//отключаем прием, чтобы обновить драйвер
	}
	else
	{

#ifndef USE_PROGRAMM_TIMER
		//Устанавливаем период 3.5t
		m->tim_func.timer_set_new_period(m->tim_func.arg, m->delay_table[kPosDelay35t]);
		m->timer_callback = MB_35tCallback;
		m->tim_func.timer_start(m->tim_func.arg);
#else
		MB_ResetProgrammTimer(m);
#endif

	}


}

static void MB_MasterUpdate(MbStr *m)
{
	//проверка изменения бодрейта для пересчета таймингов
	if(*(m->connection_parameters.current_baudrate) != m->connection_parameters.old_baudrate)
	{
#ifndef USE_PROGRAMM_TIMER
		error = MB_CalculateTimingsForHardwareTimer(m, m->timer_frequency, *(m->connection_parameters.current_baudrate));
#else
		//эта функция возвращает значение, можем проверять его и закидывать в общий стейт
		m->state = MB_CalculateTimingsForSoftwareTimer(m, m->callback_frequency, *(m->connection_parameters.current_baudrate));
		if(m->state == kError) return;
#endif
		*(m->connection_parameters.current_baudrate) = m->connection_parameters.old_baudrate;
	}

	//условие для проверки таймаута ожидания ответа от Slave'a
	if(m->state == kReseivingResp)
		{
			m->reseive_timeout_counter++;
			if(m->reseive_timeout_counter >= m->callback_frequency)
			{
				m->programm_timer.state = kOff;
				m->state = kZeroState;
				m->error_state = kSlaveResponseTimeoutError;
			}

		}


	//сначала реализация программного таймера
#ifdef USE_PROGRAMM_TIMER
	MB_ProgrammTimerUpdate(m);
	if(m->programm_timer.state == kOn) return;
#endif







	switch (m->state)
	{
	case kNotInit: 			//what are u doing here?
		return;
		break;
	case kZeroState:
		MB_Reset(m);
		break;
	case kWaitData:
		return;
		break;
	case kNeedSendReq:		//нужно отправить запрос
		if(!(m->interface_state == kBusy))
		{
			m->interface_state = kBusy;
			m->interface_functions.send_bytes(m->interface_functions.arg, m->data.tx_buf, m->data.tx_size);
		}

		break;
	case kReseivingResp:
		return;
		break;
	case kResResp:
	{
		uint8_t crcMatch = MB_CheckCrcRx(m);
		if(crcMatch)
		{
			commands_responces_master[m->current_command](m);
		}
		else
		{
			//m->state = kZeroState;	//вот тут надо подумать какое состояние устанавливать
			m->error_state= kCrcError;
		}
		m->state = kNeedReadResp;
		//парсим полученный ответ

		break;
	}
	case kNeedReadResp:
		return;
		break;


	//Думаю состояния ниже не нужны
//
//	case kResReq:			//успешно принят запрос
//
//
//		break;
//	case kNeedSendResp:		//нужно отправить ответ
//		break;
	default:
		return;//???

	}
}



static void MB_MasterSendRequest0x01(MbStr *m)
{
	uint16_t crc16;
	*m->data.tx_size = 6;
	m->data.tx_buf[0] = m->mb_params.id;
	m->data.tx_buf[1] = m->mb_params.command;
	m->data.tx_buf[2] = (m->mb_params.addr >> 8) & 0xFF;
	m->data.tx_buf[3] = (m->mb_params.addr >> 0) & 0xFF;
	m->data.tx_buf[4] = (m->mb_params.numReg >> 8) & 0xFF;
	m->data.tx_buf[5] = (m->mb_params.numReg >> 0) & 0xFF;
	crc16 = MB_CrcCalc(*m->data.tx_size, &m->data.tx_buf[0]);
	m->data.tx_buf[*m->data.tx_size + 0] = (crc16 >> 8) & 0xFF;
	m->data.tx_buf[*m->data.tx_size + 1] = (crc16) & 0xFF;
	*m->data.tx_size += 2;
}
static void MB_MasterSendRequest0x02(MbStr *m)
{
	uint16_t crc16;
	*m->data.tx_size = 6;
	m->data.tx_buf[0] = m->mb_params.id;
	m->data.tx_buf[1] = m->mb_params.command;
	m->data.tx_buf[2] = (m->mb_params.addr >> 8) & 0xFF;
	m->data.tx_buf[3] = (m->mb_params.addr >> 0) & 0xFF;
	m->data.tx_buf[4] = (m->mb_params.numReg >> 8) & 0xFF;
	m->data.tx_buf[5] = (m->mb_params.numReg >> 0) & 0xFF;
	crc16 = MB_CrcCalc(*m->data.tx_size, &m->data.tx_buf[0]);
	m->data.tx_buf[*m->data.tx_size + 0] = (crc16 >> 8) & 0xFF;
	m->data.tx_buf[*m->data.tx_size + 1] = (crc16) & 0xFF;
	*m->data.tx_size += 2;
}
static void MB_MasterSendRequest0x03(MbStr *m)
{
	uint16_t crc16;
	*m->data.tx_size = 6;
	m->data.tx_buf[0] = m->mb_params.id;
	m->data.tx_buf[1] = m->mb_params.command;
	m->data.tx_buf[2] = (m->mb_params.addr >> 8) & 0xFF;
	m->data.tx_buf[3] = (m->mb_params.addr >> 0) & 0xFF;
	m->data.tx_buf[4] = (m->mb_params.numReg >> 8) & 0xFF;
	m->data.tx_buf[5] = (m->mb_params.numReg >> 0) & 0xFF;
	crc16 = MB_CrcCalc(*m->data.tx_size, &m->data.tx_buf[0]);
	m->data.tx_buf[*m->data.tx_size + 0] = (crc16 >> 8) & 0xFF;
	m->data.tx_buf[*m->data.tx_size + 1] = (crc16) & 0xFF;
	*m->data.tx_size += 2;
}
static void MB_MasterSendRequest0x05(MbStr *m)
{
	uint16_t crc16;
	*m->data.tx_size = 6;
	m->data.tx_buf[0] = m->mb_params.id;
	m->data.tx_buf[1] = m->mb_params.command;
	m->data.tx_buf[2] = (m->mb_params.addr >> 8) & 0xFF;
	m->data.tx_buf[3] = (m->mb_params.addr >> 0) & 0xFF;
	m->data.tx_buf[4] = (m->mb_params.val >> 8) & 0xFF;
	m->data.tx_buf[5] = (m->mb_params.val >> 0) & 0xFF;
	crc16 = MB_CrcCalc(*m->data.tx_size, &m->data.tx_buf[0]);
	m->data.tx_buf[*m->data.tx_size + 0] = (crc16 >> 8) & 0xFF;
	m->data.tx_buf[*m->data.tx_size + 1] = (crc16) & 0xFF;
	*m->data.tx_size += 2;
}

static void MB_MasterSendRequest0x0F(MbStr *m)
{
	uint16_t crc16;
	*m->data.tx_size = 7;
	m->data.tx_buf[0] = m->mb_params.id;
	m->data.tx_buf[1] = m->mb_params.command;
	m->data.tx_buf[2] = (m->mb_params.addr >> 8) & 0xFF;
	m->data.tx_buf[3] = (m->mb_params.addr >> 0) & 0xFF;
	m->data.tx_buf[4] = (m->mb_params.numReg >> 8) & 0xFF;
	m->data.tx_buf[5] = (m->mb_params.numReg >> 0) & 0xFF;
	m->data.tx_buf[6] = (m->mb_params.numReg % 8 == 0) ? m->mb_params.numReg / 8 : m->mb_params.numReg / 8 + 1;

	uint8_t* temp_coil_val = (uint8_t*) m->buf_for_rw;

	for(int i = 0; i < m->data.tx_buf[6]; i++)
	{
		m->data.tx_buf[(*m->data.tx_size)++] = temp_coil_val[i];
	}

	crc16 = MB_CrcCalc(*m->data.tx_size, &m->data.tx_buf[0]);
	m->data.tx_buf[*m->data.tx_size + 0] = (crc16 >> 8) & 0xFF;
	m->data.tx_buf[*m->data.tx_size + 1] = (crc16) & 0xFF;
	*m->data.tx_size += 2;
}

static void MB_MasterSendRequest0x10(MbStr *m)
{
	uint16_t crc16;
	*m->data.tx_size = 7;
	m->mb_params.numBytes = m->mb_params.numReg * 2;
	m->data.tx_buf[0] = m->mb_params.id;
	m->data.tx_buf[1] = m->mb_params.command;
	m->data.tx_buf[2] = (m->mb_params.addr >> 8) & 0xFF;
	m->data.tx_buf[3] = (m->mb_params.addr >> 0) & 0xFF;
	m->data.tx_buf[4] = (m->mb_params.numReg >> 8) & 0xFF;
	m->data.tx_buf[5] = (m->mb_params.numReg >> 0) & 0xFF;
	m->data.tx_buf[6] = (m->mb_params.numBytes) & 0xFF;

	uint16_t* buff_for_write = (uint16_t*) m->buf_for_rw;
	for(int i = 0; i < m->mb_params.numBytes / 2; i++)
	{
		m->data.tx_buf[(*m->data.tx_size)++] = (buff_for_write[i] >> 8) & 0x00FF;
		m->data.tx_buf[(*m->data.tx_size)++] = (buff_for_write[i]) & 0x00FF;
	}

	crc16 = MB_CrcCalc(*m->data.tx_size, m->data.tx_buf);
	m->data.tx_buf[*m->data.tx_size + 0] = (crc16 >> 8) & 0xFF;
	m->data.tx_buf[*m->data.tx_size + 1] = (crc16) & 0xFF;
	*m->data.tx_size += 2;
}
static void MB_MasterSendRequest0x11(MbStr *m)
{
	uint16_t crc16;
	*m->data.tx_size = 2;
	m->data.tx_buf[0] = m->mb_params.id;
	m->data.tx_buf[1] = m->mb_params.command;
	crc16 = MB_CrcCalc(*m->data.tx_size, &m->data.tx_buf[0]);
	m->data.tx_buf[*m->data.tx_size] = (crc16 >> 8) & 0xFF;
	m->data.tx_buf[*m->data.tx_size + 1] = (crc16) & 0xFF;
	*m->data.tx_size += 2;
}









static void MB_MasterCheckResponce0x01(MbStr *m)
{
	m->error_state = kNotError;
	if((m->mb_params.command | 0x80) == m->data.rx_buf[1])
	{
		m->exception_code = m->data.rx_buf[2];
		if(m->exception_code == kExceptionCode1)
			m->error_state = kNotSupportedFunctionCode;
		else if(m->exception_code == kExceptionCode2)
			m->error_state = kNotAvaibleAddrCode;
		else if(m->exception_code == kExceptionCode3)
			m->error_state = kDataValueError;
		else if(m->exception_code == kExceptionCode4)
			m->error_state = kCommandExecutionError;


		return;
	}

	if((m->mb_params.id != m->data.rx_buf[0]) || (m->mb_params.command != m->data.rx_buf[1]))
	{
		m->error_state = kSlaveResponseError;
		return;
	}

}

static void MB_MasterCheckResponce0x02(MbStr *m)
{
	m->error_state = kNotError;
	if((m->mb_params.command | 0x80) == m->data.rx_buf[1])
	{
		m->exception_code = m->data.rx_buf[2];
		if(m->exception_code == kExceptionCode1)
			m->error_state = kNotSupportedFunctionCode;
		else if(m->exception_code == kExceptionCode2)
			m->error_state = kNotAvaibleAddrCode;
		else if(m->exception_code == kExceptionCode3)
			m->error_state = kDataValueError;
		else if(m->exception_code == kExceptionCode4)
			m->error_state = kCommandExecutionError;


		return;
	}



	if((m->mb_params.id != m->data.rx_buf[0]) || (m->mb_params.command != m->data.rx_buf[1]))
	{
		m->error_state = kSlaveResponseError;
		return;
	}

}

static void MB_MasterCheckResponce0x03(MbStr *m)
{
	//тут можно еще проверять ID, если не совпал - выдавать ошибку
	m->error_state = kNotError;
	if((m->mb_params.command | 0x80) == m->data.rx_buf[1])
	{
		m->exception_code = m->data.rx_buf[2];
		if(m->exception_code == kExceptionCode1)
			m->error_state = kNotSupportedFunctionCode;
		else if(m->exception_code == kExceptionCode2)
			m->error_state = kNotAvaibleAddrCode;
		else if(m->exception_code == kExceptionCode3)
			m->error_state = kDataValueError;
		else if(m->exception_code == kExceptionCode4)
			m->error_state = kCommandExecutionError;


		return;
	}
	if((m->mb_params.id != m->data.rx_buf[0]) || (m->mb_params.command != m->data.rx_buf[1]))
	{
		m->error_state = kSlaveResponseError;
		return;
	}

}

static void MB_MasterCheckResponce0x05(MbStr *m)
{
	m->error_state = kNotError;
	if((m->mb_params.command | 0x80) == m->data.rx_buf[1])
	{
		m->exception_code = m->data.rx_buf[2];
		if(m->exception_code == kExceptionCode1)
			m->error_state = kNotSupportedFunctionCode;
		else if(m->exception_code == kExceptionCode2)
			m->error_state = kNotAvaibleAddrCode;
		else if(m->exception_code == kExceptionCode3)
			m->error_state = kDataValueError;
		else if(m->exception_code == kExceptionCode4)
			m->error_state = kCommandExecutionError;


		return;
	}
	uint8_t temp_id = m->data.rx_buf[0];
	uint8_t temp_com = m->data.rx_buf[1];
	uint16_t temp_adr = ((uint16_t)m->data.rx_buf[2] << 8) | m->data.rx_buf[3];
	uint16_t temp_val = ((uint16_t)m->data.rx_buf[4] << 8) | m->data.rx_buf[5];




	if((m->mb_params.id != temp_id) || (m->mb_params.command != temp_com) || (m->mb_params.addr != temp_adr) || (m->mb_params.val != temp_val))
	{
		m->error_state = kSlaveResponseError;
		return;
	}

}

static void MB_MasterCheckResponce0x0F(MbStr *m)
{
	m->error_state = kNotError;
	if((m->mb_params.command | 0x80) == m->data.rx_buf[1])
	{
		m->exception_code = m->data.rx_buf[2];
		if(m->exception_code == kExceptionCode1)
			m->error_state = kNotSupportedFunctionCode;
		else if(m->exception_code == kExceptionCode2)
			m->error_state = kNotAvaibleAddrCode;
		else if(m->exception_code == kExceptionCode3)
			m->error_state = kDataValueError;
		else if(m->exception_code == kExceptionCode4)
			m->error_state = kCommandExecutionError;

		return;
	}
	uint8_t temp_id = m->data.rx_buf[0];
	uint8_t temp_com = m->data.rx_buf[1];
	uint16_t temp_adr = ((uint16_t)m->data.rx_buf[2] << 8) | m->data.rx_buf[3];
	uint16_t temp_len = ((uint16_t)m->data.rx_buf[4] << 8) | m->data.rx_buf[5];

	if((m->mb_params.id != temp_id) || (m->mb_params.command != temp_com) || (m->mb_params.addr != temp_adr) || (m->mb_params.numReg != temp_len))
	{
		m->error_state = kSlaveResponseError;
		return;
	}

}

static void MB_MasterCheckResponce0x10(MbStr *m)
{
	m->error_state = kNotError;
	if((m->mb_params.command | 0x80) == m->data.rx_buf[1])
	{
		m->exception_code = m->data.rx_buf[2];
		if(m->exception_code == kExceptionCode1)
			m->error_state = kNotSupportedFunctionCode;
		else if(m->exception_code == kExceptionCode2)
			m->error_state = kNotAvaibleAddrCode;
		else if(m->exception_code == kExceptionCode3)
			m->error_state = kDataValueError;
		else if(m->exception_code == kExceptionCode4)
			m->error_state = kCommandExecutionError;

		return;
	}
	uint8_t temp_id = m->data.rx_buf[0];
	uint8_t temp_com = m->data.rx_buf[1];
	uint16_t temp_adr = ((uint16_t)m->data.rx_buf[2] << 8) | m->data.rx_buf[3];
	uint16_t temp_len = ((uint16_t)m->data.rx_buf[4] << 8) | m->data.rx_buf[5];

	if((m->mb_params.id != temp_id) || (m->mb_params.command != temp_com) || (m->mb_params.addr != temp_adr) || (m->mb_params.numReg != temp_len))
	{
		m->error_state = kSlaveResponseError;
		return;
	}
}

static void MB_MasterCheckResponce0x11(MbStr *m)
{
	m->error_state = kNotError;
	if((m->mb_params.command | 0x80) == m->data.rx_buf[1])
	{
		m->exception_code = m->data.rx_buf[2];
		if(m->exception_code == kExceptionCode1)
			m->error_state = kNotSupportedFunctionCode;
		else if(m->exception_code == kExceptionCode4)
			m->error_state = kDataValueError;


		return;
	}

	uint8_t temp_id = m->data.rx_buf[0];
	uint8_t temp_com = m->data.rx_buf[1];

	if((m->mb_params.id != temp_id) || (m->mb_params.command != temp_com))
	{
		m->error_state = kSlaveResponseError;
		return;
	}

}










