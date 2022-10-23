#ifndef __MY_MODBUS_SLAVE_H__
#define __MY_MODBUS_SLAVE_H__

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif
//-------------------------------------------------------------
//	Определяет максимальный размер пакета (в байтах),
//	принятого за одну транзакцию
//-------------------------------------------------------------
#define MAX_NUM_PACK 255
//-------------------------------------------------------------
//	выбор таймера, для отслеживания таймингов. Если нижепредставленный макрос определен,
//	используется программный таймер, если необходимо использовать аппаратный таймер
//	закомментируйте строку #define USE_PROGRAMM_TIMER
//--------------------------------------------------------------
#define USE_PROGRAMM_TIMER





//==================================================================================
//================================= ENUMS ==========================================
//==================================================================================

//перечисление, определяет состояние модбаса
typedef enum {
	kNotInit = 0,		//драйвер не проинициализирован
	kZeroState,			//начальное состояние драйвера после инициализации
	kWaitData,
	kNeedResp,			//нужен ответ мастеру
	kResReq,			//успешно принят запрос
	kNeedSendResp,		//нужно отправить ответ (устанавливается, когда ответ сформирован)
	kError,
	//ниже прописаны стейты для мастера
	kReseivingResp,
	kResResp,
	kNeedSendReq,
	kNeedReadResp
}MbState;



typedef enum {
	kMaster = 0,
	kSlave
}MbMode;


typedef enum {
	kClearPackage = 0,
	kCorrupted,
	kOverflow,
} MbPackageState;

//Пока не используется
typedef enum {
	kNotError = 0,
	kNotSupportedFunctionCode,
	kNotAvaibleAddrCode,
	kDataValueError,
	kReadWriteSystemVarWithoutPassw,
	kWriteOnlyReadVar,
	kCrcError,
	kCommandExecutionError,
	kSlaveResponseError,		// поля ответа слейва не совпадают с полями мастера.
	kSlaveResponseTimeoutError,	// тайматут ответа Slave'a

	kBusyError		//ошибка возникает когда мы пытаемся отправить запрос, но драйвер занят или когда пытаемся прочитать ответ, но ответа нет или когда пытаемся отправить запрос, не прождав 3.5t после ответа слейва
}MbError;


//состояния программного таймера
typedef enum {
	kOn = 0,
	kOff
}MbProgrammTimer;

//состояния интерфейса передачи данных
typedef enum {
	kSucsecc = 0,
	kBusy
} InterfaceStates;

//индексы для массива задержек при использовании аппаратного таймера
typedef enum {
	kPosDelay15t = 0,
	kPosDelay35t,
	kPosDelayNoPackage,
	kSizeOfDelayTable
} MbDelays;


//все поддерживаемые команды
typedef enum {
	kReadCoild0x01 = 0,
	kReadDiscreteInputs0x02 = 1,
	kReadHoldingRegisters0x03 = 2,
	kWriteSingleCoil0x05 = 3,
	kWriteMultipleCoils0x0F = 4,
	kWriteMultipleRegisters0x10 = 5,
	kReportServerID0x11 = 6,

	kAllFunctions = 7

} MbCommands;


//все коды ошибок, формируемые при отправки пакета ошибки
typedef enum {
	kExceptionCode1 = 1,
	kExceptionCode2 = 2,
	kExceptionCode3 = 3,
	kExceptionCode4 = 4,
} MbExceptionCode;

//==================================================================================
//============================  functions typedefs  ================================
//==================================================================================
typedef struct MbStr MbStr;
typedef void (*MbFunc)(MbStr *);

typedef uint8_t (*rx_callback_function)(void*);			//данная функцивозвращает 1 байт данные их полученного по интерфейсу
typedef void (*send_bytes)(void*, uint8_t*, uint8_t*);	//данная функция инициализирует НЕПРЕРЫВНУЮ отправку данных по интерфейсу (принимает ссылку на буфер отправки и ссылку на длину)



typedef void (*timer_start)(void*);						//данная функция просто запускает таймер
typedef void (*timer_stop)(void*);						//данная функция просто останавливает таймер
typedef void (*timer_set_new_period)(void*, uint16_t);	//данная функция просто устанавливает новый период

//==================================================================================
//===============================  typedef struct  =================================
//==================================================================================




