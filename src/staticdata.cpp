#include "staticdata.h"

int StaticData::speed = 500; //arrumar aqui

// PID
double StaticData::p = -3.0;
double StaticData::i = 0.0;
double StaticData::d = -75.0;

// Pathing UVF
double StaticData::k_de = 80.0;     //40     //82         // Define o raio máximo da curva a ser feita no trajeto, dada a posição da bola
double StaticData::k_kr = 77.5;     //30     //45
double StaticData::k_dmin = 40.0;   //37     //35         //distancia (raio) para repelir / distancia entre o jogador e o obstáculo
double StaticData::k_delta = 65.0;  //40     //65
double StaticData::angle_error = 10.0;

// Ball data
QPoint StaticData::ballPos = QPoint(-1550,1350);
QPointF StaticData::ballVel = QPoint(0.0,0.0);
QPointF StaticData::lastBallVel = QPoint(0.0,0.0);
QPointF StaticData::futureBallPos = QPoint(0.0,0.0);
QPoint StaticData::lastBallPos = QPoint(0.0,0.0);

// Player data
QPoint StaticData::player[][4];        // Position of each robot
double StaticData::player_angle[][4];   // Position of each robot

// Field dimensions
QPoint StaticData::min_field = QPoint(0,0);
QPoint StaticData::max_field = QPoint(1500,1300);
QPoint StaticData::middle_field = QPoint(750,650);
FieldSpace StaticData::field = FieldSpace(QPoint(0,1300),QPoint(1500,1300), QPoint(0,0), QPoint(1500,0)); // Change here
FieldSpace StaticData::area[2]; //0 -> Left side and 1 -> Right Side
FieldSpace StaticData::goal = FieldSpace(QPoint(0,850),QPoint(1500,850),QPoint(0,450),QPoint(1500,450));  // Change here

// Formations
bool StaticData::formation1 = false;
bool StaticData::formation2 = false;
bool StaticData::formation3 = false;
bool StaticData::formation4 = false;
bool StaticData::formation5 = false;
bool StaticData::formation6 = false;

// Game status and information about our team (wich side and color)
int StaticData::playStatus = 0;
int StaticData::playTeam = 0;
int StaticData::playSide = 0;

bool StaticData::penalty = false;
bool StaticData::defendPenalty = false;
int StaticData::contDefendPenalty = -1;

bool StaticData::goalKick = false;
int StaticData::contGoalKick = -1;

bool StaticData::freeBall = false;
int StaticData::contFreeBall = -1;

int StaticData::robotAction[] = {-1,-1,-1,-1,-1};
int StaticData::robotPenalty[] = {-1,-1,-1,-1,-1};
int StaticData::penaltyAction = 0;
double StaticData::speed_spin_increment = 3.0;

play_mode StaticData::mode = bola_ataque;

StaticData::StaticData()
{

}
