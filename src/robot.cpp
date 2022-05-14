#ifndef ROBOT_CPP
#define ROBOT_CPP

#include "robot.h"

Robot::Robot(int _team_id, int _id, double _kp, double _ki, double _kd, double _base_speed, double _spin_speed){
    this->team_id = _team_id;
    this->player_id = _id;
    this->KP = _kp;
    this->KI = _ki;
    this->KD = _kd;
    this->base_speed = _base_speed;
    this->spin_speed = _spin_speed;
}

void Robot::printStartGOAL(){
    std::cout << "THREAD GOAL STARTED" << std::endl;
}

void Robot::printFinishGOAL(){
    std::cout << "THREAD GOAL FINISHED" << std::endl;
}

void Robot::printStartDEF(){
    std::cout << "THREAD DEF STARTED" << std::endl;
}

void Robot::printFinishDEF(){
    std::cout << "THREAD DEF FINISHED" << std::endl;
}

void Robot::printStartATT(){
    std::cout << "THREAD ATT STARTED" << std::endl;
}

void Robot::printFinishATT(){
    std::cout << "THREAD ATT FINISHED" << std::endl;
}

int Robot::getId(){
    return this->player_id;
}

int Robot::getTeamId(){
    return this->team_id;
}

int Robot::getX(){
    return this->x;
}

int Robot::getY(){
    return this->y;
}

int Robot::getVx(){
    return this->vx;
}

int Robot::getVy(){
    return this->vy;
}

double Robot::getVo(){
    return this->vo;
}

double Robot::getAngle(){
    return this->angle;
}

void Robot::setId(int player_id){
    this->player_id = player_id;
}

void Robot::setTeamId(int team_id){
    this->team_id = team_id;
    this->data->playTeam = team_id;
}

void Robot::setX(int x){
    this->x = x;
    this->pos.setX(x);
}

void Robot::setY(int y){
    this->y = y;
    this->pos.setY(y);
}

void Robot::setVx(int vx){
    this->vx = vx;
}

void Robot::setVy(int vy){
    this->vy = vy;
}

void Robot::setVo(double vo){
    this->vo = vo;
}

void Robot::setAngle(double angle){
    this->angle = angle;
}

void Robot::setActuator(ActuatorClient *command){
    this->actuator = command;
}

void Robot::setKD(double _KD){
    this->KD = _KD;
}

void Robot::setKP(double _KP){
    this->KP = _KP;
}

void Robot::setBaseSpeed(double _base_speed){
    this->base_speed = _base_speed;
}

void Robot::sendFIRA(int id, double wheelL, double wheelR){
    this->actuator->sendCommand(id,wheelL,wheelR);
}

void Robot::run(){
    switch(this->getId()) {
    case ID_2:
        this->playID_2();
        break;
    case ID_0:
        this->playID_0();
        break;
    default:
        this->playID_1();
        break;
    }
}

void Robot::playID_0(){
    if(this->data->formation1){ // 1-0-2
        //GOALKEEPER_LIBERO - GOALKEEPER_LIBERO - GOALKEEPER_LIBERO
        switch(data->mode){
        case bola_ataque:
            if(!data->defendPenalty && !data->goalKick)
                this->defend_goal_libero();
            break;
        case bola_defesa:
            if(!data->defendPenalty && !data->goalKick)
                this->defend_goal_libero();
        case bola_meio:
            if(!data->defendPenalty && !data->goalKick)
                this->defend_goal_libero();
            break;
        case bola_area:
            if(!data->defendPenalty && !data->goalKick)
                this->defend_goal_libero();
            break;
        }
    }
    else if(this->data->formation2){
        //GOALKEEPER - GOALKEEPER - GOALKEEPER
        switch(data->mode){
        case bola_ataque:
            if(!data->defendPenalty && !data->goalKick)
                this->defend_goal_safe();
            break;
        case bola_meio:
            if(!data->defendPenalty && !data->goalKick)
                this->defend_goal_safe();
            break;
        case bola_defesa:
            if(!data->defendPenalty && !data->goalKick)
                this->defend_goal_safe();
            break;
        case bola_area:
            if(!data->defendPenalty && !data->goalKick)
                this->defend_goal_safe();
            break;
        }
    }
    else if(this->data->formation3){
        //GOALKEEPER - GOALKEEPER - GOALKEEPER
        switch(data->mode){
        case bola_ataque:
            if(!data->defendPenalty && !data->goalKick)
                this->defend_goal_safe();
            break;
        case bola_meio:
            if(!data->defendPenalty && !data->goalKick)
                this->defend_goal_safe();
            break;
        case bola_defesa:
            if(!data->defendPenalty && !data->goalKick)
                this->defend_goal_safe();
            break;
        case bola_area:
            if(!data->defendPenalty && !data->goalKick)
                this->defend_goal_safe();
            break;
        }
    }
    else if(this->data->formation4){
        //GOALKEEPER - GOALKEEPER - GOALKEEPER
        switch(data->mode){
        case bola_ataque:
            if(!data->defendPenalty && !data->goalKick)
                this->defend_goal_safe();
            break;
        case bola_meio:
            if(!data->defendPenalty && !data->goalKick)
                this->defend_goal_safe();
            break;
        case bola_defesa:
            if(!data->defendPenalty && !data->goalKick)
                this->defend_goal_safe();
            break;
        case bola_area:
            if(!data->defendPenalty && !data->goalKick)
                this->defend_goal_safe();
            break;
        }
    }
    else if(this->data->formation5){
        //GOALKEEPER - GOALKEEPER - GOALKEEPER
        switch(data->mode){
        case bola_ataque:
            if(!data->defendPenalty && !data->goalKick)
                this->defend_goal_safe();
            break;
        case bola_meio:
            if(!data->defendPenalty && !data->goalKick)
                this->defend_goal_safe();
            break;
        case bola_defesa:
            if(!data->defendPenalty && !data->goalKick)
                this->defend_goal_safe();
            break;
        case bola_area:
            if(!data->defendPenalty)
                this->defend_goal_safe();
            break;
        }
    }
    else if(this->data->formation6){
        //GOALKEEPER - GOALKEEPER - GOALKEEPER
        switch(data->mode){
        case bola_ataque:
            this->attack_antigo();
            break;
        case bola_meio:
            this->attack_antigo();
        case bola_defesa:
            this->attack_antigo();
            break;
        case bola_area:
            this->attack_antigo();
            break;
        }
    }
}

void Robot::playID_1(){
    if(this->data->formation1){ // 1-0-2
        //STRICKER - DEFENDER - CONTAINING
        switch(data->mode){
        case bola_ataque:
            this->attack();
            break;
        case bola_meio:
            this->intercept();
            break;
        case bola_defesa:
            this->defend();
            break;
        case bola_area:
            this->defend_middle();
            break;
        }
    }
    else if(this->data->formation2){
        //CONTAINING - DEFENDER - CONTAINING
        switch(data->mode){
        case bola_ataque:
            if(!data->penalty)
                this->attack();
            break;
        case bola_meio:
            this->intercept();
            break;
        case bola_defesa:
            this->defend();
            break;
        case bola_area:
            this->defend_middle();
            break;
        }
    }
    else if(this->data->formation3){
        //CONTAINING - DEFENDER - CONTAINING
        switch(data->mode){
        case bola_ataque:
            this->defend_middle_attack();
            break;
        case bola_meio:
            this->intercept();
            break;
        case bola_defesa:
            this->defend();
            break;
        case bola_area:
            this->defend_middle();
            break;
        }
    }
    else if(this->data->formation4){
        //CONTAINING - DEFENDER - CONTAINING
        switch(data->mode){
        case bola_ataque:
            this->attack_antigo();
            break;
        case bola_meio:
            this->intercept();
            break;
        case bola_defesa:
            this->defend();
            break;
        case bola_area:
            this->defend_middle();
            break;
        }
    }
    else if(this->data->formation5){
        //CONTAINING - DEFENDER - CONTAINING
        switch(data->mode){
        case bola_ataque:
            this->defend_middle_attack();
            break;
        case bola_meio:
            this->intercept();
            break;
        case bola_defesa:
            this->defend();
            break;
        case bola_area:
            this->defend_middle();
            break;
        }
    }
    else if(this->data->formation6){
        //GOALKEEPER - GOALKEEPER - GOALKEEPER
        switch(data->mode){
        case bola_ataque:
            this->stopRobot();
            break;
        case bola_meio:
            this->stopRobot();
        case bola_defesa:
            this->stopRobot();
            break;
        case bola_area:
            this->stopRobot();
            break;
        }
    }
}

