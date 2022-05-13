#include "core.h"

Core::Core()
{
    codeArguments = new Arguments();
}

void Core::updateArguments(int argc, char** argv){
    this->codeArguments->update(argc, argv);
    this->codeArguments->print();
}

void Core::verifyStatus(){
    if(this->refereeClient->getLastFoul() == VSSRef::Foul::GAME_ON){
        this->data->playStatus = ON;
    }
    else if (this->refereeClient->getLastFoul() != VSSRef::Foul::HALT && this->refereeClient->getLastFoul() != VSSRef::Foul::STOP){
        this->data->playStatus = OFF;
        emit repositioningAlert();
    }
    else {
        this->data->playStatus = OFF;
    }
}

void Core::setTestCondition(bool _test){
    this->testCondition = _test;
}

bool Core::getTestCondition(){
    return this->testCondition;
}

void Core::ballInfo(){
    cout << "X: " << this->data->ballPos.x() << endl;
    cout << "Y: " << this->data->ballPos.y() << endl;
    cout << "X FUT: " << this->data->futureBallPos.x() << endl;
    cout << "Y FUT: " << this->data->futureBallPos.y() << endl;
    //cout << "Vx: " << this->data->ballVel.x() << endl;
    //cout << "Vy: " << this->data->ballVel.y() << endl;
}

void Core::start(){
    //Creating client objects
    this->visionClient = new VisionClient(this->codeArguments->get_ip_Multicast_FIRA_Vision(),this->codeArguments->get_port_FIRA_Vision());
    this->actuatorClient = new ActuatorClient(this->codeArguments->get_ip_Local_Machine_FIRA(), this->codeArguments->get_port_FIRA_Actuator());
    this->refereeClient = new RefereeClient(this->codeArguments->get_ip_Multicast_REF(), this->codeArguments->get_port_REF());
    this->replacerClient = new ReplacerClient(this->codeArguments->get_ip_Multicast_REF(), this->codeArguments->get_port_REF_REPLACER());

    //Setting team color for actuator and replacer object
    VSSRef::Color teamColor = this->codeArguments->get_teamColor() ? VSSRef::Color::YELLOW : VSSRef::Color::BLUE;
    this->actuatorClient->setTeamColor(teamColor);
    this->replacerClient->setTeamColor(teamColor);

    //Connecting clients that receive information
    this->visionClient->connect();
    this->refereeClient->connect();

    //Initialize
    this->initializeTeam(this->codeArguments->get_teamColor(), this->codeArguments->get_teamFormation());

    //Defining events and actions

    //Events involving FIRASim
    QObject::connect(this->visionClient, SIGNAL(dataReceived()), this, SLOT(update()));
    QObject::connect(this, SIGNAL(readyToProcess()), this, SLOT(process()));

    //Events involving Referee
    QObject::connect(this->refereeClient, SIGNAL(refereeAlert()), this, SLOT(verifyStatus()));
    QObject::connect(this, SIGNAL(repositioningAlert()), this, SLOT(reposition()));
}

