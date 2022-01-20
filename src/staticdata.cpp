#include "staticdata.h"

int StaticData::speed = 500; //arrumar aqui

// PID
double StaticData::p = -3.0;
double StaticData::i = 0.0;
double StaticData::d = -75.0;

// Pathing UVF
double StaticData::k_de = 0.08225;      //0.08225       // Define o raio máximo da curva a ser feita no trajeto, dada a posição da bola
double StaticData::k_kr = 0.045;        //0.045
double StaticData::k_dmin = 0.035;      //0.035         //distancia (raio) para repelir / distancia entre o jogador e o obstáculo
double StaticData::k_delta = 0.065;     //0.065         //distancia (raio) entre o jogador e a bola
double StaticData::angle_error = 10.0;


// Ball data
QPointF StaticData::ballPos = QPointF(-20,481);
QPointF StaticData::ballVel = QPointF(0.0,0.0);
QPointF StaticData::futureBallPos = QPointF(0.0,0.0);
QPointF StaticData::lastBallPos = QPointF(0.0,0.0);

// Player data
QPointF StaticData::player[][4];        // Position of each robot
double StaticData::player_angle[][4];   // Position of each robot

// Field dimensions
QPointF StaticData::min_field = QPointF(0,0);
QPointF StaticData::max_field = QPointF(1.5,1.3);
QPointF StaticData::middle_field = QPointF(0.75,0.65);
FieldSpace StaticData::field = FieldSpace(QPointF(0,1.30),QPointF(1.50,1.30), QPointF(0,0), QPointF(1.50,0)); // Change here
FieldSpace StaticData::area[2]; //0 -> Left side and 1 -> Right Side
FieldSpace StaticData::goal = FieldSpace(QPointF(0,0.85),QPointF(1.5,0.85),QPointF(0,0.45),QPointF(1.5,0.45));  // Change here

// Formations
bool StaticData::formation1 = false;
bool StaticData::formation2 = false;
bool StaticData::formation3 = false;
bool StaticData::formation4 = false;
bool StaticData::formation5 = false;
bool StaticData::formation6 = false;

// Game status and information about out team (wich side and color)
int StaticData::playTeam = 0;
int StaticData::playStatus = 0;
int StaticData::playSide = 0;
int StaticData::resetPosition = 0;

bool StaticData::penalty = false;

double StaticData::robot1_motor = 0.0;
double StaticData::robot2_motor = 0.0;
double StaticData::robot3_motor = 0.0;

double StaticData::openglTime = 0.0;
double StaticData::pathTime = 0.0;
double StaticData::delayTime = 0.0;

int StaticData::robotAction[] = {-1,-1,-1,-1,-1};
int StaticData::robotPenalty[] = {-1,-1,-1,-1,-1};
int StaticData::penaltyAction = 0;
double StaticData::speed_spin_increment = 3.0;

StaticData::StaticData()
{

}