void Robot::playID_2(){
    if(this->data->formation1){ // 1-0-2
        //STRICKER - DEFENDER - CONTAINING
        switch(data->mode){
        case bola_ataque:
            if(!data->penalty)
                this->attack();
            break;
        case bola_meio:
            this->attack_antigo();
            break;
        case bola_defesa:
            this->defend();
            break;
        case bola_area:
            this->defend_middle_attack();
            break;
        }
    }
    else if(this->data->formation2){
        //STRICKER - DEFENDER - CONTAINING
        switch(data->mode){
        case bola_ataque:
            if(!data->penalty)
                this->attack();
            break;
        case bola_meio:
            this->attack_antigo();
            break;
        case bola_defesa:
            this->defend();
            break;
        case bola_area:
            this->defend_middle_attack();
            break;
        }
    }
    else if(this->data->formation3){
        //STRICKER - DEFENDER - CONTAINING
        switch(data->mode){
        case bola_ataque:
            if(!data->penalty)
                this->attack();
            break;
        case bola_defesa:
            this->defend();
            break;
        case bola_area:
            this->defend_middle_attack();
            break;
        }
    }
    else if(this->data->formation4){
        //STRICKER - DEFENDER - CONTAINING
        switch(data->mode){
        case bola_ataque:
            if(!data->penalty)
                this->attack_antigo();
            break;
        case bola_meio:
            this->attack_antigo();
            break;
        case bola_defesa:
            this->defend();
            break;
        case bola_area:
            this->defend_middle_attack();
            break;
        }
    }
    else if(this->data->formation5){
        //STRICKER - DEFENDER - CONTAINING
        switch(data->mode){
        case bola_ataque:
            if(!data->penalty)
                this->attack_antigo();
            break;
        case bola_defesa:
            this->defend();
            break;
        case bola_area:
            this->defend_middle_attack();
            break;
        }
    }
    else if(this->data->formation6){
        //GOALKEEPER - GOALKEEPER - GOALKEEPER
        switch(data->mode){
        case bola_ataque:
            this->stopRobot();
            break;
        case bola_meio:
            this->stopRobot();
        case bola_defesa:
            this->stopRobot();
            break;
        case bola_area:
            this->stopRobot();
            break;
        }
    }
}

void Robot::attack()
{
    enum play_mode {att_normal, att_borda,att_danger} mode = att_normal;
    int min_ball_player_dist_to_go_ball = 400;
    if(is_inside(data->ballPos, this->data->area[RIGHT_SIDE]) && this->data->playSide == LEFT_SIDE){
        mode = att_danger;
    }

    else if(is_inside(data->ballPos, this->data->area[LEFT_SIDE]) && this->data->playSide == RIGHT_SIDE){
        mode = att_danger;
    }

    else if(border_y()){
        mode = att_borda;
    }

    else{
        mode = att_normal;
    }

    switch(mode){
    case att_borda:
        if(verify_Player_Best_Condition(ID_1,ID_2) == this->getId())
            this->intercept_antigo();
        else {
            if(distance(data->ballPos, pos) > min_ball_player_dist_to_go_ball)
                containing();
            else this->intercept_antigo();
        }
        break;
    case att_normal:
        if(verify_Player_Best_Condition(ID_1,ID_2) == this->getId())
            this->kick();
        else {
            if(distance(data->ballPos, pos) > min_ball_player_dist_to_go_ball)
                containing();
            else this->kick();
        }
        break;
    case att_danger:
        if(verify_Player_Best_Condition(ID_1,ID_2) == this->getId())
            this->kick();
        else this->defend_attack();
        break;
    }
}

void Robot::attack_antigo()
{
    enum play_mode {att_normal, att_borda,att_danger} mode = att_normal;

    if(border_y()){
        mode = att_borda;
    }

    else{
        mode = att_normal;
    }

    switch(mode){
    case att_borda:
        intercept_antigo();
        break;
    case att_normal:
        this->kick();
        break;
    case att_danger:
        if(verify_Player_Best_Condition(ID_1,ID_2) == this->getId())
            this->kick();
        else this->defend_attack();
        break;
    }
}

void Robot::containing(){
    if(data->mode == bola_ataque){
        int desloc_y = 110;
        int ball_Player_xDist = 350; //350
        if(data->playSide == LEFT_SIDE){
            int x_Max = 1000;
            int x_Min = 350;
            int x_Point = data->ballPos.x() - ball_Player_xDist;
            if(x_Point < x_Min)
                x_Point = x_Min;
            else if(x_Point > x_Max)
                x_Point = x_Max;
            if(data->ballPos.y() >= data->middle_field.y()){
                QPoint containingPoint = QPoint(x_Point, data->middle_field.y() + desloc_y);
                this->position(containingPoint, 0, 1, 1);
            }
            else{
                QPoint containingPoint = QPoint(x_Point, data->middle_field.y() - desloc_y);
                this->position(containingPoint, 0, 1, 1);
            }
        }
        else{
            int x_Max = 500; //600
            int x_Min = 1150;
            int x_Point = data->ballPos.x() + ball_Player_xDist;
            if(x_Point < x_Max)
                x_Point = x_Max;
            else if(x_Point > x_Min)
                x_Point = x_Min;
            if(data->ballPos.y() >= data->middle_field.y()){
                QPoint containingPoint = QPoint(x_Point, data->middle_field.y() + desloc_y);
                this->position(containingPoint, 0, 1, 1);
            }
            else{
                QPoint containingPoint = QPoint(x_Point, data->middle_field.y() - desloc_y);
                this->position(containingPoint, 0, 1, 1);
            }
        }
    }
}

void Robot::kick()
{
    double min_dist_ball_y = 180;
    double max_dist_ball_y = 520;
    //double min_distance_kickable = 70;
    //Minimum distance threshold for entering kick mode
    double min_threshold = 160;
    //Minimum angle diference threshold for entering kick mode
    double angle_threshold = 10;
    //Maximum angle diference threshold for leaving kick mode
    double max_angle_threshold = 50;
    //Maximum distance threshold for leaving kick mode
    double max_threshold = 270;

    QPoint med_goal2;

    double ball_player_angle = fabs(get_angle(QPoint(120*cos(angle),120*sin(angle)),QPoint(data->ballPos - pos)))*180/PI;
    if(ball_player_angle >= 90.0){
        ball_player_angle = 180.0 - ball_player_angle;
    }

    //Kick behaviour
    if(data->playSide == LEFT_SIDE){
        med_goal2 = medium_qpoint(data->goal.getTopRight(),data->goal.getBottomRight());
            if(this->distance(this->pos,this->data->ballPos) <= min_threshold && this->pos.x() < this->data->ballPos.x() && ball_player_angle <= angle_threshold && this->data->ballPos.x() > this->data->middle_field.x()){
                this->speed_kick_current = this->speed_kick_base;
                this->kick_stat = true;
            }
            else if(((this->distance(pos,data->ballPos) >= max_threshold) || ball_player_angle > max_angle_threshold) && kick_stat == true){
                this->kick_stat = false;
            }
            if(this->kick_stat == false){
                if(this->data->ballPos.y() <= this->data->middle_field.y() + min_dist_ball_y && this->data->ballPos.y() >= this->data->middle_field.y() - min_dist_ball_y){ // Center
                    //cout << "meio" << endl;
                    this->go_to(QPoint(this->data->futureBallPos.x(), this->data->futureBallPos.y()), PI, 1, 3); //PI
                }
                else if((this->data->ballPos.y() <= (this->data->middle_field.y() + max_dist_ball_y)) && (this->data->ballPos.y() >= (this->data->middle_field.y() - max_dist_ball_y))){ //External
                    if(this->data->ballPos.y() > this->data->middle_field.y()){ // External up
                        //cout << "em cima meio" << endl;
                        this->go_to(QPoint(this->data->futureBallPos.x(), this->data->futureBallPos.y()), 3*PI/4.0, 1, 3);//3*PI/4.0
                    }
                    else{   // External down
                        //cout << "baixo meio" << endl;
                        this->go_to(QPoint(this->data->futureBallPos.x(), this->data->futureBallPos.y()), -3*PI/4.0, 1, 3);//-3*PI/4.0
                    }
                }
                else{
                    this->intercept();
                }
            }
            else{
                QPoint med_goal = medium_qpoint(this->data->goal.getTopRight(),this->data->goal.getBottomRight());
                QPoint vec_ball_goal = QPoint(med_goal.x() - this->data->ballPos.x(), med_goal.y() - this->data->ballPos.y());
                //go_to(QPoint(data->ballPos.x() + vec_ball_goal.x(),data->ballPos.y() + vec_ball_goal.y()), -999, 0, 5);
                this->go_to_kick(QPoint(this->data->ballPos.x() + vec_ball_goal.x(), this->data->ballPos.y() + vec_ball_goal.y()), min(this->speed_kick_current, this->speed_kick_max));
                this->speed_kick_current += this->speed_kick_increment;
                //go_to_debug();
            }
    }
    else{
        med_goal2 = medium_qpoint(data->goal.getTopLeft(),data->goal.getBottomLeft());
        //QPoint med_goal2 = medium_qpoint(data->goal.getTopLeft(),data->goal.getBottomLeft());
        if(this->distance(this->pos,this->data->ballPos) <= min_threshold && this->pos.x() > this->data->ballPos.x() && ball_player_angle <= angle_threshold && this->data->ballPos.x() < this->data->middle_field.x()){
            this->kick_stat = true;
        }
        else if(((this->distance(this->pos,this->data->ballPos) >= max_threshold) || (ball_player_angle > max_angle_threshold)) && kick_stat == true){
            this->kick_stat = false;
        }
        if(this->kick_stat == false){
            if(this->data->ballPos.y() <= this->data->middle_field.y() + min_dist_ball_y && this->data->ballPos.y() >= this->data->middle_field.y() - min_dist_ball_y){
                //cout << "meio" << endl;
                this->go_to(QPoint(this->data->futureBallPos.x(), this->data->futureBallPos.y()), 0, 1, 3);
            }
            else if(this->data->ballPos.y() <= this->data->middle_field.y() + max_dist_ball_y && this->data->ballPos.y() >= this->data->middle_field.y() - max_dist_ball_y){
                if(this->data->ballPos.y() > this->data->middle_field.y()){
                    //cout << "em cima dent" << endl;
                    this->go_to(QPoint(this->data->futureBallPos.x(), this->data->futureBallPos.y()), PI/4.0, 1, 3);
                }
                else{
                    //cout << "baixo dent" << endl;
                    this->go_to(QPoint(this->data->futureBallPos.x(), this->data->futureBallPos.y()), -PI/4.0, 1, 3);
                }
            }
            else{
                this->intercept();
            }
        }
        else{
            QPoint med_goal = medium_qpoint(this->data->goal.getTopLeft(), this->data->goal.getBottomLeft());
            QPoint vec_ball_goal = QPoint(med_goal.x() - this->data->ballPos.x(), med_goal.y() - this->data->ballPos.y());
            //go_to(QPoint(data->ballPos.x() + vec_ball_goal.x(),data->ballPos.y() + vec_ball_goal.y()), -999, 0, 5);
            this->go_to_kick(QPoint(this->data->ballPos.x() + vec_ball_goal.x(),this->data->ballPos.y() + vec_ball_goal.y()), min(this->speed_kick_current, this->speed_kick_max));
            this->speed_kick_current += this->speed_kick_increment;
        }
    }
}