typedef struct {
	struct {
		uint8_t *rx_buf;									// ссылка на массив приема чаров интерфейса
		uint8_t *rx_size;								// ссылка на длину массива приема чаров интерфейса
		uint8_t *tx_buf;									// ссылка на массив отправки чаров интерфейса
		uint8_t *tx_size;								// ссылка на длину массива отправки чаров интерфейса
	} interface_data_links;
	struct {
		rx_callback_function rx_callback_function;		// данная функцивозвращает 1 байт данных полученного по интерфейсу
		send_bytes send_bytes;							// данная функция инициализирует НЕПРЕРЫВНУЮ отправку данных по интерфейс (принимает ссылку на буфер отправки и ссылку на длину)
		void* arg;										// аргумент, который принимают данные функции (по типу функций MB_ с их первым аргументом)
	}interface_functions;
	struct {
		 char size_of_server_id;						// размер ServerId
		 char *server_id;								// ссылка на массив server ID (для команды 0x11)
	} server_id;

	uint32_t **gl_var_pointer;							// ссылка на массив подготовленных ссылок на глобальные переменные
	uint32_t *baudrate;									// бодрейт интерфейса
	uint32_t *modbus_id;
	uint16_t callback_frequency;						// частота вызова функции update

	MbMode role;										// роль драйвера (ведомый или ведущий)

	struct {
		uint32_t *corrupted_packages;
		uint32_t *unresponded_packages;
		uint32_t *overflow_packages;
	} error_counters;									// ссылки на счетчики ошибок
	#ifndef USE_PROGRAMM_TIMER
	struct {
		struct{
			timer_start timer_start;					// данная функцивозвращает включает таймер
			timer_stop timer_stop;						// данная функция останавливает таймер
			timer_set_new_period timer_set_new_period;	// данная функция устанавливает новый период таймера
			void* arg;									// аргумент, который принимают данные функции (по типу функций MB_ с их первым аргументом)
		} functions;
		uint16_t frequency;								// частота таймера

	} hardware_timer;

#else

#endif

} mbInitStruct;


//-------------------------------------------------------------
//	Ниже представлены типы соответствующих запросов. Необходимо
//	заполнить экземпляр какого-либо запроса и вызвать функцию
//	запроса, в которую помещается ссылка на заполненную структуру
//--------------------------------------------------------------

typedef struct {
	uint8_t id;
	uint16_t addr;
	uint16_t num_coils;
} mbRequest0x01;


typedef struct {
	uint8_t id;
	uint16_t addr;
	uint16_t num_inputs;
} mbRequest0x02;

typedef struct {
	uint8_t id;
	uint16_t addr;
	uint16_t num_reg;
} mbRequest0x03;

typedef struct {
	uint8_t id;
	uint16_t addr;
	uint16_t coil_val;
} mbRequest0x05;

typedef struct {
	uint8_t id;
	uint16_t addr;
	uint16_t num_coils;
	uint8_t* coil_value;
} mbRequest0x0F;

typedef struct {
	uint8_t id;
	uint16_t addr;
	uint16_t num_reg;
	uint16_t* write_buf;
} mbRequest0x10;

typedef struct {
	uint8_t id;
} mbRequest0x11;
















typedef struct {
	uint8_t* coils_values;
	uint8_t* num_of_bytes;
} mbResponse0x01;

typedef struct {
	uint8_t* inputs_status;
	uint8_t* num_of_bytes;
} mbResponse0x02;

typedef struct {
	uint16_t* read_buf;
	uint8_t* num_of_registers;
} mbResponse0x03;

typedef struct {
	uint8_t* server_id;
	uint8_t* size_of_server_id;
} mbResponse0x11;


//==================================================================================
//================================ structs =========================================
//==================================================================================
//-------------------------------------------------------------
//	Основная структура драйвера modbus,
//	пользователю необходимо создать экземпляр данной структуры
//--------------------------------------------------------------
struct MbStr{
	//Структура, содержащая ссылки на массивы данных, принимаемых/передаваемых по выбранному интерфейсу и ссылки на размеры этих массивов(размер текущих полезных данных)
	struct{
		uint8_t *rx_buf;
		uint8_t *rx_size;
		uint8_t *tx_buf;
		uint8_t *tx_size;
		MbPackageState state;
	}data;
	MbMode mode;
	MbError error_state;

	 struct{
		uint16_t addr;
		uint16_t val;
		uint8_t id;
		uint8_t command;
		uint8_t numReg;
		uint8_t numBytes;
		uint8_t len;
	}mb_params;

