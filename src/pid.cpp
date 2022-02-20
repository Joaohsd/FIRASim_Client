#include "pid.h"
#include <QDebug>

PID::PID(double *_input, double *_set_point)
{
	input = _input;
	set_point = _set_point;
	PTerm = ITerm = DTerm = 0;
	error = last_error = 0;
	for(int i = 0; i < 10; ++i){
		integrative[i] = 0.0;
	}
}

double PID::process(double kp, double ki, double kd, double out_min, double out_max)
{
	error = *set_point - *input;
    /*std::cout << "ERRO: " << error << std::endl;
    std::cout << "SETPOINT: " << *set_point << std::endl;
    std::cout << "INPUT: " << *input << std::endl;*/
	/*
	if(-error <= 10){
		PFactor = 0.1;
	}
	else if(-error >= 11 && -error <= 30){
		PFactor = 0.2857;
	}
	else if(-error >= 31 && -error <= 50){
		PFactor = 0.4;
	}
	else if(-error >= 51 && -error <= 70){
		PFactor = 0.3429;
	}
	else if(-error >= 71 && -error <= 90){
		PFactor = 0.2857;
	}
	*/
	//PTerm = (kp) * error * PFactor;
	PTerm = (kp) * error;
	integrative[i_int] = (ki) * error;
	DTerm = (kd) * (error - last_error);
	ITerm = 0;
	for(int i = 0; i < 10; ++i){
		ITerm += integrative[i];
	}
	if(ITerm > out_max) ITerm = out_max;
	if(ITerm < out_min) ITerm = out_min;
	last_error = error;
	output = PTerm + ITerm + DTerm;
	if(output > out_max) output = out_max;
	if(output < out_min) output = out_min;
	i_int++;
	if(i_int == 10)
		i_int = 0;
	return output;
}