void Robot::midfielder(){
    if(data->playSide == LEFT_SIDE){
        if((pos.x() + 200) < data->ballPos.x()){
            /*this->defend_midfielder++;
            if(this->defend_midfielder >= 120)
                this->attack_antigo();
            else*/
                this->attack_antigo();
        }
        else{
            //this->defend_midfielder = 0;
            this->intercept();
        }
    }else{
        if((pos.x() - 200) > data->ballPos.x()){
            /*this->defend_midfielder++;
            if(this->defend_midfielder >= 120)
                this->attack_antigo();
            else*/
                this->attack_antigo();
        }
        else{
            //this->defend_midfielder = 0;
            this->intercept();
        }
    }
}

void Robot::defend(){
    int desloc_y = 225;
    int desloc_x = 130;
    if(verify_Player_Best_Condition(ID_2, ID_1) == this->getId()){
        this->intercept_antigo();
    }
    else{
        QPoint containing_point;
        if(data->playSide == LEFT_SIDE){
            if(data->ballPos.y() >= data->middle_field.y()){
                if(data->ballPos.y() > data->middle_field.y() + desloc_y){
                    containing_point = QPoint(data->middle_field.x()/2 - desloc_x,data->middle_field.y() + desloc_y);
                }
                else{
                    containing_point = QPoint(data->middle_field.x()/2 - desloc_x,data->ballPos.y());
                }
            }
            else{
                if(data->ballPos.y() < data->middle_field.y() - desloc_y){
                    containing_point = QPoint(data->middle_field.x()/2 - desloc_x,data->middle_field.y() - desloc_y);
                }
                else{
                    containing_point = QPoint(data->middle_field.x()/2 - desloc_x,data->ballPos.y());
                }
            }
        }
        else{
            if(data->ballPos.y() >= data->middle_field.y()){
                if(data->ballPos.y() > data->middle_field.y() + desloc_y){
                    containing_point = QPoint(data->middle_field.x() + data->middle_field.x()/2 + desloc_x,data->middle_field.y() + desloc_y);
                }
                else{
                    containing_point = QPoint(data->middle_field.x() + data->middle_field.x()/2 + desloc_x,data->ballPos.y());
                }
            }
            else{
                if(data->ballPos.y() < data->middle_field.y() - desloc_y){
                    containing_point = QPoint(data->middle_field.x() + data->middle_field.x()/2 + desloc_x,data->middle_field.y() - desloc_y);
                }
                else{
                    containing_point = QPoint(data->middle_field.x() + data->middle_field.x()/2 + desloc_x,data->ballPos.y());
                }
            }
        }
        this->position(containing_point, PI/2, 1, 1);
    }

}

/*void Robot::defend(){
    QPoint containing_point;
    if(distance(data->player[data->playTeam][ID_2], data->ballPos) <= distance(data->player[data->playTeam][ID_1], data->ballPos)){
        if(this->getId() == ID_2){
            this->intercept();
        }
        else{
            if(data->playSide == LEFT_SIDE){
                if(data->ballPos.y() >= data->middle_field.y()){
                    containing_point = QPoint(data->middle_field.x()/2 - 100,data->middle_field.y() + 100);
                }
                else{
                    containing_point = QPoint(data->middle_field.x()/2 - 100,data->middle_field.y() - 100);
                }
            }
            else{
                if(data->ballPos.y() >= data->middle_field.y()){
                    containing_point = QPoint(data->middle_field.x() + data->middle_field.x()/2 + 100,data->middle_field.y() + 100);
                }
                else{
                    containing_point = QPoint(data->middle_field.x() + data->middle_field.x()/2 + 100,data->middle_field.y() - 100);
                }
            }
            this->position(containing_point, PI/2, 1, 1);
        }
    }
    else{
        if(this->getId() == ID_2){
            if(data->playSide == LEFT_SIDE){
                if(data->ballPos.y() >= data->middle_field.y()){
                    containing_point = QPoint(data->middle_field.x()/2 - 100,data->middle_field.y() + 100);
                }
                else{
                    containing_point = QPoint(data->middle_field.x()/2 - 100,data->middle_field.y() - 100);
                }
            }
            else{
                if(data->ballPos.y() >= data->middle_field.y()){
                    containing_point = QPoint(data->middle_field.x() + data->middle_field.x()/2 + 100,data->middle_field.y() + 100);
                }
                else{
                    containing_point = QPoint(data->middle_field.x() + data->middle_field.x()/2 + 100,data->middle_field.y() - 100);
                }
            }
            this->position(containing_point, PI/2, 1, 1);
        }
        else {
            this->intercept();
        }
    }
}*/

