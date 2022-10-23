/*
 * pid.h
 *
 *  Created on: 1 сент. 2022 г.
 *      Author: akcel
 */

#ifndef REGULATOR_PID_H_
#define REGULATOR_PID_H_
#define _sat(A, Pos, Neg)     (((A) > (Pos)) ?                              \
                                 (Pos) :                                      \
                                 (((A) < (Neg)) ? (Neg) : (A)))

class pid {


	float Up;
	float Ui;
	float SatErr;
	float Kp;
	float Ki;
	float Kc;
	float Kd;
	float Max;
	float Min;

public:
	pid(){
		this->Ui = 0;
		this->Up = 0;
		this->SatErr = 0;
	}

	float calculatePid(bool R, float ref, float fb)
	{
	  float Up1, Ud, Out1;
	  float Out;
	  if (R)
	  {
	    Out    = 0;
	    this->Up     = 0;
	    this->Ui     = 0;
	    this->SatErr = 0;
	    return Out;
	  }

	  Up1       = this->Kp * (ref - fb);
	  this->Ui     = this->Ui + (this->Ki * Up1) + (this->Kc * this->SatErr);
	  Ud        = (this->Kd *  (Up1 - this->Up));
	  Out1      = Up1 + this->Ui + Ud;
	  Out    = _sat(Out1, this->Max, this->Min);
	  this->SatErr = Out - Out1;
	  this->Up     = Up1;
	  return Out;
	}

	void setCoef(float p, float i, float c, float d)
	{
		this->Kp = p;
		this->Ki = i;
		this->Kc = c;
		this->Kd = d;
	}

	void setMinMax(float min, float max)
	{
		this->Min = min;
		this->Max = max;
	}


};

#endif /* REGULATOR_PID_H_ */
