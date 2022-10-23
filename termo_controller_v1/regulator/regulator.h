#ifndef __REGULATOR_H__
#define __REGULATOR_H__

#include <stdbool.h>
#include <stdint.h>

#define NORMAL false
#define REVERSE true
#define ON_ERROR false
#define ON_RATE true




template <typename datatype>
class regulator {
private:
    int16_t _dt = 100;		// время итерации в мс
    float _dt_s = 0.1;		// время итерации в с
    bool _mode = 0, _direction = 0;
    int _minOut = 0, _maxOut = 255;
    datatype prevInput = 0;
    uint32_t pidTimer = 0;
#if (PID_INTEGRAL_WINDOW > 0)
    datatype errors[PID_INTEGRAL_WINDOW];
    int t = 0;
#endif
	float constrain(float val, float min, float max)
	{
		if(val < min) val = min;
		if (val > max) val = max;
		return val;
	}



public:
    // ==== datatype это float или int, в зависимости от выбранного (см. пример integer_calc) ====
    regulator(){}

    // kp, ki, kd, dt
    regulator(float new_kp, float new_ki, float new_kd, int16_t new_dt = 100) {
        setDt(new_dt);
        Kp = new_kp;
        Ki = new_ki;
        Kd = new_kd;
    }

    // направление регулирования: NORMAL (0) или REVERSE (1)
    void setDirection(bool direction) {
        _direction = direction;
    }

    // режим: работа по входной ошибке ON_ERROR (0) или по изменению ON_RATE (1)
    void setMode(bool mode) {
        _mode = mode;
    }

    // лимит выходной величины (например для ШИМ ставим 0-255)
    void setLimits(int min_output, int max_output) {
        _minOut = min_output;
        _maxOut = max_output;
    }

    // установка времени дискретизации (для getResultTimer)
    void setDt(int16_t new_dt) {
        _dt_s = new_dt / 1000.0f;
        _dt = new_dt;
    }

    datatype setpoint = 0;		// заданная величина, которую должен поддерживать регулятор
    datatype input = 0;			// сигнал с датчика (например температура, которую мы регулируем)
    datatype output = 0;		// выход с регулятора на управляющее устройство (например величина ШИМ или угол поворота серво)
    float Kp = 0.0;				// коэффициент P
    float Ki = 0.0;				// коэффициент I
    float Kd = 0.0;				// коэффициент D
    float integral = 0.0;		// интегральная сумма


    void setParameters(float Kp,  float Ki, float Kd ) {
            this->Kp = Kp;
            this->Ki = Ki;
            this->Kd = Kd;
	}


    // возвращает новое значение при вызове (если используем свой таймер с периодом dt!)
    datatype getResult(datatype input, datatype setpoint) {
        datatype error = setpoint - input;			// ошибка регулирования
        datatype delta_input = prevInput - input;	// изменение входного сигнала за dt
        prevInput = input;							// запомнили предыдущее
        if (_direction) {							// смена направления
            error = -error;
            delta_input = -delta_input;
        }
        output = _mode ? 0 : (error * Kp); 			// пропорциональая составляющая
        output += delta_input * Kd / _dt_s;			// дифференциальная составляющая

#if (PID_INTEGRAL_WINDOW > 0)
        // ЭКСПЕРИМЕНТАЛЬНЫЙ РЕЖИМ ИНТЕГРАЛЬНОГО ОКНА
        if (++t >= PID_INTEGRAL_WINDOW) t = 0; 		// перемотка t
        integral -= errors[t];     					// вычитаем старое
        errors[t] = error * Ki * _dt_s;  			// запоминаем в массив
        integral += errors[t];         				// прибавляем новое
#else
        integral += error * Ki * _dt_s;				// обычное суммирование инт. суммы
#endif

#ifdef PID_OPTIMIZED_I
        // ЭКСПЕРИМЕНТАЛЬНЫЙ РЕЖИМ ОГРАНИЧЕНИЯ ИНТЕГРАЛЬНОЙ СУММЫ
        output = constrain(output, _minOut, _maxOut);
        if (Ki != 0) integral = constrain(integral, (_minOut - output) / (Ki * _dt_s), (_maxOut - output) / (Ki * _dt_s));
#endif

        if (_mode) integral += delta_input * Kp;			// режим пропорционально скорости
        integral = constrain(integral, _minOut, _maxOut);	// ограничиваем инт. сумму
        output += integral;									// интегральная составляющая
        output = constrain(output, _minOut, _maxOut);		// ограничиваем выход
        return output;
    }

    // возвращает новое значение не ранее, чем через dt миллисекунд (встроенный таймер с периодом dt)
    datatype getResultTimer() {
//        if (millis() - pidTimer >= _dt) {
//            pidTimer = millis();
            getResult();
        //}
        return output;
    }


};

#endif