void Robot::intercept_antigo()
{
    double min_pos_dist = 80;
    int speed = 7;
    bool entrou = false;
    bool borda = false;
    int min_dist = 80;
    int y = data->ballPos.y();

    if(y >= data->max_field.y() - min_dist || y <= data->min_field.y() + min_dist)
        borda = true;
    else
        borda = false;


    if(distance(pos, data->ballPos) <= min_pos_dist){
        if(border_x()){
            spin_x();
        }
        else{
            spin_ataque();
            entrou = true;
        }
    }
    else{
        if(data->playSide == LEFT_SIDE){
            if(border_x()){
                if(data->ballPos.x() < data->middle_field.x()){
                    if(data->ballPos.y() > data->middle_field.y()){
                        go_to(data->ballPos, -PI/4, 1, speed);
                    }
                    else{
                        go_to(data->ballPos, PI/4, 1, speed);
                    }
                }
                else{
                    if(data->ballPos.y() > data->middle_field.y()){
                        go_to(data->ballPos, 3*PI/4, 1, speed);
                    }
                    else{
                         go_to(data->ballPos, -3*PI/4, 1, speed);
                    }
                }
            }
            else if(!borda){
                //cout << "na borda" << endl;
                if(data->ballPos.y() > data->middle_field.y()){
                    go_to(data->ballPos, -3*PI/4, 1, speed);
                }
                else{
                    go_to(data->ballPos, 3*PI/4, 1, speed);
                }
            }
            else if(data->ballPos.y() > pos.y()){
                //cout << "parte de cima" << endl;
                go_to(data->ballPos, -3*PI/4, 1, speed);
            }
            else{
                //cout << "parte de baixo" << endl;
                go_to(data->ballPos, 3*PI/4, 1, speed);
            }
        }
        //lado direito
        else{
            if(border_x()){
                if(data->ballPos.x() > data->middle_field.x()){
                    if(data->ballPos.y() > data->middle_field.y()){
                        go_to(data->ballPos, -3*PI/4, 1, speed);
                    }
                    else{
                        go_to(data->ballPos, 3*PI/4, 1, speed);
                    }
                }
                else{
                    if(data->ballPos.y() > data->middle_field.y()){
                        go_to(data->ballPos, PI/4, 1, speed);
                    }
                    else{
                        go_to(data->ballPos, -PI/4, 1, speed);
                    }
                }
            }
            else if(!borda){
                if(data->ballPos.y() > data->middle_field.y()){
                    go_to(data->ballPos, -PI/4, 1, speed);
                }
                else{
                    go_to(data->ballPos, PI/4, 1, speed);
                }
            }
            else if(data->ballPos.y() > pos.y()){
                go_to(data->ballPos, -PI/4, 1, speed);
            }
            else{
                go_to(data->ballPos, PI/4, 1, speed);
            }
        }
    }
}

/**
 * @brief Intercept the ball and spin
 */

void Robot::intercept()
{
    double min_pos_dist = 80;
    int speed = 7;
    QPoint go_to_position = QPoint(data->futureBallPos.x(), data->futureBallPos.y());
    //cout << "INTERCEPT" << endl;

    if(distance(pos, data->ballPos) <= min_pos_dist){
        if(border_x()){
            spin_x();
        }
        else{
            spin();
        }
    }
    else{
        if(data->playSide == LEFT_SIDE){
            if(border_x()){
                if(data->ballPos.x() < data->middle_field.x()){
                    if(data->ballPos.y() > data->middle_field.y()){
                        go_to(go_to_position, -PI/4, 1, speed);
                    }
                    else{
                        go_to(go_to_position, PI/4, 1, speed);
                    }
                }
                else{
                    if(data->ballPos.y() > data->middle_field.y()){
                        go_to(go_to_position, 3*PI/4, 1, speed);
                    }
                    else{
                        go_to(go_to_position, -3*PI/4, 1, speed);
                    }
                }
            }
            else if(border_y()){
                if(data->ballPos.y() > data->middle_field.y()){
                    go_to(go_to_position, -3*PI/4, 1, speed);
                }
                else{
                    go_to(go_to_position, 3*PI/4, 1, speed);
                }
            }
            else if((data->ballPos.y() - 20) > pos.y()){
                go_to(go_to_position, -3*PI/4, 1, speed);
            }
            else{
                go_to(go_to_position, 3*PI/4, 1, speed);
            }
        }
        //lado direito
        else{
            if(border_x()){
                if(data->ballPos.x() > data->middle_field.x()){
                    if(data->ballPos.y() > data->middle_field.y()){
                        go_to(go_to_position, -3*PI/4, 1, speed);
                    }
                    else{
                        go_to(go_to_position, 3*PI/4, 1, speed);
                    }
                }
                else{
                    if(data->ballPos.y() > data->middle_field.y()){
                        go_to(go_to_position, PI/4, 1, speed);
                    }
                    else{
                        go_to(go_to_position, -PI/4, 1, speed);
                    }
                }
            }
            else if(border_y()){
                if(data->ballPos.y() > data->middle_field.y()){
                    go_to(go_to_position, -PI/4, 1, speed);
                }
                else{
                    go_to(go_to_position, PI/4, 1, speed);
                }
            }
            else if((data->ballPos.y() - 20) > pos.y()){
                go_to(go_to_position, -PI/4, 1, speed);
            }
            else{
                go_to(go_to_position, PI/4, 1, speed);
                }
            }
        }
}

/**
 * @brief Robot::go_to
 * @param target Target point
 * @param target_angle Target angle:	-999 - Fast mode without obstacle avoidance
 * @param have_obstacle Boolean for obstacle avoidance
 * @param decay_mode Decay mode:		0 - Normal (Faster)
 *										1 - Slow
 *										2 - Medium
 *										3 - Fast
 */

void Robot:: go_to(QPointF target, double target_angle, bool have_obstacle, int decay_mode)
{
    double kp_ = this->KP;
    double ki_ = this->KI;
    double kd_ = this->KD;
    double out_min = -this->base_speed;
    double out_max = this->base_speed;

    if(target_angle == -999){
        input = get_angle(QPointF(60*cos(angle),60*sin(angle)),QPointF(target-pos))*180/PI;
    }
    else{
        phi = path.get_angle(pos,target,target_angle,this->team_id,this->player_id,have_obstacle);

        if(phi > 2*PI){
            phi -= 2*PI;
        }
        else if(phi < -2*PI){
            phi += 2*PI;
        }

        //Da um passo com o robo a partir do angulo fornecido pelo uvf
        fut_pos.setX(pos.x() + 60*cos(phi));
        fut_pos.setY(pos.y() + 60*sin(phi));
        //O input é o ângulo entre o vetor orientação do robo e o vetor direção da trajetória a ser feita (fut_pos - pos)
        input = get_angle(QPoint(60*cos(angle),60*sin(angle)),QPoint(fut_pos - pos))*180/PI;
    }
    curr_speed = base_speed;
    if(decay_mode == 5){
        //cout << "decay_mode 5" << endl;
        kp_ *= 0.8;
        ki_ *= 0.6;
        kd_ *= 0.9;
    }
    else if(decay_mode == 1){
        //cout << "decay_mode 1" << endl;
        double min_goto_dist = 320; //320
        double min_goto_dist_2 = 520; //520
        double min_goto_fact = 0.7;     //CONFERIR
        double max_goto_fact= 1.0;      //CONFERIR
        if(distance(pos,target) < min_goto_dist_2){
            curr_speed *= distance(pos,target)/min_goto_dist_2;
            if(curr_speed < max_goto_fact * this->base_speed){
                curr_speed = max_goto_fact * this->base_speed;
            }
        }
        else if(distance(pos,target) < min_goto_dist){
            curr_speed *= distance(pos,target)/min_goto_dist;
            if(curr_speed < min_goto_fact * this->base_speed){
                curr_speed = min_goto_fact * this->base_speed;
            }
        }
    }
    else if(decay_mode == 2){
        //cout << "decay_mode 2" << endl;
        double min_dist = 250; //0.25
        if(distance(pos,target) <= min_dist){
            curr_speed *= 500;
            kp_ *= 0.6;
            ki_ *= 0.6;
            kd_ *= 0.8;
        }
    }
    else if(decay_mode == 3){
        //cout << "decay_mode 3" << endl;
        double min_dist = 300; //0.3
        double min_min_dist = 180;//0.18
        if(distance(pos,target) <= min_min_dist){
            //cout << "PERTO" << endl;
            curr_speed *= 0.7; //0.7
            kp_ *= 0.7; //0.7
            ki_ *= 0.8; //0.7
            kd_ *= 0.6; //0.6
        }
        else if(distance(pos,target) <= min_dist){
            curr_speed *= 0.8; //0.8
            //cout << "LONGE" << endl;
            kp_ *= 0.8; //0.8
            ki_ *= 0.8; //0.8
            kd_ *= 0.8; //0.8
        }
    }
    else if(decay_mode == 4){
        double min_dist = 120; //0.15
        if(distance(pos,target) <= min_dist){
            //cout << "decay_mode 4" << endl;
            curr_speed *= 0.85;
            kp_ *= 0.85;
            ki_ *= 0.85;
            kd_ *= 0.85;
        }
    }
    else if(decay_mode == 6){
        curr_speed = this->base_speed;
        //cout << "decay_mode 6" << endl;
        if (distance(data->ballPos,data->middle_field)<distance(data->ballPos,pos)){
            curr_speed *= 0.6;
        }
    }
    else if(decay_mode == 7){
        double min_dist = 325;  //0.27
        //cout << "decay_mode 7" << endl;
        if(distance(pos,target) <= min_dist){
            //curr_speed *= 1.0; //
            curr_speed *= 0.9; //75
            kp_ *= 0.95;
            ki_ *= 0.7;
            kd_ *= 0.9;
        }
    }

    out_min = -curr_speed;
    out_max = curr_speed;

    if(input >= 90.0){
        input = 180.0 - input;
        output = path_pid.process(kp_, ki_, kd_, out_min, out_max);
        speed.first = -1.0*curr_speed + output;
        speed.second = -1.0*curr_speed;
    }
    else if(input <= 0 && input >= -90){
        input = -1.0*input;
        output = path_pid.process(kp_, ki_, kd_, out_min, out_max);
        speed.first = curr_speed;
        speed.second = curr_speed - output;
    }
    else if(input < -90){
        input = 180 + input;
        output = path_pid.process(kp_, ki_, kd_, out_min, out_max);
        speed.first = -1.0*curr_speed;
        speed.second = -1.0*curr_speed + output;
    }
    else{
        output = path_pid.process(kp_, ki_, kd_, out_min, out_max);
        speed.first = curr_speed - output;
        speed.second = curr_speed;
    }

    this->sendFIRA(this->getId(),speed.first,speed.second);
}

