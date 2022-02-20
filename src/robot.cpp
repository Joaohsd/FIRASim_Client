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
        //GOALKEEPER - GOALKEEPER - GOALKEEPER
        switch(data->mode){
        case bola_ataque:
            this->defend_goal_LARC();
            break;
        case bola_defesa:
            this->defend_goal_LARC();
            break;
        case bola_area:
            this->defend_goal_LARC();
            break;
        }
    }
}

void Robot::playID_1(){
    if(this->data->formation1){ // 1-0-2
        //STRICKER - MIDFIELDER - MIDFIELDER
        switch(data->mode){
        case bola_ataque:
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
}

void Robot::playID_2(){
    if(this->data->formation1){ // 1-0-2
        //STRICKER - DEFENDER - DEFENDER
        switch(data->mode){
        case bola_ataque:
            if(!data->penalty)
                this->attack();
            break;
        case bola_defesa:
            this->defend();
            break;
        case bola_area:
            this->defend_middle();
            break;
        }
    }
}

void Robot::attack()
{
    enum play_mode {att_normal, att_borda, att_danger} mode = att_normal;

    if(verify_Strickers_Left_Is_Inside_Attack_Area() && this->data->playSide == LEFT_SIDE){
        mode = att_danger;
    }

    else if(verify_Strickers_Right_Is_Inside_Attack_Area() && this->data->playSide == RIGHT_SIDE){
        mode = att_danger;
    }

    else if(this->border_y()){
        mode = att_borda;
    }

    else{
        mode = att_normal;
    }

    switch(mode){
    case att_normal:
        if(distance(data->player[data->playTeam][ID_2], data->ballPos) <= distance(data->player[data->playTeam][ID_1], data->ballPos)){
            if(this->getId() == ID_2)
                this->kick();
            else this->defend_attack();
        }
        else{
            if(this->getId() == ID_2)
                this->defend_attack();
            else this->kick();
        }
        break;
    case att_borda:
        this->intercept();
        break;
    case att_danger:
        if(this->data->playSide == LEFT_SIDE){
            if(is_inside(this->data->player[this->data->playTeam][ID_2], this->data->area[RIGHT_SIDE]) || this->data->player[this->data->playTeam][ID_2].x() > this->data->max_field.x()){
                if(this->getId() == ID_2){
                    this->kick();
                }
                else{
                    this->defend_attack();
                }
            }
            else{
                if(this->getId() == ID_1){
                    this->kick();
                }
                else{
                    this->defend_attack();
                }
            }
        }
        else{
            if(is_inside(this->data->player[this->data->playTeam][ID_2], this->data->area[LEFT_SIDE]) || this->data->player[this->data->playTeam][ID_2].x() < this->data->min_field.x()){
                if(this->getId() == ID_2){
                    this->kick();
                }
                else{
                    this->defend_attack();
                }
            }
            else{
                if(this->getId() == ID_1){
                    this->kick();
                }
                else{
                    this->defend_attack();
                }
            }
        }
        break;
    }
}

void Robot::kick()
{
    double min_dist_ball_y = 220;
    double max_dist_ball_y = 550;
    //double min_distance_kickable = 70;
    //Minimum distance threshold for entering kick mode
    double min_threshold = 160; //Modified 0.25 to 0.15
    //Minimum angle diference threshold for entering kick mode
    double angle_threshold = 15; //Alterado de 10 p 20
    //Maximum angle diference threshold for leaving kick mode
    double max_angle_threshold = 55; //50
    //Maximum distance threshold for leaving kick mode
    double max_threshold = 270;
    double dist_bet_players = MIN_DIST_BET_PLAYERS;
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
                else {//if((this->data->ballPos.y() <= (this->data->middle_field.y() + max_dist_ball_y)) && (this->data->ballPos.y() >= (this->data->middle_field.y() - max_dist_ball_y))){ //External
                    if(this->data->ballPos.y() > this->data->middle_field.y()){ // External up
                        //cout << "em cima meio" << endl;
                        this->go_to(QPoint(this->data->futureBallPos.x(), this->data->futureBallPos.y()), 3*PI/4.0, 1, 3);//3*PI/4.0
                    }
                    else{   // External down
                        //cout << "baixo meio" << endl;
                        this->go_to(QPoint(this->data->futureBallPos.x(), this->data->futureBallPos.y()), -3*PI/4.0, 1, 3);//-3*PI/4.0
                    }
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
        //}
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
            /*if(this->data->ballPos.y() <= this->data->middle_field.y() + min_dist_ball_y && this->data->ballPos.y() >= this->data->middle_field.y() - min_dist_ball_y){
                //cout << "meio" << endl;
                this->go_to(QPoint(this->data->futureBallPos.x(), this->data->futureBallPos.y()), 0, 1, 3);
            }
            else {*///if(this->data->ballPos.y() <= this->data->middle_field.y() + max_dist_ball_y && this->data->ballPos.y() >= this->data->middle_field.y() - max_dist_ball_y){
                if(this->data->ballPos.y() > this->data->middle_field.y()){
                    //cout << "em cima dent" << endl;
                    this->go_to(QPoint(this->data->futureBallPos.x(), this->data->futureBallPos.y()), PI/4.0, 1, 3);
                }
                else{
                    //cout << "baixo dent" << endl;
                    this->go_to(QPoint(this->data->futureBallPos.x(), this->data->futureBallPos.y()), -PI/4.0, 1, 3);
                }
            }
        //}}
        else{
            QPoint med_goal = medium_qpoint(this->data->goal.getTopLeft(), this->data->goal.getBottomLeft());
            QPoint vec_ball_goal = QPoint(med_goal.x() - this->data->ballPos.x(), med_goal.y() - this->data->ballPos.y());
            //go_to(QPoint(data->ballPos.x() + vec_ball_goal.x(),data->ballPos.y() + vec_ball_goal.y()), -999, 0, 5);
            this->go_to_kick(QPoint(this->data->ballPos.x() + vec_ball_goal.x(),this->data->ballPos.y() + vec_ball_goal.y()), min(this->speed_kick_current, this->speed_kick_max));
            this->speed_kick_current += this->speed_kick_increment;
            }
        //}
    }
}

void Robot::defend(){
    if(distance(data->player[data->playTeam][ID_2], data->ballPos) <= distance(data->player[data->playTeam][ID_1], data->ballPos)){
        if(this->getId() == ID_2)
            this->intercept();
        else this->defend_middle_attack();
    }
    else{
        if(this->getId() == ID_2)
            this->defend_middle_attack();
        else this->intercept();
    }
}

void Robot::intercept_antigo()
{
    double min_pos_dist = 60;
    int speed = -1;
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
    int speed = 3;
    QPoint goal = medium_qpoint(data->goal.getTopLeft(),data->goal.getBottomLeft());
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
            else if(data->ballPos.y() > pos.y()){
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
            else if(data->ballPos.y() > pos.y()){
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
        input = get_angle(QPointF(1000*cos(angle),1000*sin(angle)),QPointF(target-pos))*180/PI;
    }
    else{
        phi = path.get_angle(pos,target,target_angle,this->team_id,this->player_id,have_obstacle);

        if(phi > 2*PI){
            phi -= 2*PI;
        }
        else if(phi < -2*PI){
            phi += 2*PI;
        }

        //cout << "UVF: " << phi*180/PI << " GRAUS" << endl;

        //Da um passo com o robo a partir do vetor direção fornecido pelo uvf
        fut_pos.setX(pos.x() + 60*cos(phi));
        fut_pos.setY(pos.y() + 60*sin(phi));
        //O input é o ângulo entre o vetor orientação do robo e o vetor direção da trajetória a ser feita (fut_pos - pos)
        input = get_angle(QPoint(60*cos(angle),60*sin(angle)),QPoint(fut_pos - pos))*180/PI;
        /*cout << "POS X:" << pos.x() << endl;
        cout << "POS Y:" << pos.y() << endl;
        cout << "ANGULO ROBO: " << this->angle*180/PI << endl;
        cout << "ANGULO DESEJADO: " << target_angle*180/PI << endl;
        cout << "FUT X 1000:    " << fut_pos.x() << endl;
        cout << "FUT Y 1000:    " << fut_pos.y() << endl;
        cout << "DIR X 1000:    " << QPoint(fut_pos - pos).x() << endl;
        cout << "DIR Y 1000:    " << QPoint(fut_pos - pos).y() << endl;
        cout << "INPUT: " << input << endl;*/
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
        double min_goto_dist = 320; //0.3
        double min_goto_dist_2 = 520; //0.5
        double min_goto_fact = 700;     //CONFERIR
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
        cout << "decay_mode 3" << endl;
        double min_dist = 320; //0.3
        double min_min_dist = 160;//0.18
        if(distance(pos,target) <= min_min_dist){
            cout << "PERTO" << endl;
            curr_speed *= 0.7; //0.65
            kp_ *= 0.7; //0.7
            ki_ *= 0.8; //0.7
            kd_ *= 0.6; //0.7
        }
        else if(distance(pos,target) <= min_dist){
            curr_speed *= 0.8; //0.8
            cout << "LONGE" << endl;
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
            curr_speed *= 0.8; //75
            kp_ *= 1.0;
            ki_ *= 0.7;
            kd_ *= 1.0;
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
        //cout << "OUTPUT:  " << output << endl;
        speed.first = -1.0*curr_speed;
        speed.second = -1.0*curr_speed + output;
    }
    else{
        output = path_pid.process(kp_, ki_, kd_, out_min, out_max);
        speed.first = curr_speed - output;
        speed.second = curr_speed;
    }
    //cout << "WHEEL LEFT:  " << speed.first << endl;
    //cout << "WHEEL RIGHT:  " << speed.second << endl;

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
    //cout << speed.first << " " << speed.second << endl;
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

void Robot::stopRobot(){
    this->sendFIRA(this->getId(),0,0);
}

void Robot::defend_goal_Left()
{
    QPointF ball_pos = data->futureBallPos;
    QPoint goal = data->goal.getTopLeft();
    goal.setX(goal.x() + 0.06);
    if(ball_pos.y() > data->goal.getTopLeft().y()){
        goal.setY(data->goal.getTopLeft().y() - 0.08);
    }
    else if(ball_pos.y() < data->goal.getBottomLeft().y()){
        goal.setY(data->goal.getBottomLeft().y() + 0.08);
    }
    else{
        goal.setY(ball_pos.y());
    }
    if(distance(pos,goal) <= 0.05){
        set_angle(PI/2);
    }
    else{
        go_to(goal,-999,0,3);
    }
}

void Robot::defend_goal_Right()
{
    QPointF ball_pos = data->futureBallPos;
    QPoint goal = data->goal.getTopRight();
    double x_distance_goal = 0.05;
    goal.setX(goal.x() + x_distance_goal);
    if(ball_pos.y() > data->goal.getTopRight().y()){
        goal.setY(data->goal.getTopRight().y());
    }
    else if(ball_pos.y() < data->goal.getBottomRight().y()){
        goal.setY(data->goal.getBottomRight().y());
    }
    else{
        goal.setY(ball_pos.y());
    }
    if(distance(pos,goal) <= 0.05){
        set_angle(PI/2);
    }
    else{
        go_to(goal,-999,0,3);
    }
}

void Robot::defend_goal_LARC(){
    double x_pos;
    int x_distance_goal = 200;
    int ball_goal_dist = 450;

    int y_top = 1050;
    int y_bot = 250;
    int y_center = 650;

    QPoint top_right = QPoint(data->max_field.x()-x_distance_goal, y_top - 200);
    QPoint center_right = QPoint(data->max_field.x()-x_distance_goal, y_center);
    QPoint bot_right = QPoint(data->max_field.x()-x_distance_goal, y_bot + 200);

    QPoint top_left = QPoint(data->min_field.x()+x_distance_goal, y_top - 200);
    QPoint center_left = QPoint(data->min_field.x()+x_distance_goal, y_center);
    QPoint bot_left = QPoint(data->min_field.x()+x_distance_goal, y_bot + 200);

    QPoint middle_top = QPoint(data->middle_field.x(),data->middle_field.y()+200);
    QPoint middle_bot = QPoint(data->middle_field.x(),data->middle_field.y()-200);

    if(this->data->playSide == LEFT_SIDE){
        if((data->ballPos.x() <= 370 && data->ballPos.y() <= 280)/* && flag_Corner_Fight*/){
            QPoint secure_point = QPoint(bot_left.x() - 130, bot_left.y());
            position(secure_point, -PI/2, 0, 1);
        }
        else if((data->ballPos.x() <= 370 && data->ballPos.y() >= 1020)/* && flag_Corner_Fight*/){
            QPoint secure_point = QPoint(top_left.x() - 130, top_left.y());
            position(secure_point, PI/2, 0, 1);
        }
        else if(distance(this->data->ballPos,center_left) <= ball_goal_dist || (data->ballPos.x() <= 450 && distance(data->player[data->playTeam][ID_1], data->ballPos) > 350 && distance(data->player[data->playTeam][ID_2], data->ballPos) > 350)){
            if(is_inside(data->ballPos, data->area[LEFT_SIDE]))
                intercept();
            else
                go_to_debug();
        }
        else{
            if(data->ballPos.y() >= y_top){
                QPoint vec_top = middle_top - top_left;
                QPoint vec_ball_goalkeeper = data->ballPos - top_left;
                double angle_ball_goalKeeper = get_angle(vec_top,vec_ball_goalkeeper);
                position(top_left, angle_ball_goalKeeper, 0, 1);
            }
            else if(data->ballPos.y() <= y_bot){
                QPoint vec_bot = middle_bot - bot_left;
                QPoint vec_ball_goalkeeper = data->ballPos - bot_left;
                double angle_ball_goalKeeper = get_angle(vec_bot,vec_ball_goalkeeper);
                position(bot_left, angle_ball_goalKeeper, 0, 1);
            }
            else{
                QPoint vec_middle = data->middle_field - center_left;
                QPoint vec_ball_goalkeeper = data->ballPos - center_left;
                double angle_ball_goalKeeper = get_angle(vec_middle,vec_ball_goalkeeper);
                if(data->ballPos.y() > top_left.y())
                    position(top_left, angle_ball_goalKeeper, 0, 1);
                else if(data->ballPos.y() < bot_left.y())
                    position(bot_left, angle_ball_goalKeeper, 0, 1);
                else position(QPoint(center_left.x(),data->ballPos.y()), angle_ball_goalKeeper, 0, 1);
            }
        }
    }
    else{
        if((data->ballPos.x() >= 1130 && data->ballPos.y() <= 280) /*&& flag_Corner_Fight*/){
            QPoint secure_point = QPoint(bot_right.x() + 130, bot_right.y());
            position(secure_point, PI/2, 0, 1);
        }
        else if((data->ballPos.x() >= 1.13 && data->ballPos.y() >= 1020) /*&& flag_Corner_Fight*/){
            QPoint secure_point = QPoint(top_right.x() + 130, top_right.y());
            position(secure_point, -PI/2, 0, 1);
        }
        else if((distance(this->data->ballPos,center_right) <= ball_goal_dist || data->ballPos.x() >= 1050) && distance(data->player[data->playTeam][ID_1], data->ballPos) > 350 && distance(data->player[data->playTeam][ID_2], data->ballPos) > 350){
            if(is_inside(data->ballPos, data->area[RIGHT_SIDE]))
                intercept();
            else
                go_to_debug();
        }
        else{
            if(data->ballPos.y() >= y_top){
                QPoint vec_top = middle_top - top_right;
                QPoint vec_ball_goalkeeper = data->ballPos - top_right;
                double angle_ball_goalKeeper = get_angle(vec_top,vec_ball_goalkeeper);
                position(top_right, angle_ball_goalKeeper, 0, 1);
            }
            else if(data->ballPos.y() <= y_bot){
                QPoint vec_bot = middle_bot - bot_right;
                QPoint vec_ball_goalkeeper = data->ballPos - bot_right;
                double angle_ball_goalKeeper = get_angle(vec_bot,vec_ball_goalkeeper);
                position(bot_right, angle_ball_goalKeeper, 0, 1);
            }
            else{
                if(distance(this->data->ballPos,center_right) <= ball_goal_dist){
                    go_to_debug();
                }
                else{
                    QPoint vec_middle = data->middle_field - center_right;
                    QPoint vec_ball_goalkeeper = data->ballPos - center_right;
                    double angle_ball_goalKeeper = get_angle(vec_middle,vec_ball_goalkeeper);
                    if(data->ballPos.y() > top_right.y())
                        position(top_right, angle_ball_goalKeeper, 0, 1);
                    else if(data->ballPos.y() < bot_right.y())
                        position(bot_right, angle_ball_goalKeeper, 0, 1);
                    else position(QPoint(center_right.x(),data->ballPos.y()), angle_ball_goalKeeper, 0, 1);
                }
            }
        } 
    }
}

void Robot::defend_middle()
{
    int x_point = 0, y_point= 0;
    int x_var = -400;
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

    /*
    if(player_id == 0){
        x_var = -0.5;
    }*/

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
    double min_pos_dist = 30;
    if(speed_mode == 1){
        double max_distance = 200;
        if(distance(pos, point) <= min_pos_dist /*&& abs(pos.x() - point.x()) < 10 && abs(pos.y() - point.y()) < 20*/){
            set_angle(angle);
        }
        else{
            if(distance(pos,point) <= max_distance){
                go_to(point,-999,0,1);
            }
            else{
                go_to(point, angle, has_obstacle, 1);
            }
        }
    }
    else{
        if(distance(pos, point) <= min_pos_dist){
            set_angle(angle);
        }
        else{
            go_to(point, angle, has_obstacle, speed_mode);
        }
    }
}

/*
void Robot::position(QPoint point, double angle, bool has_obstacle, int speed_mode)
{
    double min_pos_dist = 5;
    double med_pos_dist = 15;
    double dist_to_point = distance(pos, point);
    if(speed_mode == 1){
        double max_distance = 350;
        /*if(distance(pos, point) <= min_pos_dist && this->getAngle() != PI/2.0){
            set_angle(angle);
        }*//*
        if(dist_to_point <= min_pos_dist*/ /*|| this->getAngle() == PI/2.0*//*){
            this->stopRobot();
            //cout << "PAROU" << endl;
        }
        else if(dist_to_point > min_pos_dist && dist_to_point <= med_pos_dist){
            set_angle(angle);
        }
        else{
            if(distance(pos,point) <= max_distance){
                go_to(point,-999,0,1);
            }
            else{
                go_to(point, angle, has_obstacle, 1);
            }
        }
    }
    else{
        if(distance(pos, point) <= min_pos_dist){
            set_angle(angle);
        }
        else{
            go_to(point, angle, has_obstacle, speed_mode);
        }
    }
}
*/


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
    double spinA = spin_speed * 0.25 + data->speed_spin_increment;
    double spinB = spin_speed * 0.25 - data->speed_spin_increment/3.3;
    data->speed_spin_increment += data->speed_spin_increment;
    if(data->playSide == LEFT_SIDE){
        this->sendFIRA(this->getId(),-spinB,spinA);
    }
    else{
        this->sendFIRA(this->getId(),spinA,-spinB);
    }
}

void Robot::defend_center(){
    position(this->data->middle_field,PI/2.0,1,1);
}
#endif // ROBOT_CPP
