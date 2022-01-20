#ifndef UVF_H
#define UVF_H

#include <iostream>
#include <algorithm>
#include <vector>
#include <cmath>
#include <QPoint>
#include <QObject>
#include <QImage>
#include <QString>
#include "staticdata.h"

#define PI 3.14159265358979323846

class StaticData;

using namespace std;

class UVF : public QObject
{
	Q_OBJECT
public:
	explicit UVF(QObject *parent = nullptr);
    StaticData *data;
	inline double constrain_angle(double x){
		x = fmod(x+PI,2*PI);
		if(x <= 0)
			x += 2*PI;
		return x - PI;
	}

	inline double get_gauss(double r, double delta){
		return exp(-(r*r)/(2*(delta*delta)));
	}
	vector<QPointF> get_obstacles(int team_id, int player_id, bool has_obstacles);
	double get_phi(QPointF origin, QPointF target, double target_ori, vector<QPointF> obstacles);
	double get_phih(QPointF p, double tx, double ty, double ccw);
	double get_angle(QPointF origin, QPointF target, double target_ori, int team_id, int player_id, bool has_obs);
    void setData(StaticData* data);
signals:

public slots:

private:

    double field_width = 1.5; //Modificado de 640 para 1.5 int para float
    double field_height = 1.3; //Modificado de 480 para 1.3 int para float
};

#endif // UVF_H