void Robot::go_to_debug()
{
    double kp_ = this->KP;
    double ki_ = this->KI;
    double kd_ = this->KD;
    double out_min = -this->base_speed;
    double out_max = this->base_speed;
    curr_speed = this->base_speed;
    this->input = get_angle(QPoint(1000*cos(angle),1000*sin(angle)),QPoint(data->ballPos-pos))*180/PI;
    if(input >= 90.0){
        input = 180.0 - input;
        output = path_pid.process(kp_, ki_, kd_, out_min, out_max);
        speed.first = -1.0*curr_speed + output;
        speed.second = -1.0*curr_speed;
    }
    else if(input <= 0 && input >= -90){
        input = -1.0*input;
        output = path_pid.process(kp_, ki_, kd_, out_min, out_max);
        speed.first = curr_speed;
        speed.second = curr_speed - output;
    }
    else if(input < -90){
        input = 180 + input;
        output = path_pid.process(kp_, ki_, kd_, out_min, out_max);
        speed.first = -1.0*curr_speed;
        speed.second = -1.0*curr_speed + output;
    }
    else{
        output = path_pid.process(kp_, ki_, kd_, out_min, out_max);
        speed.first = curr_speed - output;
        speed.second = curr_speed;
    }
    this->sendFIRA(this->getId(),speed.first,speed.second);
}

void Robot::go_to_kick(QPoint target, double speed_factor)
{
    double kp_ = this->KP * (speed_factor);
    double ki_ = this->KI * speed_factor;
    double kd_ = this->KD * speed_factor;
    double curr_speed = this->base_speed * speed_factor;
    //cout << curr_speed << endl;
    double out_min = -this->base_speed * speed_factor;
    double out_max = this->base_speed * speed_factor;

    input = get_angle(QPoint(1000*cos(this->angle),1000*sin(this->angle)),QPoint(target-this->pos))*180/PI;

    if(input >= 90.0){
        input = 180.0 - input;
        output = path_pid.process(kp_, ki_, kd_, out_min, out_max);
        speed.first = -1.0*curr_speed + output;
        speed.second = -1.0*curr_speed;
    }
    else if(input <= 0 && input >= -90){
        input = -1.0*input;
        output = path_pid.process(kp_, ki_, kd_, out_min, out_max);
        speed.first = curr_speed;
        speed.second = curr_speed - output;
    }
    else if(input < -90){
        input = 180 + input;
        output = path_pid.process(kp_, ki_, kd_, out_min, out_max);
        speed.first = -1.0*curr_speed;
        speed.second = -1.0*curr_speed + output;
    }
    else{
        output = path_pid.process(kp_, ki_, kd_, out_min, out_max);
        speed.first = curr_speed - output;
        speed.second = curr_speed;
    }

    this->sendFIRA(this->getId(),speed.first,speed.second);
}


void Robot::go_to_penalty(){
    speed.first = 20000;
    speed.second = 20000;
    this->sendFIRA(this->getId(),speed.first,speed.second);
}

void Robot::penalty_rebound(){
    if(data->playSide == LEFT_SIDE){
        this->go_to(data->futureBallPos,3*PI/4.0,1,3);
    }
    else{
        this->go_to(data->futureBallPos,PI/4.0,1,3);
    }
}

void Robot::stopRobot(){
    this->sendFIRA(this->getId(),0,0);
}

void Robot::defend_goal_safe(){
    if(data->playSide == LEFT_SIDE){
        if(distance(pos, data->ballPos) > 100){
            defend_goalLine_Left();
        }
        else{
            intercept_goalkeeper(false);
        }
    }
    else{
        if(distance(pos, data->ballPos) > 100)
            defend_goalLine_Right();
        else {
            intercept_goalkeeper(false);
        }
    }
}

void Robot::defend_goal_libero(){
    int x_distance_goal = 200;
    int ball_center_dist = 600;

    int y_top = 1050;
    int y_bot = 250;
    int y_center = 650;

    QPoint middle_top = QPoint(data->middle_field.x(),data->middle_field.y() + 200);
    QPoint middle_bot = QPoint(data->middle_field.x(),data->middle_field.y() - 200);

    if(this->data->playSide == LEFT_SIDE){  //left side
        QPoint top_left = QPoint(data->min_field.x()+x_distance_goal, y_top - 200);
        QPoint center_left = QPoint(data->min_field.x()+x_distance_goal, y_center);
        QPoint bot_left = QPoint(data->min_field.x()+x_distance_goal, y_bot + 200);
        if(data->ballPos.x() <= 350 && data->ballPos.y() <= 200){        //corner bot
            QPoint secure_point = QPoint(bot_left.x() - 120, bot_left.y() + 30);
            position(secure_point, PI/2, 1, 1);
        }
        else if(data->ballPos.x() <= 350 && data->ballPos.y() >= 1100){  //corner top
            QPoint secure_point = QPoint(top_left.x() - 120, top_left.y() - 30);
            position(secure_point, -PI/2, 1, 1);
        }
        else if(distance(data->ballPos, center_left) <= ball_center_dist || data->ballPos.x() <= 750){
            if(is_inside(data->ballPos, data->area[LEFT_SIDE])){
                if(distance(pos, data->ballPos) > 110)
                    defend_goalLine_Left();
                else intercept_goalkeeper(false);
            }
            else if(distance(pos, data->ballPos) <= 550 && distance(data->player[data->playTeam][ID_1], data->ballPos) > 450 && distance(data->player[data->playTeam][ID_2], data->ballPos) > 450){
                if(border_y())
                    intercept_goalkeeper(true);
                else go_to_debug();
            }
            else{
                defend_goalLine_Left();
            }
        }
        else{
            if(data->ballPos.y() >= y_top){
                QPoint vec_top = middle_top - top_left;
                QPoint vec_ball_top = data->ballPos - top_left;
                double angle_ball_top = get_angle(vec_top,vec_ball_top);
                position(top_left, angle_ball_top, 1, 1);
            }
            else if(data->ballPos.y() <= y_bot){
                QPoint vec_bot = middle_bot - bot_left;
                QPoint vec_ball_bot = data->ballPos - bot_left;
                double angle_ball_bot = get_angle(vec_bot,vec_ball_bot);
                position(bot_left, angle_ball_bot, 1, 1);
            }
            else{
                QPoint vec_middle = data->middle_field - center_left;
                QPoint vec_ball_center = data->ballPos - center_left;
                double angle_ball_center = get_angle(vec_middle,vec_ball_center);
                position(center_left, angle_ball_center, 1, 1);
            }
        }
    }
    else{
        QPoint top_right = QPoint(data->max_field.x()-x_distance_goal, y_top - 200);
        QPoint center_right = QPoint(data->max_field.x()-x_distance_goal, y_center);
        QPoint bot_right = QPoint(data->max_field.x()-x_distance_goal, y_bot + 200);
        if(data->ballPos.x() >= 1150 && data->ballPos.y() <= 200){        //corner bot
            QPoint secure_point = QPoint(bot_right.x() + 120, bot_right.y() + 30);
            position(secure_point, PI/2, 1, 1);
        }
        else if(data->ballPos.x() >= 1150 && data->ballPos.y() >= 1100){  //corner top
            QPoint secure_point = QPoint(top_right.x() + 120, top_right.y() - 30);
            position(secure_point, -PI/2, 1, 1);
        }
        else if(distance(data->ballPos, center_right) <= ball_center_dist || data->ballPos.x() >= 750){
            if(is_inside(data->ballPos, data->area[RIGHT_SIDE])){
                if(distance(pos, data->ballPos) > 110)
                    defend_goalLine_Right();
                else intercept_goalkeeper(false);
            }
            else if(distance(pos, data->ballPos) <= 550 && distance(data->player[data->playTeam][ID_1], data->ballPos) > 450 && distance(data->player[data->playTeam][ID_2], data->ballPos) > 450){
                if(border_y())
                    intercept_goalkeeper(true);
                else go_to_debug();
            }
            else{
                defend_goalLine_Right();
            }
        }
        else{
            if(data->ballPos.y() >= y_top){
                QPoint vec_top = middle_top - top_right;
                QPoint vec_ball_top = data->ballPos - top_right;
                double angle_ball_top = get_angle(vec_top,vec_ball_top);
                position(top_right, angle_ball_top, 1, 1);
            }
            else if(data->ballPos.y() <= y_bot){
                QPoint vec_bot = middle_bot - bot_right;
                QPoint vec_ball_bot = data->ballPos - bot_right;
                double angle_ball_bot = get_angle(vec_bot,vec_ball_bot);
                position(bot_right, angle_ball_bot, 1, 1);
            }
            else{
                QPoint vec_middle = data->middle_field - center_right;
                QPoint vec_ball_center = data->ballPos - center_right;
                double angle_ball_center = get_angle(vec_middle,vec_ball_center);
                position(center_right, angle_ball_center, 1, 1);
            }
        }
    }
}