void Core::initializeTeam(bool team, int formation){
    //Defininf test condition
    this->setTestCondition(this->codeArguments->getTestCondition());

    // Defining team and side
    if(team){   // Yellow Team
        this->data->playTeam = TEAM_YELLOW;
        this->data->playSide = RIGHT_SIDE;
        this->data->goal = FieldSpace(QPoint(0,770),QPoint(1500,900),QPoint(0,530),QPoint(1500,400));
    }
    else{       // Blue Team
        this->data->playTeam = TEAM_BLUE;
        this->data->playSide = LEFT_SIDE;
        this->data->goal = FieldSpace(QPoint(0,900),QPoint(1500,770),QPoint(0,400),QPoint(1500,530));
    }

    // Defining Formation
    switch(formation){
        case 1:
            this->data->formation1 = true;
            break;
        case 2:
            this->data->formation2 = true;
            break;
        case 3:
            this->data->formation3 = true;
            break;
        case 4:
            this->data->formation4 = true;
            break;
        case 5:
            this->data->formation5 = true;
            break;
        case 6:
            this->data->formation6 = true;
            break;
    }

    // Defining field size
    data->max_field.setX(min(data->field.getBottomRight().x(),data->field.getTopRight().x()));
    data->max_field.setY(min(data->field.getTopLeft().y(),data->field.getTopRight().y()));
    data->min_field.setX(max(data->field.getBottomLeft().x(),data->field.getTopLeft().x()));
    data->min_field.setY(max(data->field.getBottomLeft().y(),data->field.getBottomRight().y()));

    // Defining area size
    data->area[LEFT_SIDE] = FieldSpace(QPoint(0,1050),QPoint(200, 1050),QPoint(0,250),QPoint(200,250));
    data->area[RIGHT_SIDE] = FieldSpace(QPoint(1300,1050),QPoint(1500, 1050),QPoint(1300,250),QPoint(1500,250));

    // Initializing ball variables
    data->ballVel = QPoint(0,0);
    data->futureBallPos = QPoint(750,650);
    data->ballPos = QPoint(750,650);

    // PID and Speed goalkeeper (robot 0)
    double fast_speed = 80;  //180
    double spin_speed = fast_speed*2.5;
    double kp_base = -1.0;     //-1.8
    double ki_base = -0.0;
    double kd_base = -0.0;     //-38.1

    // PID and Speed defender (robot 1)
    double fast_speed2 = 200;   //200   //300
    double spin_speed2 = fast_speed2*1.5;
    double kp_base2 = -2.4;    //-2.65 //-4.65
    double ki_base2 = -0.0;
    double kd_base2 = -40.0;    //-50.5  //-95.5

    //PID and Speed stricker (robot 2)
    double fast_speed3_r = 200; //110   //400
    double spin_speed3_r = fast_speed3_r*1.5;
    double kp_base3_r = -2.4;   //-3.0 //-5.65
    double ki_base3_r = -0.0;
    double kd_base3_r = -40.0;  //-15.0 //-100.5

    // Configuring robots
    //int _team_id, int _id, double _kp, double _ki, double _kd, double _base_speed, double_spin_speed
    //this->player[this->data->playTeam][ID_0]->setRobot(this->data->playTeam, ID_0, kp_base, ki_base, kd_base, fast_speed, spin_speed);
    this->player[this->data->playTeam][ID_0] = new Robot(this->data->playTeam, ID_0, kp_base, ki_base, kd_base, fast_speed, spin_speed);
    this->player[!this->data->playTeam][ID_0] = new Robot(this->data->playTeam, ID_0, kp_base, ki_base, kd_base, fast_speed, spin_speed);
    this->player[this->data->playTeam][ID_0]->setActuator(actuatorClient);

    //this->player[this->data->playTeam][ID_1]->setRobot(this->data->playTeam, ID_1, kp_base2, ki_base2, kd_base2, fast_speed2, spin_speed2);
    this->player[this->data->playTeam][ID_1] = new Robot(this->data->playTeam, ID_1, kp_base2, ki_base2, kd_base2, fast_speed2, spin_speed2);
    this->player[!this->data->playTeam][ID_1] = new Robot(this->data->playTeam, ID_1, kp_base2, ki_base2, kd_base2, fast_speed2, spin_speed2);
    this->player[this->data->playTeam][ID_1]->setActuator(actuatorClient);

    //this->player[this->data->playTeam][ID_2]->setRobot(this->data->playTeam, ID_2, kp_base3_r, ki_base3_r, kd_base3_r, fast_speed3_r, spin_speed3_r);
    this->player[this->data->playTeam][ID_2] = new Robot(this->data->playTeam, ID_2, kp_base3_r, ki_base3_r, kd_base3_r, fast_speed3_r, spin_speed3_r);
    this->player[!this->data->playTeam][ID_2] = new Robot(this->data->playTeam, ID_2, kp_base3_r, ki_base3_r, kd_base3_r, fast_speed3_r, spin_speed3_r);
    this->player[this->data->playTeam][ID_2]->setActuator(actuatorClient);
}

