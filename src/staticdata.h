#ifndef STATICDATA_H
#define STATICDATA_H

#include "fieldspace.h"
#include "stdlib.h"

enum play_mode {
    bola_ataque,
    bola_meio,
    bola_defesa,
    bola_area
};

class StaticData
{
public:
    static QPoint ballPos, lastBallPos;
    static QPointF ballVel, lastBallVel, futureBallPos;
    static FieldSpace field, area[2], goal;
    static QPoint middle_field, max_field, min_field;
    static QPoint player[2][4];
	static double player_angle[2][4];
    static int robotAction[5];
    static int robotPenalty[5];
    static int penaltyAction;
    static bool formation1, formation2, formation3, formation4, formation5, formation6;
    static int playStatus, playSide, playTeam;
	static int speed;
	static double p, i, d, k_de, k_kr, k_dmin, k_delta;
    static double angle_error;
    static bool penalty;
    static bool defendPenalty;
    static int contDefendPenalty;
    static bool goalKick;
    static int contGoalKick;
    static bool freeBall;
    static int contFreeBall;
    static double speed_spin_increment;
    static play_mode mode;
	StaticData();
};

#endif // STATICDATA_H