void Robot::defend_goalLine_Left()
{
    QPointF ball_pos = data->futureBallPos;
    QPoint goal = data->goal.getTopLeft();
    int speed_mode = 0;
    double angle = PI/2;

    goal.setX(goal.x() + 50);
    if(data->ballPos.x() >= data->middle_field.x() - 75){
        goal.setY(data->middle_field.y());
    }
    else if(ball_pos.y() > data->goal.getTopLeft().y()){
        goal.setY(data->goal.getTopLeft().y() - 80);
    }
    else if(ball_pos.y() < data->goal.getBottomLeft().y()){
        goal.setY(data->goal.getBottomLeft().y() + 80);
    }
    else{
        QPoint med_goal = medium_qpoint(data->goal.getTopLeft(),data->goal.getBottomLeft());
        if(distance(med_goal, data->ballPos) < 450){
            QPointF futureBallPos = data->futureBallPos;
            futureBallPos.setY(data->futureBallPos.y() + data->ballVel.y() * 7 * TIME_TO_RECEIVE_DATA);
            if(futureBallPos.y() >= data->goal.getTopLeft().y()){
                futureBallPos.setY(data->goal.getTopLeft().y() - 80);
            }
            else if(futureBallPos.y() <= data->goal.getBottomLeft().y()){
                futureBallPos.setY(data->goal.getBottomLeft().y() + 80);
            }
            goal.setY(futureBallPos.y());
        }
        else{
            goal.setY(ball_pos.y());
        }
    }
    if(abs(this->pos.x() - goal.x()) <= 60){
        speed_mode = 10;
        this->KP = -1.0;
        this->KD = 0.0;
        this->base_speed = 80;
    }
    else{
        this->KD = -14.9;
        this->KP = -1.0;
        this->base_speed = 80;
    }
    if(data->ballPos.y() > pos.y()){
        angle = PI/2;
    }
    else{
        angle = -PI/2;
    }
    position_goalKeeper(goal, angle, 0, 1);
}

void Robot::defend_goalLine_Right()
{
    QPointF ball_pos = data->futureBallPos;
    QPoint goal = data->goal.getTopRight();
    double angle = PI/2;
    goal.setX(goal.x() - 50);
    if(data->ballPos.x() <= data->middle_field.x() + 75){
        goal.setY(data->middle_field.y());
    }
    else if(ball_pos.y() > data->goal.getTopRight().y()){
        goal.setY(data->goal.getTopRight().y() - 80);
    }
    else if(ball_pos.y() < data->goal.getBottomRight().y()){
        goal.setY(data->goal.getBottomRight().y() + 80);
    }
    else{
        QPoint med_goal = medium_qpoint(data->goal.getTopRight(),data->goal.getBottomRight());
        if(distance(med_goal, data->ballPos) < 450){
            QPointF futureBallPos = data->futureBallPos;
            futureBallPos.setY(data->futureBallPos.y() + data->ballVel.y() * 7 * TIME_TO_RECEIVE_DATA);
            if(futureBallPos.y() >= data->goal.getTopRight().y()){
                futureBallPos.setY(data->goal.getTopRight().y() - 80);
            }
            else if(futureBallPos.y() <= data->goal.getBottomRight().y()){
                futureBallPos.setY(data->goal.getBottomRight().y() + 80);
            }
            goal.setY(futureBallPos.y());
        }
        else{
            goal.setY(ball_pos.y());
        }
    }
    if(abs(this->pos.x() - goal.x()) <= 60){
        this->KP = -1.0;
        this->KD = 0.0;
        this->base_speed = 80;
    }
    else{
        this->KD = -14.9;
        this->KP = -1.0;
        this->base_speed = 80;
    }
    if(data->ballPos.y() > pos.y()){
        angle = PI/2;
    }
    else{
        angle = -PI/2;
    }
    position_goalKeeper(goal, angle, 0, 1);
}

void Robot::defend_goalLine_Left_Projection(){
    QPointF ball_pos = data->futureBallPos;
    QPointF point = data->goal.getTopLeft();
    double angle;
    point.setX(point.x() + 50);

    if(data->ballVel.x() < 0){
        QPointF ballVellX = QPointF(data->ballVel.x(),0);
        QPointF ballVellY = QPointF(0,data->ballVel.y());
        double angle_ballVel = get_angle(ballVellX, ballVellY);
        QPointF projY = QPointF(data->ballPos.y(), 0);
        double distX_ball_projY = abs(projY.x() - data->ballPos.x());
        point.setY(data->ballPos.y() - distX_ball_projY*tan(angle_ballVel));
        if(point.y() > data->goal.getTopLeft().y()){
            point.setY(data->goal.getTopLeft().y() - 80);
        }
        else if(point.y() < data->goal.getBottomLeft().y()){
            point.setY(data->goal.getBottomLeft().y() + 80);
        }
        if(data->ballPos.y() > pos.y()){
            angle = PI/2;
        }
        else{
            angle = -PI/2;
        }

        position_goalKeeper(point, angle, 0, 1);
    }
    else{
        if(ball_pos.y() > data->goal.getTopLeft().y()){
            point.setY(data->goal.getTopLeft().y() - 80);
        }
        else if(ball_pos.y() < data->goal.getBottomLeft().y()){
            point.setY(data->goal.getBottomLeft().y() + 80);
        }
        if(data->ballPos.y() > pos.y()){
            angle = PI/2;
        }
        else{
            angle = -PI/2;
        }
        point.setY(ball_pos.y());
        position_goalKeeper(point, angle, 0, 1);
    }

}

void Robot::intercept_goalkeeper(bool hasObstacle){
    double min_pos_dist = 70;
    int speed = 7;
    QPoint go_to_position = QPoint(data->futureBallPos.x(), data->futureBallPos.y());
    this->KD = -14.9;
    if(distance(pos, data->ballPos) <= min_pos_dist){
        spin_goleiro();
    }
    else{
        if(data->playSide == LEFT_SIDE){
            if(border_x()){
                if(data->ballPos.x() < data->middle_field.x()){
                    if(data->ballPos.y() > data->middle_field.y()){
                        go_to(go_to_position, -PI/4, hasObstacle, speed);
                    }
                    else{
                        go_to(go_to_position, PI/4, hasObstacle, speed);
                    }
                }
                else{
                    if(data->ballPos.y() > data->middle_field.y()){
                        go_to(go_to_position, 3*PI/4, hasObstacle, speed);
                    }
                    else{
                        go_to(go_to_position, -3*PI/4, hasObstacle, speed);
                    }
                }
            }
            else if(border_y()){
                if(data->ballPos.y() > data->middle_field.y()){
                    go_to(go_to_position, -3*PI/4, hasObstacle, speed);
                }
                else{
                    go_to(go_to_position, 3*PI/4, hasObstacle, speed);
                }
            }
            else if(data->ballPos.y() > pos.y()){
                go_to(go_to_position, -3*PI/4, hasObstacle, speed);
            }
            else{
                go_to(go_to_position, 3*PI/4, hasObstacle, speed);
            }
        }
        //lado direito
        else{
            if(border_x()){
                if(data->ballPos.x() > data->middle_field.x()){
                    if(data->ballPos.y() > data->middle_field.y()){
                        go_to(go_to_position, -3*PI/4, hasObstacle, speed);
                    }
                    else{
                        go_to(go_to_position, 3*PI/4, hasObstacle, speed);
                    }
                }
                else{
                    if(data->ballPos.y() > data->middle_field.y()){
                        go_to(go_to_position, PI/4, hasObstacle, speed);
                    }
                    else{
                        go_to(go_to_position, -PI/4, hasObstacle, speed);
                    }
                }
            }
            else if(border_y()){
                if(data->ballPos.y() > data->middle_field.y()){
                    go_to(go_to_position, -PI/4, hasObstacle, speed);
                }
                else{
                    go_to(go_to_position, PI/4, hasObstacle, speed);
                }
            }
            else if(data->ballPos.y() > pos.y()){
                go_to(go_to_position, -PI/4, hasObstacle, speed);
            }
            else{
                go_to(go_to_position, PI/4, hasObstacle, speed);
                }
            }
        }
}


