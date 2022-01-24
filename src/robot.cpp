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

void Robot::setRobot(int _team_id, int _id, double _kp, double _ki, double _kd, double _base_speed, double _spin_speed){
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

double Robot::getX(){
    return this->x;
}

double Robot::getY(){
    return this->y;
}

double Robot::getVx(){
    return this->vx;
}

double Robot::getVy(){
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

void Robot::setX(double x){
    this->x = x;
    this->pos.setX(x);
}

void Robot::setY(double y){
    this->y = y;
    this->pos.setY(y);
}

void Robot::setVx(double vx){
    this->vx = vx;
}

void Robot::setVy(double vy){
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

void Robot::setData(StaticData* data){
    this->data = data;
    this->path.data = data;
}

void Robot::sendFIRA(int id, double wheelL, double wheelR){
    this->actuator->sendCommand(id,wheelL,wheelR);
}

void Robot::run(){
    if(this->getId() == GOAL_ID){
        this->playGoalKeeper();
    }
    else if(this->getId() == DEF_ID){
        this->playDefender();
    }
    else{
        this->playStricker();
    }
}

void Robot::playGoalKeeper(){
    if(this->data->formation1){ // 1-0-2
        //GOALKEEPER
        switch(data->mode){
        case bola_ataque:
            if(!data->penalty)
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
    emit endPlay();
}

void Robot::playDefender(){
    if(this->data->formation1){ // 1-0-2
        //DEFENDER
        switch(data->mode){
        case bola_ataque:
            this->attack();
            break;
        case bola_defesa:
            this->defend_middle_attack();
            break;
        case bola_area:
            this->defend_middle_attack();
            break;
        }
    }
    emit endPlay();
}

void Robot::playStricker(){
    if(this->data->formation1){ // 1-0-2
        //STRICKER
        switch(data->mode){
        case bola_ataque:
            if(!data->penalty)
                this->attack();
            break;
        case bola_defesa:
            this->intercept();
            break;
        case bola_area:
            this->intercept();
            break;
        }
    }
    emit endPlay();
}

void Robot::attack()
{
    enum play_mode {att_normal, att_borda, att_danger} mode = att_normal;

    if(verify_Strickers_Left_Is_Inside_Attack_Area()){
        if(this->data->playSide == LEFT_SIDE){
           mode = att_danger;
        }
    }

    else if(verify_Strickers_Right_Is_Inside_Attack_Area()){
        if(this->data->playSide == RIGHT_SIDE){
            mode = att_danger;
        }
    }

    else if(this->border_y()){
        mode = att_borda;
    }

    else{
        mode = att_normal;
    }

    switch(mode){
    case att_normal:
        this->kick();
        break;
    case att_borda:
        this->intercept();
        break;
    case att_danger:
        if(is_inside(this->data->player[data->playTeam][ATT_ID],this->data->area[RIGHT_SIDE]) || is_inside(this->data->player[data->playTeam][ATT_ID],this->data->area[LEFT_SIDE])){
            if(this->getId() == 2){
                this->kick();
                //cout << "ENTROU KICK :::::::    2" << endl;
            }
            if(this->getId() == 1){
                this->defend_attack();
                //cout << "ENTROU DEFEND :::::::    1" << endl;
            }
        }
        else{
            if(this->getId() == 2){
                this->defend_attack();
                //cout << "ENTROU DEFEND :::::::    2" << endl;
            }
            if(this->getId() == 1){
                //cout << "ENTROU KICK :::::::    1" << endl;
                this->kick();
            }
        }
        break;
    }
}

void Robot::kick()
{
    double min_dist_ball_y = 0.25;
    double max_dist_ball_y = 0.52;
    //double min_distance_kickable = 70;
    //Minimum distance threshold for entering kick mode
    double min_threshold = 0.2; //Modified 0.25 to 0.15
    //Minimum angle diference threshold for entering kick mode
    double angle_threshold = 15; //Alterado de 10 p 20
    //Maximum angle diference threshold for leaving kick mode
    double max_angle_threshold = 55; //50
    //Maximum distance threshold for leaving kick mode
    double max_threshold = 0.3;
    double dist_bet_players = MIN_DIST_BET_PLAYERS;
    QPointF med_goal2;
    //Angle between player and ball
/*    QPointF vec_middle_max_field = data->max_field - data->middle_field;
    QPointF vec_ball_player = pos - data->ballPos;
    QPointF vec_player_middle = pos - vec_middle_max_field;

    double player_ball_target_angle = fabs(get_angle(vec_player_middle,vec_ball_player))*180/PI;
*/
    double ball_player_angle = fabs(get_angle(QPointF(30*cos(angle),30*sin(angle)),QPointF(data->ballPos - pos)))*180/PI;
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
                /*if(this->data->ballPos.y() <= this->data->middle_field.y() + min_dist_ball_y && this->data->ballPos.y() >= this->data->middle_field.y() - min_dist_ball_y){ // Center
                    //cout << "meio" << endl;
                    this->go_to(QPointF(this->data->futureBallPos.x(), this->data->futureBallPos.y()), PI, 1, 3); //PI
                }
                else {*///if((this->data->ballPos.y() <= (this->data->middle_field.y() + max_dist_ball_y)) && (this->data->ballPos.y() >= (this->data->middle_field.y() - max_dist_ball_y))){ //External
                    if(this->data->ballPos.y() > this->data->middle_field.y()){ // External up
                        //cout << "em cima meio" << endl;
                        this->go_to(QPointF(this->data->futureBallPos.x(), this->data->futureBallPos.y()), 3*PI/4.0, 1, 3);//3*PI/4.0
                    }
                    else{   // External down
                        //cout << "baixo meio" << endl;
                        this->go_to(QPointF(this->data->futureBallPos.x(), this->data->futureBallPos.y()), -3*PI/4.0, 1, 3);//-3*PI/4.0
                    }
                }
            //}
            else{
                QPointF med_goal = medium_qpoint(this->data->goal.getTopRight(),this->data->goal.getBottomRight());
                QPointF vec_ball_goal = QPointF(med_goal.x() - this->data->ballPos.x(), med_goal.y() - this->data->ballPos.y());
                //go_to(QPoint(data->ballPos.x() + vec_ball_goal.x(),data->ballPos.y() + vec_ball_goal.y()), -999, 0, 5);
                this->go_to_kick(QPointF(this->data->ballPos.x() + vec_ball_goal.x(), this->data->ballPos.y() + vec_ball_goal.y()), min(this->speed_kick_current, this->speed_kick_max));
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
                this->go_to(QPointF(this->data->futureBallPos.x(), this->data->futureBallPos.y()), 0, 1, 3);
            }
            else {*///if(this->data->ballPos.y() <= this->data->middle_field.y() + max_dist_ball_y && this->data->ballPos.y() >= this->data->middle_field.y() - max_dist_ball_y){
                if(this->data->ballPos.y() > this->data->middle_field.y()){
                    //cout << "em cima dent" << endl;
                    this->go_to(QPointF(this->data->futureBallPos.x(), this->data->futureBallPos.y()), PI/4.0, 1, 3);
                }
                else{
                    //cout << "baixo dent" << endl;
                    this->go_to(QPointF(this->data->futureBallPos.x(), this->data->futureBallPos.y()), -PI/4.0, 1, 3);
                }
            }
        //}}
        else{
            QPointF med_goal = medium_qpoint(this->data->goal.getTopLeft(), this->data->goal.getBottomLeft());
            QPointF vec_ball_goal = QPointF(med_goal.x() - this->data->ballPos.x(), med_goal.y() - this->data->ballPos.y());
            //go_to(QPoint(data->ballPos.x() + vec_ball_goal.x(),data->ballPos.y() + vec_ball_goal.y()), -999, 0, 5);
            this->go_to_kick(QPointF(this->data->ballPos.x() + vec_ball_goal.x(),this->data->ballPos.y() + vec_ball_goal.y()), min(this->speed_kick_current, this->speed_kick_max));
            this->speed_kick_current += this->speed_kick_increment;
            }
        //}
    }
}

/**
 * @brief Intercept the ball and spin
 */

void Robot::intercept()
{
    double min_pos_dist = 0.08;
    int speed = -1;
    double dist_bet_players = MIN_DIST_BET_PLAYERS;
    QPointF goal = medium_qpoint(data->goal.getTopLeft(),data->goal.getBottomLeft());
    QPointF go_to_position = QPointF(data->futureBallPos.x(), data->futureBallPos.y());
    //cout << "INTERCEPT" << endl;
    if(distance(this->data->player[data->playTeam][DEF_ID],this->data->player[data->playTeam][ATT_ID]) < dist_bet_players){
        if(this->getId() == DEF_ID && pos.x() < data->player[this->data->playTeam][ATT_ID].x()){
            go_to(goal,PI,0,3);
        }
        else if(this->getId() == DEF_ID && pos.x() > data->player[this->data->playTeam][ATT_ID].x()){
            defend_center();
        }
        else if(this->getId() == ATT_ID && pos.x() < data->player[this->data->playTeam][DEF_ID].x()){
            go_to(goal,PI,0,3);
        }
        else if(this->getId() == ATT_ID && pos.x() > data->player[this->data->playTeam][DEF_ID].x()){
            defend_center();
        }
    }
    else{
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
        input = get_angle(QPointF(30*cos(angle),30*sin(angle)),QPointF(target-pos))*180/PI;
    }
    else{
        phi = path.get_angle(pos,target,target_angle,this->team_id,this->player_id,have_obstacle);

        fut_pos.setX(pos.x() + 30*cos(phi));
        fut_pos.setY(pos.y() + 30*sin(phi));

        input = get_angle(QPointF(30*cos(angle),30*sin(angle)),QPointF(fut_pos - pos))*180/PI;
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
        double min_goto_dist = 0.35; //0.3
        double min_goto_dist_2 = 0.54; //0.5
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
        double min_dist = 0.25; //0.25
        if(distance(pos,target) <= min_dist){
            curr_speed *= 0.5;
            kp_ *= 0.6;
            ki_ *= 0.6;
            kd_ *= 0.8;
        }
    }
    else if(decay_mode == 3){
        //cout << "decay_mode 3" << endl;
        double min_dist = 0.31; //0.3
        double min_min_dist = 0.15;//0.18
        //cout << "ENTROOOU GO TO" << endl;
        if(distance(pos,target) <= min_min_dist){
            //cout << "PERTO" << endl;
            curr_speed *= 0.65; //0.65
            kp_ *= 0.7; //0.7
            ki_ *= 0.7; //0.7
            kd_ *= 0.7; //0.7
        }
        else if(distance(pos,target) <= min_dist){
            curr_speed *= 0.8; //0.8
            // Robo 3 novo 0.7
            //curr_speed *= 0.7;
            //cout << "LONGE" << endl;
            kp_ *= 0.8; //0.8
            ki_ *= 0.8; //0.8
            kd_ *= 0.8; //0.8
        }
    }
    else if(decay_mode == 4){
        double min_dist = 0.12; //0.15
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
        double min_dist = 0.325;  //0.27
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
        speed.first = -1.0*curr_speed;
        speed.second = -1.0*curr_speed + output;
    }
    else{
        //input = 1.0*input;
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
    this->input = get_angle(QPointF(30*cos(angle),30*sin(angle)),QPointF(data->ballPos-pos))*180/PI;
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

void Robot::go_to_kick(QPointF target, double speed_factor)
{
    double kp_ = this->KP * (speed_factor);
    double ki_ = this->KI * speed_factor;
    double kd_ = this->KD * speed_factor;
    double curr_speed = this->base_speed * speed_factor;
    //cout << curr_speed << endl;
    double out_min = -this->base_speed * speed_factor;
    double out_max = this->base_speed * speed_factor;

    input = get_angle(QPointF(30*cos(this->angle),30*sin(this->angle)),QPointF(target-this->pos))*180/PI;

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

void Robot::goleiro_semUVF()
{
    double min_distance = 0.05;
    //Maxium distance to stop spin
    double max_distance = 0.07;
    //Minimum distance to predict ball position
    double min_ball_predict = 0.06;
    //Minimum angle to spin and go to ball
    double min_angle = 90;


    bool entrou = false;
    float dist_area = 0.05, y_distance = 0.05;
    float pos_x_goleiro = 0;
    QPointF middle_goal, future_ball, ball_pos, pos_correta;
    double kp_ = this->KP;
    double ki_ = this->KI;
    double kd_ = this->KD;
    double out_min = -this->base_speed;
    double out_max = this->base_speed;
    curr_speed = this->base_speed;


    future_ball = QPoint(data->ballPos.x() + data->ballVel.x()*5, data->ballPos.y() + data->ballVel.y()*5);
    ball_pos = data->ballPos;
    if(data->playSide == RIGHT_SIDE) {
        middle_goal = medium_qpoint(data->goal.getTopRight(),data->goal.getBottomRight());
        pos_x_goleiro = middle_goal.x() - dist_area;

        if(ball_pos.y() > data->goal.getTopRight().y() + y_distance){
            future_ball.setY(data->goal.getTopRight().y() + y_distance);
            pos_correta.setX(pos_x_goleiro);
            pos_correta.setY(data->goal.getTopRight().y() + y_distance);
            entrou = true;
        }
        else if(ball_pos.y() < data->goal.getBottomRight().y() - y_distance){
            future_ball.setY(data->goal.getBottomRight().y() - y_distance);
            pos_correta.setX(pos_x_goleiro);
            pos_correta.setY(data->goal.getBottomRight().y() - y_distance);
            entrou = true;
        }
        else {
            if(data->ballVel.y() >= 0)
                future_ball.setY(data->ballPos.y());
            else
                future_ball.setY(data->ballPos.y());
            entrou = false;
        }
    }
    else if(data->playSide == LEFT_SIDE) {
        middle_goal = medium_qpoint(data->goal.getTopLeft(),data->goal.getBottomLeft());
        pos_x_goleiro = middle_goal.x() + dist_area;

        if(ball_pos.y() > data->goal.getTopLeft().y() + y_distance){
            future_ball.setY(data->goal.getTopLeft().y() + y_distance);
            pos_correta.setX(pos_x_goleiro);
            pos_correta.setY(data->goal.getTopLeft().y() + y_distance);
            entrou = true;
        }
        else if(ball_pos.y() < data->goal.getBottomLeft().y() - y_distance){
            future_ball.setY(data->goal.getBottomLeft().y() - y_distance);
            pos_correta.setX(pos_x_goleiro);
            pos_correta.setY(data->goal.getBottomLeft().y() - y_distance);
            entrou = true;
        }
        else {
            if(data->ballVel.y() >= 0)
                future_ball.setY(data->ballPos.y());
            else
                future_ball.setY(data->ballPos.y());
            entrou = false;
        }
    }

    QPointF aux;

    aux.setX(pos_x_goleiro);
    //cout << future_ball.y() << endl;
    aux.setY(future_ball.y());
    //cout << angle*180/PI << endl;
    if(distance(pos, data->ballPos) > 30)
        this->input = (get_angle(QPointF(30*cos(angle),30*sin(angle)),QPointF(aux - pos)))*180/PI;
    //else
    //	go_to_debug();
    //cout << this->input << endl;
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
    double ball_player_angle = fabs(get_angle(QPointF(30*cos(angle),30*sin(angle)),QPointF(data->ballPos - pos)))*180/PI;

    if(distance(pos, data->ballPos) <= min_distance && ball_player_angle <= min_angle){
        defend_spin = true;
    }
    else if(distance(pos,data->ballPos) >= max_distance && defend_spin == true){
        defend_spin = false;
    }
    bool entrou_go_to = false;
    if(defend_spin == true){
        spin();
    }
    else if(distance(pos, data->ballPos) <= 0.07)
    {
        //go_to(data->ballPos,PI,0,3);
        intercept();
        entrou_go_to = true;
    }
    else if(distance(pos, data->ballPos) > 0.09){
        entrou_go_to = false;
    }
    QPointF aux_int;
    aux_int.setX(aux.x());
    aux_int.setY(aux.y());
    if(entrou_go_to == false && defend_spin == false && (abs(pos.y() - data->ballPos.y()) < 10 || abs(pos.y() - future_ball.y()) < 10) && distance(pos, aux_int) < 10)
        speed.first = speed.second = 0;
    if(entrou_go_to == false && defend_spin == false)
        this->sendFIRA(this->getId(),speed.first,speed.second);
}

/**
 * @brief Align with the ball-y inside the goal line and spins if the
 * ball is close
 */

void Robot::defend_goal()
{
    //double ball_play_dist_min = 70;
    //Distance between player-x and goal line-x
    double x_distance = 0.06;
    //Maximum distance between player-y and goal line-y
    double y_distance = 0.03; //Conferir
    //Ball player distance to go to ball
    double ball_play_dist = 0.3;
    //Enemy distance
    double enem_dist = 0.3;
    //Distance to future point
    double x_fut_dist = 0.1;
    //Minimum ball distance to spin
    double min_distance = 0.075;
    //Maxium distance to stop spin
    double max_distance = 0.1;
    //Minimum distance to predict ball position
    double min_ball_predict = 0.2; //0.05
    //Minimum angle to spin and go to ball
    double min_angle = 70;

    //Angle between player and ball
    double ball_player_angle = fabs(get_angle(QPointF(30*cos(angle),30*sin(angle)),QPointF(data->ballPos - pos)))*180/PI;
    if(ball_player_angle >= 90.0){
        ball_player_angle = 180.0 - ball_player_angle;
    }
    QPointF goal, future_ball, goal_intersect, ball_pos;
    future_ball = QPointF(data->ballPos.x() + data->ballVel.x()*1.0, data->ballPos.y() + data->ballVel.y()*1.0);

    if(data->playSide == LEFT_SIDE){
        goal = medium_qpoint(data->goal.getTopLeft(),data->goal.getBottomLeft());
        goal_intersect = line_intersect(data->ballPos,future_ball,QPointF(data->goal.getTopLeft() + QPointF(x_distance + x_fut_dist,y_distance)),QPointF(data->goal.getBottomLeft() + QPointF(x_distance + x_fut_dist,-y_distance)));
        //goal_intersect = line_intersect(data->ballPos,future_ball,QPoint(data->goal.getTopLeft() + QPoint(x_distance + x_fut_dist,y_distance)),QPoint(data->goal.getBottomLeft() + QPoint(x_distance + x_fut_dist,-y_distance)));
        ball_pos = data->ballPos;
        goal.setX(goal.x() + x_distance);
        if(data->ballVel.manhattanLength() >= min_ball_predict && data->ballVel.x() < 0){
            ball_pos = goal_intersect;
        }
        if(ball_pos.y() > data->goal.getTopLeft().y() + y_distance){
            goal.setY(data->goal.getTopLeft().y() + y_distance);
        }
        else if(ball_pos.y() < data->goal.getBottomLeft().y() - y_distance){
            goal.setY(data->goal.getBottomLeft().y() - y_distance);
        }
        else{
            //goal.setY(ball_pos.y());
            goal.setY(data->futureBallPos.y());
        }
    }
    else{
        goal = medium_qpoint(data->goal.getTopRight(),data->goal.getBottomRight());
        goal_intersect = line_intersect(data->ballPos,future_ball,QPointF(data->goal.getTopRight() + QPointF(-x_distance - x_fut_dist,y_distance)),QPointF(data->goal.getBottomRight() + QPointF(-x_distance - x_fut_dist,-y_distance)));
        ball_pos = data->ballPos;
        goal.setX(goal.x() - x_distance);
        if(data->ballVel.manhattanLength() >= min_ball_predict && data->ballVel.x() > 0){
            ball_pos = goal_intersect;
        }
        if(ball_pos.y() > data->goal.getTopRight().y() + y_distance){
            goal.setY(data->goal.getTopRight().y() + y_distance);
        }
        else if(ball_pos.y() < data->goal.getBottomRight().y() - y_distance){
            goal.setY(data->goal.getBottomRight().y() - y_distance);
        }
        else{
            //goal.setY(ball_pos.y());
            goal.setY(data->futureBallPos.y());
        }
    }
    //Spin mode
    if(distance(pos,data->ballPos) <= min_distance && ball_player_angle >= min_angle){ //trocado <= por >=
        defend_spin = true;
        cout << "ENTROOOOU                   SPIN" << endl;
    }
    else if(distance(pos,data->ballPos) >= max_distance && defend_spin == true){
        defend_spin = false;
    }

    if(defend_spin == true){
        if(is_inside(pos,data->area[data->playSide]))
            spin_goleiro();
        else
            intercept(); //MODIFIQUEI
        //go_to_debug();



        //	cout << "entrou spin" << endl;
    }
    //Alterado aqui
    else if(distance(data->ballPos,pos) <= ball_play_dist && ball_player_angle <= min_angle){// && is_inside(data->ballPos,data->area[data->playSide])){
        if(distance(data->ballPos,pos) >= 0.07)
        {
            QPointF goal;
            goal = medium_qpoint(data->goal.getTopLeft(),data->goal.getBottomLeft());
            //goal.setY(data->ballPos.y());
            goal.setY(data->futureBallPos.y());
            position(goal,PI/2,1,1);
        }
        else
            go_to(data->ballPos,-999,0,1);

        cout << "ANGULO JOGADOR BOLA :::::::    " <<ball_player_angle <<endl;
    }
    else if(pos.y() > goal.y()){
        if(distance(pos,goal) >= enem_dist)
            position(goal,PI/2,1,1);
        else
            position(goal,PI/2,0,1);

        cout << "ANGULO JOGADOR BOLA :::::::    " <<ball_player_angle <<endl;
    }
    else{
        if(distance(pos,goal) >= enem_dist)
            position(goal,-PI/2,1,1);
        else
            position(goal,-PI/2,0,1);

        cout << "ANGULO JOGADOR BOLA :::::::    " <<ball_player_angle <<endl;
    }
}

void Robot::stopRobot(){
    this->sendFIRA(this->getId(),0,0);
}

void Robot::defend_goal_Left()
{
    QPointF ball_pos = data->futureBallPos;
    QPointF goal = data->goal.getTopLeft();
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
    QPointF goal = data->goal.getTopRight();
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
    double x_distance_goal = 0.2;
    double ball_goal_dist = 0.45;

    double y_top = 1.05;
    double y_bot = 0.25;
    double y_center = 0.65;

    QPointF top_right = QPointF(data->max_field.x()-x_distance_goal, y_top - 0.2);
    QPointF center_right = QPointF(data->max_field.x()-x_distance_goal, y_center);
    QPointF bot_right = QPointF(data->max_field.x()-x_distance_goal, y_bot + 0.2);

    QPointF top_left = QPointF(data->min_field.x()+x_distance_goal, y_top - 0.2);
    QPointF center_left = QPointF(data->min_field.x()+x_distance_goal, y_center);
    QPointF bot_left = QPointF(data->min_field.x()+x_distance_goal, y_bot + 0.2);

    QPointF middle_top = QPointF(data->middle_field.x(),data->middle_field.y()+0.2);
    QPointF middle_bot = QPointF(data->middle_field.x(),data->middle_field.y()-0.2);

    if(this->data->playSide == LEFT_SIDE){
        if((data->ballPos.x() <= 0.37 && data->ballPos.y() <= 0.28)/* && flag_Corner_Fight*/){
            QPointF secure_point = QPointF(bot_left.x() - 0.13, bot_left.y());
            position(secure_point, -PI/2, 0, 1);
        }
        else if((data->ballPos.x() <= 0.37 && data->ballPos.y() >= 1.02)/* && flag_Corner_Fight*/){
            QPointF secure_point = QPointF(top_left.x() - 0.13, top_left.y());
            position(secure_point, PI/2, 0, 1);
        }
        else if(distance(this->data->ballPos,center_left) <= ball_goal_dist || data->ballPos.x() <= 0.45 && distance(data->player[data->playTeam][DEF_ID], data->ballPos) > 0.35 && distance(data->player[data->playTeam][ATT_ID], data->ballPos) > 0.35){
            if(is_inside(data->ballPos, data->area[LEFT_SIDE]))
                intercept();
            else
                go_to_debug();
        }
        else{
            if(data->ballPos.y() >= y_top){
                QPointF vec_top = middle_top - top_left;
                QPointF vec_ball_goalkeeper = data->ballPos - top_left;
                double angle_ball_goalKeeper = get_angle(vec_top,vec_ball_goalkeeper);
                position(top_left, angle_ball_goalKeeper, 0, 1);
            }
            else if(data->ballPos.y() <= y_bot){
                QPointF vec_bot = middle_bot - bot_left;
                QPointF vec_ball_goalkeeper = data->ballPos - bot_left;
                double angle_ball_goalKeeper = get_angle(vec_bot,vec_ball_goalkeeper);
                position(bot_left, angle_ball_goalKeeper, 0, 1);
            }
            else{
                QPointF vec_middle = data->middle_field - center_left;
                QPointF vec_ball_goalkeeper = data->ballPos - center_left;
                double angle_ball_goalKeeper = get_angle(vec_middle,vec_ball_goalkeeper);
                if(data->ballPos.y() > top_left.y())
                    position(top_left, angle_ball_goalKeeper, 0, 1);
                else if(data->ballPos.y() < bot_left.y())
                    position(bot_left, angle_ball_goalKeeper, 0, 1);
                else position(QPointF(center_left.x(),data->ballPos.y()), angle_ball_goalKeeper, 0, 1);
            }
        }
    }
    else{
        if((data->ballPos.x() >= 1.13 && data->ballPos.y() <= 0.28) /*&& flag_Corner_Fight*/){
            QPointF secure_point = QPointF(bot_right.x() + 0.13, bot_right.y());
            position(secure_point, PI/2, 0, 1);
        }
        else if((data->ballPos.x() >= 1.13 && data->ballPos.y() >= 1.02) /*&& flag_Corner_Fight*/){
            QPointF secure_point = QPointF(top_right.x() + 0.13, top_right.y());
            position(secure_point, -PI/2, 0, 1);
        }
        else if((distance(this->data->ballPos,center_right) <= ball_goal_dist || data->ballPos.x() >= 1.05) && distance(data->player[data->playTeam][DEF_ID], data->ballPos) > 0.35 && distance(data->player[data->playTeam][ATT_ID], data->ballPos) > 0.35){
            if(is_inside(data->ballPos, data->area[RIGHT_SIDE]))
                intercept();
            else
                go_to_debug();
        }
        else{
            if(data->ballPos.y() >= y_top){
                QPointF vec_top = middle_top - top_right;
                QPointF vec_ball_goalkeeper = data->ballPos - top_right;
                double angle_ball_goalKeeper = get_angle(vec_top,vec_ball_goalkeeper);
                position(top_right, angle_ball_goalKeeper, 0, 1);
            }
            else if(data->ballPos.y() <= y_bot){
                QPointF vec_bot = middle_bot - bot_right;
                QPointF vec_ball_goalkeeper = data->ballPos - bot_right;
                double angle_ball_goalKeeper = get_angle(vec_bot,vec_ball_goalkeeper);
                position(bot_right, angle_ball_goalKeeper, 0, 1);
            }
            else{
                if(distance(this->data->ballPos,center_right) <= ball_goal_dist){
                    go_to_debug();
                }
                else{
                    QPointF vec_middle = data->middle_field - center_right;
                    QPointF vec_ball_goalkeeper = data->ballPos - center_right;
                    double angle_ball_goalKeeper = get_angle(vec_middle,vec_ball_goalkeeper);
                    if(data->ballPos.y() > top_right.y())
                        position(top_right, angle_ball_goalKeeper, 0, 1);
                    else if(data->ballPos.y() < bot_right.y())
                        position(bot_right, angle_ball_goalKeeper, 0, 1);
                    else position(QPointF(center_right.x(),data->ballPos.y()), angle_ball_goalKeeper, 0, 1);
                }
            }
        } 
    }
}

void Robot::defend_goal_LARC_2(){
    //Distance between player-x and goal line-x
    double x_distance = 0.06;
    //Ball player distance to go to ball
    bool flag_position = false;
    double max_dist_x = data->middle_field.x()+0.1;
    double ball_goal_dist = 0.45;

    QPointF goal;
    double speed = 0.0;

    if(this->data->playSide == LEFT_SIDE){
        goal = medium_qpoint(data->goal.getTopLeft(),data->goal.getBottomLeft());
        goal.setX(goal.x() + x_distance);
        cout << "DISTANCIA:       " << distance(this->data->ballPos,goal) << endl;
        cout << "FUTURE BALL POS:    " << this->data->futureBallPos.x() << endl;
        if(this->getAngle() != -PI/2 && distance(this->data->ballPos,goal) > max_dist_x){
           position(goal, -PI/2, 1, 1);
           //cout << "ANGULO :::::::::::::    " <<this->getAngle()*180/PI << endl;
           cout << "ENTROU :::::::::::::    1 " <<endl;
        }
        else if(/*this->getAngle() != -PI/2 && */distance(this->data->ballPos,goal) <= max_dist_x){
            if(distance(this->data->ballPos,goal) <= ball_goal_dist){
                go_to(this->data->futureBallPos,PI, false,3);
                cout << "ENTROU :::::::::::::    2 " <<endl;
            }
            else{
                defend_goal_Left();
                cout << "ENTROU :::::::::::::    4 " <<endl;
            }
        }/*
        else if(this->getAngle() >= 1.3926 && this->getAngle() <= 1,7453){
            cout << "ENTROU :::::::::::::    3 " <<endl;
                if(distance(this->data->futureBallPos, this->pos) < 0.12){
                    go_to_debug();
                }
                else{
                    if(this->data->ballPos.y() > this->pos.y()){
                        goal.setY(this->pos.y() + (this->data->ballPos.y() - this->pos.y()));
                        if(goal.y() >= this->data->goal.getTopLeft().y()){
                           position(QPointF(this->data->min_field.x(),this->data->goal.getTopLeft().y()-0.02), -PI/2, 1, 1);
                            flag_position = true;
                        }
                        else{
                            speed = -600;
                        }
                    }
                    else{
                        goal.setY(this->pos.y() - (this->pos.y() - this->data->ballPos.y()));
                        if(goal.y() <= this->data->goal.getBottomLeft().y()){
                           position(QPointF(this->data->min_field.x(),this->data->goal.getBottomLeft().y()+0.02), -PI/2, 1, 1);
                           flag_position = true;
                        }
                        else{
                           speed = 600;
                        }
                    }
                    if(flag_position)
                        this->sendFIRA(speed,speed,this->getTeamId(),this->getId());
            }
        }*/
    }

    else{
        goal = medium_qpoint(data->goal.getTopRight(),data->goal.getBottomRight());
        goal.setX(goal.x() - x_distance);
        if(this->getAngle() != PI/2 && distance(this->data->ballPos,goal) > ball_goal_dist){
           position(goal, PI/2, 1, 1);
           //cout << "ENTROU :::::::::::::    1 " <<endl;
        }
        else if(this->getAngle() != PI/2 && distance(this->data->ballPos,goal) <= ball_goal_dist){
            go_to(this->data->ballPos,PI, false,3);
            //cout << "ENTROU :::::::::::::    2 " <<endl;
        }
        else if(this->getAngle() >= 1.3926 && this->getAngle() <= 1,7453){
            //cout << "ENTROU :::::::::::::    3 " <<endl;

                if(distance(this->data->ballPos, this->pos) < 0.1){
                    go_to_debug();
                }

                else{
                    if(this->data->ballPos.y() > this->pos.y()){
                        goal.setY(this->pos.y() + (this->data->ballPos.y() - this->pos.y()));
                        speed = 600;
                    }

                    else{
                        goal.setY(this->pos.y() - (this->pos.y() - this->data->ballPos.y()));
                        speed = -600;
                    }

                    this->sendFIRA(this->getId(),speed,speed);
            }
        }
    }
}


void Robot::defend_goal_new(){
    //Distance between player-x and goal line-x
    double x_distance = 0.06;
    //Ball player distance to go to ball
    bool flag_position = false;
    double ball_play_dist = 0.45;

    QPointF goal;
    double speed = 0.0;

    if(this->data->playSide == LEFT_SIDE){
        goal = medium_qpoint(data->goal.getTopLeft(),data->goal.getBottomLeft());
        goal.setX(goal.x() + x_distance);

        if(this->getAngle() != -PI/2 && distance(this->data->ballPos,goal) > ball_play_dist){
           position(goal, -PI/2, 1, 1);
           //cout << "ANGULO :::::::::::::    " <<this->getAngle()*180/PI << endl;
           cout << "ENTROU :::::::::::::    1 " <<endl;
        }
        else if(/*this->getAngle() != -PI/2 && */distance(this->data->ballPos,goal) <= ball_play_dist){
            go_to(this->data->futureBallPos,PI, false,3);
            cout << "ENTROU :::::::::::::    2 " <<endl;
        }
        else if(this->getAngle() >= 1.3926 && this->getAngle() <= 1,7453){
            cout << "ENTROU :::::::::::::    3 " <<endl;
                if(distance(this->data->futureBallPos, this->pos) < 0.12){
                    go_to_debug();
                }
                else{
                    if(this->data->ballPos.y() > this->pos.y()){
                        goal.setY(this->pos.y() + (this->data->ballPos.y() - this->pos.y()));
                        if(goal.y() >= this->data->goal.getTopLeft().y()){
                           position(QPointF(this->data->min_field.x(),this->data->goal.getTopLeft().y()-0.02), -PI/2, 1, 1);
                            flag_position = true;
                        }
                        else{
                            speed = -600;
                        }
                    }
                    else{
                        goal.setY(this->pos.y() - (this->pos.y() - this->data->ballPos.y()));
                        if(goal.y() <= this->data->goal.getBottomLeft().y()){
                           position(QPointF(this->data->min_field.x(),this->data->goal.getBottomLeft().y()+0.02), -PI/2, 1, 1);
                           flag_position = true;
                        }
                        else{
                           speed = 600;
                        }
                    }
                    if(flag_position)
                        this->sendFIRA(this->getId(),speed,speed);
            }
        }
    }

    else{
        goal = medium_qpoint(data->goal.getTopRight(),data->goal.getBottomRight());
        goal.setX(goal.x() - x_distance);
        if(this->getAngle() != PI/2 && distance(this->data->ballPos,goal) > ball_play_dist){
           position(goal, PI/2, 1, 1);
           //cout << "ENTROU :::::::::::::    1 " <<endl;
        }
        else if(this->getAngle() != PI/2 && distance(this->data->ballPos,goal) <= ball_play_dist){
            go_to(this->data->ballPos,PI, false,3);
            //cout << "ENTROU :::::::::::::    2 " <<endl;
        }
        else if(this->getAngle() >= 1.3926 && this->getAngle() <= 1,7453){
            //cout << "ENTROU :::::::::::::    3 " <<endl;

                if(distance(this->data->ballPos, this->pos) < 0.1){
                    go_to_debug();
                }

                else{
                    if(this->data->ballPos.y() > this->pos.y()){
                        goal.setY(this->pos.y() + (this->data->ballPos.y() - this->pos.y()));
                        speed = 600;
                    }

                    else{
                        goal.setY(this->pos.y() - (this->pos.y() - this->data->ballPos.y()));
                        speed = -600;
                    }

                    this->sendFIRA(this->getId(),speed,speed);
            }
        }
    }
}

void Robot::defend_middle()
{
    double x_point = 0, y_point= 0;
    double x_var = -0.4;
    //Minimum ball distance to spin
    double min_distance = 0.08;
    //Distance between border and position to stop
    double y_distance = 0.05;
    //Maxium distance to stop spin
    double max_distance = 0.09;
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
                if(data->ballPos.y() + 0.03 > data->middle_field.y())
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
                if(data->ballPos.y() + 0.03 > data->middle_field.y())
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
        position(QPointF(x_point,y_point), angle, 1, 4);
        //}
    }
}

void Robot::defend_middle_attack()
{
    double x_point = 0, y_point= 0;
    double x_var = -0.28;
    //Minimum ball distance to spin
    double min_distance = 0.08;
    //Distance between border and position to stop
    double y_distance = 0.15;
    //Maxium distance to stop spin
    double max_distance = 0.09;
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
                if(data->ballPos.y() + 0.02 > data->middle_field.y())
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
                if(data->ballPos.y() + 0.02 > data->middle_field.y())
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
        position(QPointF(x_point,y_point), angle, 1, 4);
        //}
    }
}

void Robot::defend_attack()
{
    double x_point = 0, y_point= 0;
    double x_var = 0.3;
    //Minimum ball distance to spin
    double min_distance = 0.08;
    //Distance between border and position to stop
    double y_distance = 0.15;
    //Maxium distance to stop spin
    double max_distance = 0.09;
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
                if(data->ballPos.y() + 0.03 > data->middle_field.y())
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
                if(data->ballPos.y() + 0.03 > data->middle_field.y())
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
        position(QPointF(x_point,y_point), angle, 1, 4);
        //}
    }
}

void Robot::defend_block(int e_id)
{
    int enemy_team = !(data->playTeam);
    QPointF enemy_pos;
    double x_dist = 0.04;
    double dist_goal = 99999, dist_ball = 99999, dist_other_goal = 0;
    int goal_id = -1, ball_id = -1, far_id = -1;
    QPointF goal_center = medium_qpoint(data->goal.getTopLeft(),data->goal.getBottomLeft());
    if(data->playSide == RIGHT_SIDE){
        x_dist *= -1;
        goal_center = medium_qpoint(data->goal.getTopRight(),data->goal.getBottomRight());
    }
    for(int i = 0; i < 3; ++i){
        enemy_pos = data->player[enemy_team][i];
        if(enemy_pos != QPointF(-5,481)){
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
            QPointF go_point = data->player[enemy_team][goal_id];
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
            QPointF go_point = data->player[enemy_team][ball_id];
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
            QPointF go_point = data->player[enemy_team][far_id];
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

void Robot::position(QPointF point, double angle, bool has_obstacle, int speed_mode)
{
    double min_pos_dist = 0.005;
    double med_pos_dist = 0.015;
    double dist_to_point = distance(pos, point);
    if(speed_mode == 1){
        double max_distance = 0.35;
        /*if(distance(pos, point) <= min_pos_dist && this->getAngle() != PI/2.0){
            set_angle(angle);
        }*/
        if(dist_to_point <= min_pos_dist /*|| this->getAngle() == PI/2.0*/){
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

void Robot::set_angle(QPointF heading_point)
{
    double kp_ = -this->KP;
    double ki_ = -this->KI;
    double kd_ = -this->KD;
    double out_min = -this->spin_speed;
    double out_max = this->spin_speed;
    double angle_aux1 = (get_angle(QPointF(30*cos(angle),30*sin(angle)),QPointF(heading_point - pos)))*180/PI;
    double angle_aux2 = (get_angle(QPointF(30*cos(angle),30*sin(angle)),QPointF(heading_point - pos)) + PI)*180/PI;
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

void Robot::go_to_defend_penalty(){
    QPointF future_ball, goal;
    future_ball = QPointF(data->ballPos.x() + data->ballVel.x()*1.0, data->ballPos.y() + data->ballVel.y()*1.0);

    if(data->playSide == LEFT_SIDE)
        goal = medium_qpoint(data->goal.getTopLeft(),data->goal.getBottomLeft());
    else goal = medium_qpoint(data->goal.getTopRight(),data->goal.getBottomRight());
    speed.first = 2000;
    speed.second = 2000;
    this->sendFIRA(this->getId(),speed.first,speed.second);
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
