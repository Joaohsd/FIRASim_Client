#ifndef PID_H
#define PID_H

#include <QObject>
#include <iostream>

class PID
{
public:
	PID(double *_input, double *_set_point);
	double process(double kp, double ki, double kd, double out_min, double out_max);

private:
	double PTerm = 0, ITerm = 0, DTerm = 0, PFactor = 0;
	int i_int = 0;
	double *input, output = 0, *set_point;
	double integrative[10];
	double error = 0, last_error = 0;

};

#endif // PID_H