	MbCommands current_command;
	MbExceptionCode exception_code;
	uint32_t **global_var;		//ссылка на массив ссылок на глобальные перменные (нужно переделать на ссылку на массив ссылок)
	MbFunc update;			//ссылка на функцию апдейта, помещается в основной колбек
	MbState state;
	InterfaceStates interface_state;

	struct{
		uint32_t *unresponded_packages;
		uint32_t *corrupted_packages;
		uint32_t *overflow_packages;
	}counters;	//   global var //счетчик ошибок (принимаются ссылки)
	MbFunc rx_callback;		// сыллка на функцию-колбек, которая будет выполняться из прерывания по RX, нужна для внутренней логики.

	struct {
		uint32_t *current_baudrate;
		uint32_t *modbus_id;
		uint32_t old_baudrate;
	} connection_parameters;
	uint16_t callback_frequency;						// частота вызова функции update
	uint16_t reseive_timeout_counter;							// счетчик для таймаута приема ответа от slave'a
	char *server_id;
	char size_of_server_id;


	void *buf_for_rw;


	//структура ответа слейва, полученная мастером
	struct {
		uint16_t val;		//сюда запишется состояние ног при команде 0х01
	} slave_responce;


#ifdef USE_PROGRAMM_TIMER
	struct {
		uint8_t scale;			//счетчик программного таймера
		MbProgrammTimer state;	//состояние программного таймера
		uint8_t scale_15t;		// значение, до которого должен досчитать таймер, чтобы получть задержку 1.5t
		uint8_t scale_35t;		// значение, до которого должен досчитать таймер, чтобы получть задержку 3.5t

	}programm_timer;

#endif
	//данные функции реализуются пользователем, а потом задаются ссылки на эти функции
	struct {
		rx_callback_function rx_callback_function;
		send_bytes send_bytes;
		void *arg;
	}interface_functions;


#ifndef USE_PROGRAMM_TIMER
	struct {
			timer_start timer_start;
			timer_stop timer_stop;
			timer_set_new_period timer_set_new_period;
			void *arg;
		}tim_func;
	MbFunc timer_callback;
	uint16_t delay_table[kSizeOfDelayTable];
	uint32_t timer_frequency;

#endif

};








//==================================================================================
//================================ FUNCTIONs =======================================
//==================================================================================

//---------------------------------------------------------------------------------------------------
//	функция MB_Init()
//
//	данная инициализирует драйвер modbus:
///		mb_init_p - указатель на инициализационную структуру драйвера modbus slave
//---------------------------------------------------------------------------------------------------
MbState MB_Init(MbStr *m, mbInitStruct *mb_init_p);


//---------------------------------------------------------------------------------------------------
//	функция MB_Update()
//	Данная функция помещается в основной коллбек
//---------------------------------------------------------------------------------------------------
void MB_Update(MbStr *m);

//---------------------------------------------------------------------------------------------------
//	функция Mb_RxCallback()
//	Данная функция помещается в колбек по приему 1го байта данных
//---------------------------------------------------------------------------------------------------
void Mb_RxCallback(MbStr *m);

//---------------------------------------------------------------------------------------------------
//	функция MB_SetTransmitCompleteFlag()
//	Данная функция вызывается после успешной отправки всех данных по линии tx
//---------------------------------------------------------------------------------------------------
void MB_SetTransmitCompleteFlag(MbStr *m);

#ifndef USE_PROGRAMM_TIMER
//---------------------------------------------------------------------------------------------------
//	функция MB_TimerCallback()
//	Данная функция помещается в колбек аппаратного таймера
//---------------------------------------------------------------------------------------------------
void MB_TimerCallback(MbStr *m);
#endif




//---------------------------------------------------------------------------------------------------
//	функция MB_Request()
//
//	Данная функция инициализирует отправку запроса слейву в соответствии со структурой запроса
//
///		request - указатель на заполненнуж структуру запроса
/// 	command - команда modbus в соответствие с перечислением MbCommands
//---------------------------------------------------------------------------------------------------
MbError MB_Request(MbStr *m, void* request, MbCommands command);
//---------------------------------------------------------------------------------------------------
//	функция MB_ReturnSlaveResponce()
//
//	Данная функция заполняет соответствующую структуру ответа Slave'а, если нет ошибок,
//	если есть ошибки - возвращает ошибку
//
///		response - указатель на структуру, в которую запишется ответ Slave'a
//---------------------------------------------------------------------------------------------------
MbError MB_ReturnSlaveResponce(MbStr *m, void* response);




#ifdef __cplusplus
}
#endif


#endif