void Robot::defend_middle()
{
    int x_point = 0, y_point= 0;
    int x_var = -450;
    //Minimum ball distance to spin
    int min_distance = 90;
    //Distance between border and position to stop
    int y_distance = 60;
    //Maxium distance to stop spin
    int max_distance = 90;
    double angle = 0.0;

    if(data->playSide == LEFT_SIDE){
        x_point = data->middle_field.x() + x_var;
        //y_point = data->ballPos.y() + data->ballVel.y();
        y_point = data->futureBallPos.y();
        if(border_y()){
            if(distance(data->ballPos,pos) <= min_distance){
                spin();
            }
            else{
                if(data->ballPos.y() + 30 > data->middle_field.y())
                    y_point -= y_distance;
                else
                    y_point += y_distance;
            }
        }
    }
    else{
        x_point = data->middle_field.x() - x_var;
        //y_point = data->ballPos.y();
        y_point = data->futureBallPos.y();
        if(border_y()){
            if(distance(data->ballPos,pos) <= min_distance){
                spin();
            }
            else{
                if(data->ballPos.y() + 30 > data->middle_field.y())
                    y_point -= y_distance;
                else
                    y_point += y_distance;
            }
        }
    }
    if(pos.y() > y_point){
        angle = PI/2;
    }
    else{
        angle = -PI/2;
    }
    //Spin mode
    if(distance(pos,data->ballPos) <= min_distance){
        defend_spin = true;
    }
    else if(distance(pos,data->ballPos) >= max_distance && defend_spin == true){
        defend_spin = false;
    }
    if(defend_spin == true){
        spin();
    }
    else{
        //if(distance(pos, QPoint(x_point,y_point)) <= 40){
        //set_angle(data->ballPos);
        //}
        //else{
        position(QPoint(x_point,y_point), angle, 1, 4);
        //}
    }
}

void Robot::defend_middle_attack()
{
    double x_point = 0, y_point= 0;
    double x_var = -280;
    //Minimum ball distance to spin
    double min_distance = 80;
    //Distance between border and position to stop
    double y_distance = 60;
    //Maxium distance to stop spin
    double max_distance = 90;
    double angle = 0.0;

    if(data->playSide == LEFT_SIDE){
        x_point = data->middle_field.x() + x_var;
        //y_point = data->ballPos.y() + data->ballVel.y();
        y_point = data->futureBallPos.y();
        if(border_y()){
            if(distance(data->ballPos,pos) <= min_distance){
                spin();
            }
            else{
                if(data->ballPos.y() + 20 > data->middle_field.y())
                    y_point -= y_distance;
                else
                    y_point += y_distance;
            }
        }
    }
    else{
        x_point = data->middle_field.x() - x_var;
        //y_point = data->ballPos.y();
        y_point = data->futureBallPos.y();
        if(border_y()){
            if(distance(data->ballPos,pos) <= min_distance){
                spin();
            }
            else{
                if(data->ballPos.y() + 20 > data->middle_field.y())
                    y_point -= y_distance;
                else
                    y_point += y_distance;
            }
        }
    }
    if(pos.y() > y_point){
        angle = PI/2;
    }
    else{
        angle = -PI/2;
    }
    //Spin mode
    if(distance(pos,data->ballPos) <= min_distance){
        defend_spin = true;
    }
    else if(distance(pos,data->ballPos) >= max_distance && defend_spin == true){
        defend_spin = false;
    }
    if(defend_spin == true){
        spin();
    }
    else{
        //if(distance(pos, QPoint(x_point,y_point)) <= 40){
        //set_angle(data->ballPos);
        //}
        //else{
        position(QPoint(x_point,y_point), angle, 1, 4);
        //}
    }
}

void Robot::defend_attack()
{
    double x_point = 0, y_point= 0;
    double x_var = 300;
    //Minimum ball distance to spin
    double min_distance = 80;
    //Distance between border and position to stop
    double y_distance = 150;
    //Maxium distance to stop spin
    double max_distance = 90;
    double angle = 0.0;


    if(data->playSide == LEFT_SIDE){
        x_point = data->middle_field.x() + x_var;
        //y_point = data->ballPos.y() + data->ballVel.y();
        y_point = data->futureBallPos.y();
        if(border_y()){
            if(distance(data->ballPos,pos) <= min_distance){
                spin();
            }
            else{
                if(data->ballPos.y() + 30 > data->middle_field.y())
                    y_point -= y_distance;
                else
                    y_point += y_distance;
            }
        }
    }
    else{
        x_point = data->middle_field.x() - x_var;
        //y_point = data->ballPos.y();
        y_point = data->futureBallPos.y();
        if(border_y()){
            if(distance(data->ballPos,pos) <= min_distance){
                spin();
            }
            else{
                if(data->ballPos.y() + 30 > data->middle_field.y())
                    y_point -= y_distance;
                else
                    y_point += y_distance;
            }
        }
    }
    if(pos.y() > y_point){
        angle = PI/2;
    }
    else{
        angle = -PI/2;
    }
    //Spin mode
    if(distance(pos,data->ballPos) <= min_distance){
        defend_spin = true;
    }
    else if(distance(pos,data->ballPos) >= max_distance && defend_spin == true){
        defend_spin = false;
    }
    if(defend_spin == true){
        spin();
    }
    else{
        //if(distance(pos, QPoint(x_point,y_point)) <= 40){
        //set_angle(data->ballPos);
        //}
        //else{
        position(QPoint(x_point,y_point), angle, 1, 4);
        //}
    }
}

void Robot::defend_block(int e_id)
{
    int enemy_team = !(data->playTeam);
    QPoint enemy_pos;
    double x_dist = 0.04;
    double dist_goal = 99999, dist_ball = 99999, dist_other_goal = 0;
    int goal_id = -1, ball_id = -1, far_id = -1;
    QPoint goal_center = medium_qpoint(data->goal.getTopLeft(),data->goal.getBottomLeft());
    if(data->playSide == RIGHT_SIDE){
        x_dist *= -1;
        goal_center = medium_qpoint(data->goal.getTopRight(),data->goal.getBottomRight());
    }
    for(int i = 0; i < 3; ++i){
        enemy_pos = data->player[enemy_team][i];
        if(enemy_pos != QPoint(-5,481)){
            double distball_aux = distance(data->ballPos,enemy_pos);
            double distgoal_aux = distance(goal_center,enemy_pos);
            double distother_aux = distance(goal_center,enemy_pos);
            if(distother_aux > dist_other_goal){
                far_id = i;
                dist_other_goal = distother_aux;
            }
            if(distball_aux < dist_ball){
                ball_id = i;
                dist_ball = distball_aux;
            }
            if(distgoal_aux < dist_goal){
                goal_id = i;
                dist_goal = distgoal_aux;
            }
        }
    }
    //Segurar o que esta perto da bola
    if(e_id == 1){
        if(goal_id != -1 && !(is_inside(data->player[enemy_team][goal_id],data->area[data->playSide]))){
            QPoint go_point = data->player[enemy_team][goal_id];
            go_point.setX(go_point.x() - x_dist);
            double angle = PI/2;
            if(pos.y() < go_point.y()){
                angle *= -1.0;;
            }
            position(go_point,angle,1,3);
        }
        else{
            defend_middle();
        }
    }
    //Segurar o que esta perto do gol
    else if(e_id == 2){
        if(ball_id != -1 && !(is_inside(data->player[enemy_team][ball_id],data->area[data->playSide]))){
            QPoint go_point = data->player[enemy_team][ball_id];
            go_point.setX(go_point.x() - x_dist);
            double angle = PI/2;
            if(pos.y() < go_point.y()){
                angle *= -1.0;;
            }
            position(go_point,angle,1,3);
        }
        else{
            defend_middle();
        }
    }
    else{
        if(far_id != -1 && !(is_inside(data->player[enemy_team][far_id],data->area[data->playSide]))){
            QPoint go_point = data->player[enemy_team][far_id];
            go_point.setX(go_point.x() - x_dist);
            double angle = PI/2;
            if(pos.y() < go_point.y()){
                angle *= -1.0;;
            }
            position(go_point,angle,1,3);
        }
        else{
            defend_middle();
        }
    }
}