void Core::update(){
    //Updating the last environment
    fira_message::sim_to_ref::Environment lastEnv = this->visionClient->getLastEnvironment();

    // Updating information of each robot
    if(lastEnv.has_frame()){
        // Get last frame to update the variables
        fira_message::Frame lastFrame = lastEnv.frame();

        // Updating information of each robot
        for(int i = 0; i < NUMBER_OF_TEAMS; i++){
            for(int j = 0; j < NUMBER_OF_ROBOTS; j++){
                if(i == TEAM_BLUE){
                    this->player[TEAM_BLUE][j]->setX(lastFrame.robots_blue(j).x() * 1000 + 750);    // Adjust to work only in the first quadrant
                    this->player[TEAM_BLUE][j]->setY(lastFrame.robots_blue(j).y() * 1000 + 650);    // Adjust to work only in the first quadrant
                    this->player[TEAM_BLUE][j]->setAngle(lastFrame.robots_blue(j).orientation());
                    data->player[TEAM_BLUE][j] = QPoint(player[TEAM_BLUE][j]->getX(),player[TEAM_BLUE][j]->getY());
                    data->player_angle[TEAM_BLUE][j] = 1.0*player[TEAM_BLUE][j]->getAngle();
                }
                else{
                    this->player[TEAM_YELLOW][j]->setX(lastFrame.robots_yellow(j).x()* 1000 + 750);    // Adjust to work only in the first quadrant
                    this->player[TEAM_YELLOW][j]->setY(lastFrame.robots_yellow(j).y()* 1000 + 650);    // Adjust to work only in the first quadrant
                    this->player[TEAM_YELLOW][j]->setAngle(lastFrame.robots_yellow(j).orientation());
                    data->player[TEAM_YELLOW][j] = QPoint(player[TEAM_YELLOW][j]->getX(),player[TEAM_YELLOW][j]->getY());
                    data->player_angle[TEAM_YELLOW][j] = 1.0*player[TEAM_YELLOW][j]->getAngle();
                }
            }
        }

        // Updating information of ball
        data->lastBallPos = this->data->ballPos;
        data->ballPos = QPoint(lastFrame.ball().x() * 1000 + 750, lastFrame.ball().y() * 1000 + 650);  // Adjust to work only in the first quadrant
        data->lastBallVel = data->ballVel;
        data->ballVel = QPoint(lastFrame.ball().vx() * 1000, lastFrame.ball().vy() * 1000);

        //Determing future ball position
        data->futureBallPos.setX(data->ballPos.x() + data->ballVel.x() * 4 * TIME_TO_RECEIVE_DATA + (0.5 * (data->ballVel.x() - data->lastBallVel.x()) * 16 * TIME_TO_RECEIVE_DATA));
        data->futureBallPos.setY(data->ballPos.y() + data->ballVel.y() * 4 * TIME_TO_RECEIVE_DATA + (0.5 * (data->ballVel.y() - data->lastBallVel.y()) * 16 * TIME_TO_RECEIVE_DATA));

        //Showing ball information
        //ballInfo();

        emit readyToProcess();
    }
}

void Core::process(){
    //Defining the play
    data->mode = bola_ataque;
    double x_attack =  200;
    double x_defense = 350;
    if(this->data->playSide == LEFT_SIDE){
        if(this->data->ballPos.x() >= this->data->middle_field.x() + x_attack){ //Attack
            data->mode = bola_ataque;
        }
        else{   //Defense
            if(this->data->ballPos.x() >= this->data->middle_field.x() - x_defense)
                data->mode = bola_meio;
            else if(is_inside(this->data->futureBallPos,this->data->area[LEFT_SIDE]))
                data->mode = bola_area;
            else
                data->mode = bola_defesa;
        }
    }
    else{
        if(this->data->ballPos.x() <= this->data->middle_field.x() - x_attack){ //Attack
            data->mode = bola_ataque;
        }
        else{   //Defense
            if(this->data->ballPos.x() <= this->data->middle_field.x() + x_defense)
                data->mode = bola_meio;
            else if(is_inside(this->data->futureBallPos,this->data->area[RIGHT_SIDE]))
                data->mode = bola_area;
            else
                data->mode = bola_defesa;
        }
    }

    if(!this->getTestCondition()){
        if(this->data->playStatus == ON){
            /////////////////////////////////////////Penalty action/////////////////////////////////////////////////
            if(data->robotPenalty[ID_2] >= 0){
                if(data->penaltyAction == 0){
                    player[data->playTeam][ID_2]->go_to_penalty();
                    data->robotPenalty[ID_2]++;
                }
                else if(data->penaltyAction == 1){
                    player[data->playTeam][ID_2]->intercept();
                    data->robotPenalty[ID_2]++;
                }
                else{
                    player[data->playTeam][ID_2]->spin_penalty();
                    data->robotPenalty[ID_2]++;
                }
                if(data->robotPenalty[ID_2] >= max_cont_penalty){
                    data->robotPenalty[ID_2] = -1;
                    data->penalty = false;
                    data->speed_spin_increment = 3;
                    this->player[data->playTeam][ID_2]->position(data->middle_field,180,1,2);
                }
            }
            if(data->contDefendPenalty >= 0){
                data->contDefendPenalty++;
                this->player[data->playTeam][ID_0]->setKD(-38.1);
                this->player[data->playTeam][ID_0]->setKP(-1.9);
                this->player[data->playTeam][ID_0]->setBaseSpeed(185);
                player[data->playTeam][ID_0]->go_to_debug();
                if(data->contDefendPenalty >= max_cont_defend_penalty){
                    data->contDefendPenalty = -1;
                    data->defendPenalty = false;
                }
            }
            /////////////////////////////////////////////////////////////////////////////////////////////////////////
            /////////////////////////////////////////GoalKick action/////////////////////////////////////////////////
            if(data->contGoalKick >= 0){
                data->contGoalKick++;
                this->player[data->playTeam][ID_0]->go_to_debug();
                if(data->contGoalKick >= 27){
                    data->contGoalKick = -1;
                    data->goalKick = false;
                }
            }

            //Starting threads of each robot
            this->player[data->playTeam][ID_2]->start(QThread::HighestPriority);
            this->player[data->playTeam][ID_0]->start(QThread::HighestPriority);
            this->player[data->playTeam][ID_1]->start();
        }
        else{
            this->player[this->data->playTeam][ID_0]->stopRobot();
            this->player[this->data->playTeam][ID_1]->stopRobot();
            this->player[this->data->playTeam][ID_2]->stopRobot();
        }
    }
    else{
        this->player[this->data->playTeam][ID_0]->stopRobot();
        this->player[this->data->playTeam][ID_1]->stopRobot();
        this->player[this->data->playTeam][ID_2]->playID_2();
    }
}