/**
 * @brief Go to a position and than stops and adjusts the angle.
 * @param point Go-to point
 * @param angle Final angle
 * @param speed_mode Speed mode:	1 - Slow
 *									2 - Medium
 *									3 - Fast
 */
void Robot::position(QPoint point, double angle, bool has_obstacle, int speed_mode)
{
    double min_pos_dist = 40;

    if(abs(pos.x() - point.x()) <= 20 && abs(pos.y() - point.y()) <= 20 && !pos_stat){
        if(abs(input) < 5){
            pos_stat = true;
            stopRobot();
            //cout << "Angulo correto" << endl;
        }
        else {
            set_angle(angle);
            //cout << "Angulo incorreto" << endl;
        }
    }
    else{
        /*if(!pos_stat){
            //cout << "Longe do ponto" << endl;
            go_to(point,-999,has_obstacle,speed_mode);
        }
        else if(abs(pos.x() - point.x()) >= 7 || abs(pos.y() - point.y()) >= 7){
            pos_stat = false;
        }*/
        if(distance(pos, point) <= min_pos_dist && !pos_stat){
            pos_stat = false;
            set_angle(angle);
            //cout << "Perto do ponto" << endl;
        }
        else{
            pos_stat = false;
            //cout << "Longe do ponto" << endl;
            go_to(point,-999,has_obstacle,speed_mode);
        }
    }
}

void Robot::position_goalKeeper(QPointF point, double angle, bool has_obstacle, int speed_mode){
    double min_pos_dist = 30;

    if(abs(pos.x() - point.x()) < 20 && abs(pos.y() - point.y()) < 15 && !pos_stat){
        if(abs(input) < 5){
            pos_stat = true;
            stopRobot();
            //cout << "Angulo correto" << endl;
        }
        else {
            set_angle(angle);
            //cout << "Angulo incorreto" << endl;
        }
    }
    else{
        /*if(!pos_stat){
            cout << "Longe do ponto" << endl;
            go_to(point,-999,has_obstacle,speed_mode);
        }
        else if(abs(pos.x() - point.x()) >= 10 || abs(pos.y() - point.y()) >= 10){
            pos_stat = false;
        }*/
        if(distance(pos, point) <= min_pos_dist && !pos_stat){
            pos_stat = false;
            set_angle(angle);
            //cout << "Perto do ponto" << endl;
        }
        else{
            pos_stat = false;
            //cout << "Longe do ponto" << endl;
            go_to(point,-999,has_obstacle,speed_mode);
        }
    }
}

void Robot::set_angle(double heading_angle)
{
    double kp_ = this->KP;
    double ki_ = this->KI;
    double kd_ = this->KD;

    double out_min = -this->spin_speed;
    double out_max = this->spin_speed;

    double angle_aux1 = (heading_angle - angle)*180/PI;
    double angle_aux2 = (heading_angle - angle + PI)*180/PI;

    if(angle_aux1 > 180)
        angle_aux1 -= 360;
    if(angle_aux1 < -180)
        angle_aux1 += 360;
    if(angle_aux2 > 180)
        angle_aux2 -= 360;
    if(angle_aux2 < -180)
        angle_aux2 += 360;

    if(fabs(angle_aux1) < fabs(angle_aux2)){
        input = angle_aux1;
        output = angle_pid.process(kp_, ki_, kd_, out_min, out_max);
        speed.first = -output;
        speed.second = output;
    }
    else{
        input = angle_aux2;
        output = angle_pid.process(kp_, ki_, kd_, out_min, out_max);
        speed.first = -output;
        speed.second = output;
    }

    //cout<< output << endl;
    //cout << "setangle" << endl;
    this->sendFIRA(this->getId(),speed.first,speed.second);
}

void Robot::set_angle(QPoint heading_point)
{
    double kp_ = -this->KP;
    double ki_ = -this->KI;
    double kd_ = -this->KD;
    double out_min = -this->spin_speed;
    double out_max = this->spin_speed;
    double angle_aux1 = (get_angle(QPoint(1000*cos(angle),1000*sin(angle)),QPoint(heading_point - pos)))*180/PI;
    double angle_aux2 = (get_angle(QPoint(1000*cos(angle),1000*sin(angle)),QPoint(heading_point - pos)) + PI)*180/PI;
    if(angle_aux1 > 180)
        angle_aux1 -= 360;
    if(angle_aux1 < -180)
        angle_aux1 += 360;
    if(angle_aux2 > 180)
        angle_aux2 -= 360;
    if(angle_aux2 < -180)
        angle_aux2 += 360;
    if(fabs(angle_aux1) < fabs(angle_aux2)){
        input = angle_aux1;
        output = angle_pid.process(kp_, ki_, kd_, out_min, out_max);
        speed.first = -output;
        speed.second = output;
    }
    else{
        input = angle_aux2;
        output = angle_pid.process(kp_, ki_, kd_, out_min, out_max);
        speed.first = -output;
        speed.second = output;
    }
    this->sendFIRA(this->getId(),-speed.first,-speed.second);
}

void Robot::spin_ataque()
{
    //double y = pos.y();
    double ball_y = data->ballPos.y();
    double ball_middle = data->middle_field.y();
    if(data->playSide == LEFT_SIDE && ball_middle > ball_y ){
        this->sendFIRA(this->getId(),-spin_speed,spin_speed);
    }
    else if(data->playSide == LEFT_SIDE && ball_middle <= ball_y){
        this->sendFIRA(this->getId(),spin_speed,-spin_speed);
    }
    else if(data->playSide == RIGHT_SIDE && ball_middle > ball_y){
        this->sendFIRA(this->getId(),spin_speed,-spin_speed);
    }
    else if(data->playSide == RIGHT_SIDE && ball_middle <= ball_y){
        this->sendFIRA(this->getId(),-spin_speed,spin_speed);
    }
}

void Robot::spin_goleiro()
{
    if(data->playSide == LEFT_SIDE && data->ballPos.y() < pos.y())
        this->sendFIRA(this->getId(),-spin_speed,spin_speed);
    else if(data->playSide == LEFT_SIDE && pos.y() < data->ballPos.y())
        this->sendFIRA(this->getId(),spin_speed,-spin_speed);
    if(data->playSide == RIGHT_SIDE && data->ballPos.y() < pos.y())
        this->sendFIRA(this->getId(),spin_speed,-spin_speed);
    else if(data->playSide == RIGHT_SIDE && pos.y() < data->ballPos.y())
        this->sendFIRA(this->getId(),-spin_speed,spin_speed);
}

void Robot::spin_x()
{
    double ball_y = data->ballPos.y();
    if(data->playSide == LEFT_SIDE && ball_y > data->middle_field.y())
        this->sendFIRA(this->getId(),spin_speed,-spin_speed);
    else if(data->playSide == LEFT_SIDE && ball_y <= data->middle_field.y())
        this->sendFIRA(this->getId(),-spin_speed,spin_speed);
    if(data->playSide == RIGHT_SIDE && ball_y <= data->middle_field.y())
        this->sendFIRA(this->getId(),spin_speed,-spin_speed);
    else if(data->playSide == RIGHT_SIDE && ball_y > data->middle_field.y())
        this->sendFIRA(this->getId(),-spin_speed,spin_speed);
}


void Robot::spin()
{
    double y = pos.y();
    double ball_y = data->ballPos.y();
    if(data->playSide == LEFT_SIDE && y > ball_y ){
        this->sendFIRA(this->getId(),-spin_speed,spin_speed);
    }
    else if(data->playSide == LEFT_SIDE && y <= ball_y){
        this->sendFIRA(this->getId(),spin_speed,-spin_speed);
    }
    else if(data->playSide == RIGHT_SIDE && y > ball_y ){
        this->sendFIRA(this->getId(),spin_speed,-spin_speed);
    }
    else if(data->playSide == RIGHT_SIDE && y <= ball_y ){
        this->sendFIRA(this->getId(),-spin_speed,spin_speed);
    }
}

void Robot::spin_penalty(){
    double spinA = spin_speed * 1.0 + data->speed_spin_increment;
    double spinB = spin_speed * 0.245 - data->speed_spin_increment/0.2;
    data->speed_spin_increment += data->speed_spin_increment;
    if(data->playSide == LEFT_SIDE){
        this->sendFIRA(this->getId(),spinB,-spinA);
    }
    else{
        this->sendFIRA(this->getId(),-spinA,spinB);
    }
}

void Robot::defend_center(){
    position(this->data->middle_field,PI/2.0,1,1);
}
#endif // ROBOT_CPP