void Core::stop(){
    delete visionClient;
    delete actuatorClient;
    delete refereeClient;
    delete replacerClient;
    delete[] player;
    /*delete player[data->playTeam][ID_0];
    delete player[!data->playTeam][ID_0];
    delete player[data->playTeam][ID_1];
    delete player[!data->playTeam][ID_1];
    delete player[data->playTeam][ID_2];
    delete player[!data->playTeam][ID_2];*/
}


void Core::reposition(){
    //REPOSITIONING FREE_BALL
    if(refereeClient->getLastFoul() == VSSRef::Foul::FREE_BALL){
        if(refereeClient->getLastFoulQuadrant() == VSSRef::Quadrant::QUADRANT_1){
            if(this->data->playTeam == TEAM_BLUE){
                player[TEAM_BLUE][ID_2]->setX(150);
                player[TEAM_BLUE][ID_2]->setY(400);
                player[TEAM_BLUE][ID_1]->setX(-100);
                player[TEAM_BLUE][ID_1]->setY(0);
                player[TEAM_BLUE][ID_1]->setAngle(0);
                player[TEAM_BLUE][ID_2]->setAngle(0);
                player[TEAM_BLUE][ID_0]->setX(-700);
                player[TEAM_BLUE][ID_0]->setY(0);
                player[TEAM_BLUE][ID_0]->setAngle(90);
            }
            else{
                player[TEAM_YELLOW][ID_2]->setX(570);
                player[TEAM_YELLOW][ID_2]->setY(400);
                player[TEAM_YELLOW][ID_2]->setAngle(180);
                player[TEAM_YELLOW][ID_1]->setX(555);
                player[TEAM_YELLOW][ID_1]->setY(-25);
                player[TEAM_YELLOW][ID_1]->setAngle(90);
                player[TEAM_YELLOW][ID_0]->setX(700);
                player[TEAM_YELLOW][ID_0]->setY(150);
                player[TEAM_YELLOW][ID_0]->setAngle(90);
            }
        }
        else if(refereeClient->getLastFoulQuadrant() == VSSRef::Quadrant::QUADRANT_2){
            if(this->data->playTeam == TEAM_BLUE){
                player[TEAM_BLUE][ID_2]->setX(-570);
                player[TEAM_BLUE][ID_2]->setY(400);
                player[TEAM_BLUE][ID_2]->setAngle(0);
                player[TEAM_BLUE][ID_1]->setX(-555);
                player[TEAM_BLUE][ID_1]->setY(-25);
                player[TEAM_BLUE][ID_1]->setAngle(90);
                player[TEAM_BLUE][ID_0]->setX(-700);
                player[TEAM_BLUE][ID_0]->setY(150);
                player[TEAM_BLUE][ID_0]->setAngle(90);
            }
            else{
                player[TEAM_YELLOW][ID_2]->setX(-180);
                player[TEAM_YELLOW][ID_2]->setY(400);
                player[TEAM_YELLOW][ID_1]->setX(100);
                player[TEAM_YELLOW][ID_1]->setY(0);
                player[TEAM_YELLOW][ID_1]->setAngle(180);
                player[TEAM_YELLOW][ID_2]->setAngle(180);
                player[TEAM_YELLOW][ID_0]->setX(700);
                player[TEAM_YELLOW][ID_0]->setY(0);
                player[TEAM_YELLOW][ID_0]->setAngle(90);
            }
        }
        else if(refereeClient->getLastFoulQuadrant() == VSSRef::Quadrant::QUADRANT_3){
            if(this->data->playTeam == TEAM_BLUE){
                player[TEAM_BLUE][ID_2]->setX(-570);
                player[TEAM_BLUE][ID_2]->setY(-400);
                player[TEAM_BLUE][ID_2]->setAngle(180);
                player[TEAM_BLUE][ID_1]->setX(-555);
                player[TEAM_BLUE][ID_1]->setY(25);
                player[TEAM_BLUE][ID_1]->setAngle(90);
                player[TEAM_BLUE][ID_0]->setX(-700);
                player[TEAM_BLUE][ID_0]->setY(-150);
                player[TEAM_BLUE][ID_0]->setAngle(-90);
            }
            else{
                player[TEAM_YELLOW][ID_1]->setX(150);
                player[TEAM_YELLOW][ID_1]->setY(-100);
                player[TEAM_YELLOW][ID_2]->setX(-180);
                player[TEAM_YELLOW][ID_2]->setY(-400);
                player[TEAM_YELLOW][ID_2]->setAngle(180);
                player[TEAM_YELLOW][ID_1]->setAngle(180);
                player[TEAM_YELLOW][ID_0]->setX(700);
                player[TEAM_YELLOW][ID_0]->setY(0);
                player[TEAM_YELLOW][ID_0]->setAngle(90);
            }
        }
        else{
            if(this->data->playTeam == TEAM_BLUE){
                player[TEAM_BLUE][ID_1]->setX(-150);
                player[TEAM_BLUE][ID_1]->setY(-100);
                player[TEAM_BLUE][ID_2]->setX(180);
                player[TEAM_BLUE][ID_2]->setY(-400);
                player[TEAM_BLUE][ID_2]->setAngle(0);
                player[TEAM_BLUE][ID_1]->setAngle(0);
                player[TEAM_BLUE][ID_0]->setX(-700);
                player[TEAM_BLUE][ID_0]->setY(0);
                player[TEAM_BLUE][ID_0]->setAngle(90);
            }
            else{
                player[TEAM_YELLOW][ID_2]->setX(570);
                player[TEAM_YELLOW][ID_2]->setY(-400);
                player[TEAM_YELLOW][ID_2]->setAngle(0);
                player[TEAM_YELLOW][ID_1]->setX(555);
                player[TEAM_YELLOW][ID_1]->setY(25);
                player[TEAM_YELLOW][ID_1]->setAngle(90);
                player[TEAM_YELLOW][ID_0]->setX(700);
                player[TEAM_YELLOW][ID_0]->setY(-150);
                player[TEAM_YELLOW][ID_0]->setAngle(-90);
            }
        }
        this->sendRef();
    }

    //REPOSITIONING GOAL_KICK
    else if(refereeClient->getLastFoul() == VSSRef::Foul::GOAL_KICK){
        if(refereeClient->getLastFoulColor() == VSSRef::Color::BLUE){
            if(data->playTeam == TEAM_BLUE){
                player[TEAM_BLUE][ID_2]->setX(-680);
                player[TEAM_BLUE][ID_2]->setY(550);
                player[TEAM_BLUE][ID_2]->setAngle(0);
                player[TEAM_BLUE][ID_1]->setX(-555);
                player[TEAM_BLUE][ID_1]->setY(-70);
                player[TEAM_BLUE][ID_1]->setAngle(90);
                player[TEAM_BLUE][ID_0]->setX(-658);
                player[TEAM_BLUE][ID_0]->setY(278);
                player[TEAM_BLUE][ID_0]->setAngle(45);
                data->goalKick = true;
                data->contGoalKick = 0;
            }
            else{
                player[TEAM_YELLOW][ID_1]->setX(-175);
                player[TEAM_YELLOW][ID_1]->setY(-275);
                player[TEAM_YELLOW][ID_1]->setAngle(0);
                player[TEAM_YELLOW][ID_2]->setX(-175);
                player[TEAM_YELLOW][ID_2]->setY(275);
                player[TEAM_YELLOW][ID_2]->setAngle(0);
                player[TEAM_YELLOW][ID_0]->setX(700);
                player[TEAM_YELLOW][ID_0]->setY(0);
                player[TEAM_YELLOW][ID_0]->setAngle(90);
            }
            this->sendRef();
        }
        else if(refereeClient->getLastFoulColor() == VSSRef::Color::YELLOW){
            if(data->playTeam == TEAM_YELLOW){
                player[TEAM_YELLOW][ID_2]->setX(680);
                player[TEAM_YELLOW][ID_2]->setY(550);
                player[TEAM_YELLOW][ID_2]->setAngle(180);
                player[TEAM_YELLOW][ID_1]->setX(555);
                player[TEAM_YELLOW][ID_1]->setY(-70);
                player[TEAM_YELLOW][ID_1]->setAngle(90);
                player[TEAM_YELLOW][ID_0]->setX(658);
                player[TEAM_YELLOW][ID_0]->setY(278);
                player[TEAM_YELLOW][ID_0]->setAngle(135);
                data->goalKick = true;
                data->contGoalKick = 0;
            }
            else{
                player[TEAM_BLUE][ID_2]->setX(175);
                player[TEAM_BLUE][ID_2]->setY(275);
                player[TEAM_BLUE][ID_2]->setAngle(0);
                player[TEAM_BLUE][ID_1]->setX(175);
                player[TEAM_BLUE][ID_1]->setY(-275);
                player[TEAM_BLUE][ID_1]->setAngle(0);
                player[TEAM_BLUE][ID_0]->setX(-700);
                player[TEAM_BLUE][ID_0]->setY(0);
                player[TEAM_BLUE][ID_0]->setAngle(90);
            }
            this->sendRef();
        }
    }

    else if(refereeClient->getLastFoul() == VSSRef::Foul::PENALTY_KICK){
        if(refereeClient->getLastGameHalf() == VSSRef::Half::PENALTY_SHOOTOUTS){
            //cout << "PENALTY_SHOOTOUTS" << endl;
            if(refereeClient->getLastFoulColor() == VSSRef::Color::BLUE){
                if(data->playTeam == TEAM_BLUE){
                    data->penalty = true;
                    data->robotPenalty[ID_2+2] = 0;
                    data->penaltyAction = 2;
                    player[TEAM_BLUE][ID_2]->setX(300);
                    player[TEAM_BLUE][ID_2]->setY(-200);
                    player[TEAM_BLUE][ID_2]->setAngle(25);
                }
                else{
                    player[TEAM_YELLOW][ID_0]->setX(720);
                    player[TEAM_YELLOW][ID_0]->setY(0);
                    player[TEAM_YELLOW][ID_0]->setAngle(90);
                    data->defendPenalty = true;
                    data->contDefendPenalty = 0;
                }
            }
            else{
                if(data->playTeam == TEAM_YELLOW){
                    data->penalty = true;
                    data->robotPenalty[ID_2+2] = 0;
                    data->penaltyAction = 2;
                    player[TEAM_YELLOW][ID_2]->setX(-300);
                    player[TEAM_YELLOW][ID_2]->setY(-20);
                    player[TEAM_YELLOW][ID_2]->setAngle(155);
                }
                else{
                    player[TEAM_BLUE][ID_0]->setX(-720);
                    player[TEAM_BLUE][ID_0]->setY(0);
                    player[TEAM_BLUE][ID_0]->setAngle(90);
                    data->defendPenalty = true;
                    data->contDefendPenalty = 0;
                }
            }
        }
        else{
            if(refereeClient->getLastFoulColor() == VSSRef::Color::BLUE){
                if(this->data->playTeam == TEAM_BLUE){
                    this->data->penalty = true;
                    data->penaltyAction = rand() % 3;
                    data->robotPenalty[ID_2] = 0;
                    //data->penaltyAction = 2;
                    //cout << data->penaltyAction << endl;
                    if(data->penaltyAction == 0){
                        player[TEAM_BLUE][ID_2]->setX(200);
                        player[TEAM_BLUE][ID_2]->setY(-20);
                        player[TEAM_BLUE][ID_2]->setAngle(8);
                        player[TEAM_BLUE][ID_1]->setX(-200);
                        player[TEAM_BLUE][ID_1]->setY(550);
                        player[TEAM_BLUE][ID_1]->setAngle(-20);
                        player[TEAM_BLUE][ID_0]->setX(-700);
                        player[TEAM_BLUE][ID_0]->setY(0);
                        player[TEAM_BLUE][ID_0]->setAngle(90);
                    }
                    else if(data->penaltyAction == 1){
                        player[TEAM_BLUE][ID_2]->setX(290);
                        player[TEAM_BLUE][ID_2]->setY(-130);
                        player[TEAM_BLUE][ID_2]->setAngle(-120);
                        player[TEAM_BLUE][ID_1]->setX(-50);
                        player[TEAM_BLUE][ID_1]->setY(600);
                        player[TEAM_BLUE][ID_1]->setAngle(-20);
                        player[TEAM_BLUE][ID_0]->setX(-700);
                        player[TEAM_BLUE][ID_0]->setY(0);
                        player[TEAM_BLUE][ID_0]->setAngle(90);
                    }
                    else{
                        player[TEAM_BLUE][ID_2]->setX(375);
                        player[TEAM_BLUE][ID_2]->setY(-85);
                        player[TEAM_BLUE][ID_2]->setAngle(45);
                        player[TEAM_BLUE][ID_1]->setX(-50);
                        player[TEAM_BLUE][ID_1]->setY(600);
                        player[TEAM_BLUE][ID_1]->setAngle(-20);
                        player[TEAM_BLUE][ID_0]->setX(-700);
                        player[TEAM_BLUE][ID_0]->setY(0);
                        player[TEAM_BLUE][ID_0]->setAngle(90);
                    }
                }
                else{
                    player[TEAM_YELLOW][ID_2]->setX(-70);
                    player[TEAM_YELLOW][ID_2]->setY(130);
                    player[TEAM_YELLOW][ID_2]->setAngle(180);
                    player[TEAM_YELLOW][ID_1]->setX(-100);
                    player[TEAM_YELLOW][ID_1]->setY(-300);
                    player[TEAM_YELLOW][ID_1]->setAngle(90);
                    player[TEAM_YELLOW][ID_0]->setX(720);
                    player[TEAM_YELLOW][ID_0]->setY(0);
                    player[TEAM_YELLOW][ID_0]->setAngle(180);
                    data->defendPenalty = true;
                    data->contDefendPenalty = 0;
                }
            }
            else{
                if(this->data->playTeam == TEAM_YELLOW){
                    this->data->penalty = true;
                    data->penaltyAction = rand() % 3;
                    //data->penaltyAction = 2;
                    data->robotPenalty[ID_2] = 0;
                    //cout << data->penaltyAction << endl;
                    if(data->penaltyAction == 0){                        
                        player[TEAM_YELLOW][ID_2]->setX(-200);
                        player[TEAM_YELLOW][ID_2]->setY(-20);
                        player[TEAM_YELLOW][ID_2]->setAngle(-8);
                        player[TEAM_YELLOW][ID_1]->setX(200);
                        player[TEAM_YELLOW][ID_1]->setY(550);
                        player[TEAM_YELLOW][ID_1]->setAngle(20);
                        player[TEAM_YELLOW][ID_0]->setX(700);
                        player[TEAM_YELLOW][ID_0]->setY(0);
                        player[TEAM_YELLOW][ID_0]->setAngle(90);
                    }
                    else if(data->penaltyAction == 1){
                        player[TEAM_YELLOW][ID_2]->setX(-290);
                        player[TEAM_YELLOW][ID_2]->setY(-130);
                        player[TEAM_YELLOW][ID_2]->setAngle(120);
                        player[TEAM_YELLOW][ID_1]->setX(50);
                        player[TEAM_YELLOW][ID_1]->setY(600);
                        player[TEAM_YELLOW][ID_1]->setAngle(20);
                        player[TEAM_YELLOW][ID_0]->setX(700);
                        player[TEAM_YELLOW][ID_0]->setY(0);
                        player[TEAM_YELLOW][ID_0]->setAngle(90);
                    }
                    else{
                        player[TEAM_YELLOW][ID_2]->setX(-375);
                        player[TEAM_YELLOW][ID_2]->setY(-85);
                        player[TEAM_YELLOW][ID_2]->setAngle(135);
                        player[TEAM_YELLOW][ID_1]->setX(50);
                        player[TEAM_YELLOW][ID_1]->setY(600);
                        player[TEAM_YELLOW][ID_1]->setAngle(20);
                        player[TEAM_YELLOW][ID_0]->setX(700);
                        player[TEAM_YELLOW][ID_0]->setY(0);
                        player[TEAM_YELLOW][ID_0]->setAngle(90);
                    }
                }
                else{
                    player[TEAM_BLUE][ID_2]->setX(70);
                    player[TEAM_BLUE][ID_2]->setY(130);
                    player[TEAM_BLUE][ID_2]->setAngle(0);
                    player[TEAM_BLUE][ID_1]->setX(100);
                    player[TEAM_BLUE][ID_1]->setY(-300);
                    player[TEAM_BLUE][ID_1]->setAngle(90);
                    player[TEAM_BLUE][ID_0]->setX(-720);
                    player[TEAM_BLUE][ID_0]->setY(0);
                    player[TEAM_BLUE][ID_0]->setAngle(180);
                    data->defendPenalty = true;
                    data->contDefendPenalty = 0;
                }
            }
        }
        this->sendRef();
    }
    //REPOSICIONAMENTO KICKOFF
    else if(refereeClient->getLastFoul() == VSSRef::Foul::KICKOFF){
        if(this->data->playTeam == TEAM_BLUE){
            if(this->refereeClient->getLastFoulColor() == VSSRef::Color::BLUE){
                player[TEAM_BLUE][ID_2]->setX(-70);
                player[TEAM_BLUE][ID_2]->setY(-30);
                player[TEAM_BLUE][ID_2]->setAngle(27.5);
                player[TEAM_BLUE][ID_1]->setX(-350);
                player[TEAM_BLUE][ID_1]->setY(0);
                player[TEAM_BLUE][ID_1]->setAngle(0);
                player[TEAM_BLUE][ID_0]->setX(-700);
                player[TEAM_BLUE][ID_0]->setY(0);
                player[TEAM_BLUE][ID_0]->setAngle(90);
            }
            else{
                player[TEAM_BLUE][ID_2]->setX(-230);
                player[TEAM_BLUE][ID_2]->setY(-70);
                player[TEAM_BLUE][ID_2]->setAngle(20);
                player[TEAM_BLUE][ID_1]->setX(-500);
                player[TEAM_BLUE][ID_1]->setY(0);
                player[TEAM_BLUE][ID_1]->setAngle(0);
                player[TEAM_BLUE][ID_0]->setX(-700);
                player[TEAM_BLUE][ID_0]->setY(0);
                player[TEAM_BLUE][ID_0]->setAngle(90);
            }
        }
        else {
            if(this->refereeClient->getLastFoulColor() == VSSRef::Color::YELLOW){
                player[TEAM_YELLOW][ID_2]->setX(100);
                player[TEAM_YELLOW][ID_2]->setY(-15);
                player[TEAM_YELLOW][ID_1]->setX(350);
                player[TEAM_YELLOW][ID_1]->setY(0);
                player[TEAM_YELLOW][ID_1]->setAngle(180);
                player[TEAM_YELLOW][ID_2]->setAngle(165);
                player[TEAM_YELLOW][ID_0]->setX(700);
                player[TEAM_YELLOW][ID_0]->setY(0);
                player[TEAM_YELLOW][ID_0]->setAngle(90);
            }
            else{
                player[TEAM_YELLOW][ID_2]->setX(230);
                player[TEAM_YELLOW][ID_2]->setY(70);
                player[TEAM_YELLOW][ID_2]->setAngle(207.5);
                player[TEAM_YELLOW][ID_1]->setX(500);
                player[TEAM_YELLOW][ID_1]->setY(0);
                player[TEAM_YELLOW][ID_1]->setAngle(0);
                player[TEAM_YELLOW][ID_0]->setX(700);
                player[TEAM_YELLOW][ID_0]->setY(0);
                player[TEAM_YELLOW][ID_0]->setAngle(90);
            }
        }
        this->sendRef();
    }
}

void Core::sendRef(){
    this->replacerClient->placeRobot(ID_0,this->player[this->data->playTeam][ID_0]->getX() / 1000.0,
                                    this->player[this->data->playTeam][ID_0]->getY() / 1000.0,
                                    this->player[this->data->playTeam][ID_0]->getAngle());
    this->replacerClient->placeRobot(ID_1,this->player[this->data->playTeam][ID_1]->getX() / 1000.0,
                                    this->player[this->data->playTeam][ID_1]->getY() / 1000.0,
                                    this->player[this->data->playTeam][ID_1]->getAngle());
    this->replacerClient->placeRobot(ID_2,this->player[this->data->playTeam][ID_2]->getX() / 1000.0,
                                    this->player[this->data->playTeam][ID_2]->getY() / 1000.0,
                                    this->player[this->data->playTeam][ID_2]->getAngle());
    this->replacerClient->sendFrame();
}
