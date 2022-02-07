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

    //QObject::connect(this->player[data->playTeam][goal_id], SIGNAL(started()), this->player[data->playTeam][goal_id], SLOT(printStartGOAL()));
    //QObject::connect(this->player[data->playTeam][def_id], SIGNAL(started()), this->player[data->playTeam][def_id], SLOT(printStartDEF()));
    //QObject::connect(this->player[data->playTeam][att_id], SIGNAL(started()), this->player[data->playTeam][att_id], SLOT(printStartATT()));
    //QObject::connect(this->player[data->playTeam][goal_id], SIGNAL(finished()), this->player[data->playTeam][goal_id], SLOT(printFinishGOAL()));
    //QObject::connect(this->player[data->playTeam][def_id], SIGNAL(finished()), this->player[data->playTeam][def_id], SLOT(printFinishDEF()));
    //QObject::connect(this->player[data->playTeam][att_id], SIGNAL(finished()), this->player[data->playTeam][att_id], SLOT(printFinishATT()));
    //QObject::connect(this->player[data->playTeam][goal_id], SIGNAL(endPlay()), this->player[data->playTeam][goal_id], SLOT(quit()));
    //QObject::connect(this->player[data->playTeam][def_id], SIGNAL(endPlay()), this->player[data->playTeam][def_id], SLOT(quit()));
    //QObject::connect(this->player[data->playTeam][att_id], SIGNAL(endPlay()), this->player[data->playTeam][att_id], SLOT(quit()));
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
    data->ballPos = QPoint(750,650);

    // PID and Speed goalkeeper (robot 0)
    double fast_speed = 105;  //200
    double spin_speed = fast_speed*1.5;
    double kp_base = -1.3;     //-2.65
    double ki_base = -0.0;
    double kd_base = -26.7;     //-50.5

    // PID and Speed defender (robot 1)
    double fast_speed2 = 200;   //200   //300
    double spin_speed2 = fast_speed2*1.5;
    double kp_base2 = -3.0;    //-2.65 //-4.65
    double ki_base2 = -0.0;
    double kd_base2 = -38.5;    //-50.5  //-95.5

    //PID and Speed stricker (robot 2)
    double fast_speed3_r = 200; //210   //400
    double spin_speed3_r = fast_speed3_r*1.5;
    double kp_base3_r = -3.0;  //-2.65 //-5.65
    double ki_base3_r = -0.0;
    double kd_base3_r = -38.5; //-50.5 //-100.5

    // Configuring robots
    //int _team_id, int _id, double _kp, double _ki, double _kd, double _base_speed, double_spin_speed
    //this->player[this->data->playTeam][goal_id]->setRobot(this->data->playTeam, goal_id, kp_base, ki_base, kd_base, fast_speed, spin_speed);
    this->player[this->data->playTeam][goal_id] = new Robot(this->data->playTeam, goal_id, kp_base, ki_base, kd_base, fast_speed, spin_speed);
    this->player[!this->data->playTeam][goal_id] = new Robot(this->data->playTeam, goal_id, kp_base, ki_base, kd_base, fast_speed, spin_speed);
    this->player[this->data->playTeam][goal_id]->setActuator(actuatorClient);

    //this->player[this->data->playTeam][def_id]->setRobot(this->data->playTeam, def_id, kp_base2, ki_base2, kd_base2, fast_speed2, spin_speed2);
    this->player[this->data->playTeam][def_id] = new Robot(this->data->playTeam, def_id, kp_base2, ki_base2, kd_base2, fast_speed2, spin_speed2);
    this->player[!this->data->playTeam][def_id] = new Robot(this->data->playTeam, def_id, kp_base2, ki_base2, kd_base2, fast_speed2, spin_speed2);
    this->player[this->data->playTeam][def_id]->setActuator(actuatorClient);

    //this->player[this->data->playTeam][att_id]->setRobot(this->data->playTeam, att_id, kp_base3_r, ki_base3_r, kd_base3_r, fast_speed3_r, spin_speed3_r);
    this->player[this->data->playTeam][att_id] = new Robot(this->data->playTeam, att_id, kp_base3_r, ki_base3_r, kd_base3_r, fast_speed3_r, spin_speed3_r);
    this->player[!this->data->playTeam][att_id] = new Robot(this->data->playTeam, att_id, kp_base3_r, ki_base3_r, kd_base3_r, fast_speed3_r, spin_speed3_r);
    this->player[this->data->playTeam][att_id]->setActuator(actuatorClient);
}

void Core::update(){
    //Updating the last environment
    fira_message::sim_to_ref::Environment lastEnv = this->visionClient->getLastEnvironment();

    // Updating information of each robot
    if(lastEnv.has_frame()){
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
                    this->player[TEAM_YELLOW][j]->setY(lastFrame.robots_yellow(j).y()* 1000 + 750);    // Adjust to work only in the first quadrant
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

        // Predicting future ball position
        /*double v_x = 30.0*(data->ballPos.x() - this->data->lastBallPos.x());
        double v_y = 30.0*(data->ballPos.y() - this->data->lastBallPos.y());;
        double k = 0.05;
        double f_vx = k * v_x + (1.0 - k) * data->ballVel.x();
        double f_vy = k * v_y + (1.0 - k) * data->ballVel.y();
        data->ballVel = QPointF(f_vx,f_vy);*/
        //data->futureBallPos = QPointF(data->ballPos.x() + data->ballVel.x()*predict_fact, data->ballPos.y() + data->ballVel.y()*predict_fact);

        //Determing future ball position
        data->futureBallPos.setX(data->ballPos.x() + data->ballVel.x() * 5 * TIME_TO_RECEIVE_DATA + (0.5 * (data->ballVel.x() - data->lastBallVel.x()) * 25 * TIME_TO_RECEIVE_DATA));
        data->futureBallPos.setY(data->ballPos.y() + data->ballVel.y() * 5 * TIME_TO_RECEIVE_DATA + (0.5 * (data->ballVel.y() - data->lastBallVel.y()) * 25 * TIME_TO_RECEIVE_DATA));

        //Showing information about ball
        /*cout << "X: " << this->data->ballPos.x() << endl;
        cout << "Y: " << this->data->ballPos.y() << endl;
        cout << "X FUT: " << this->data->futureBallPos.x() << endl;
        cout << "Y FUT: " << this->data->futureBallPos.y() << endl;*/
        /*cout << "Vx: " << this->data->ballVel.x() << endl;
        cout << "Vy: " << this->data->ballVel.y() << endl;
        */
        emit readyToProcess();
    }
}

void Core::process(){
    //Defining the play
    data->mode = bola_ataque;
    double x_attack =  160;
    if(this->data->playSide == LEFT_SIDE){
        if(this->data->ballPos.x() >= this->data->middle_field.x() - x_attack){
            data->mode = bola_ataque;
        }
        else{
            if(is_inside(this->data->ballPos,this->data->area[LEFT_SIDE]))
                data->mode = bola_area;
            else
                data->mode = bola_defesa;
        }
    }
    else{
        if(this->data->ballPos.x() <= this->data->middle_field.x() + x_attack){
            data->mode = bola_ataque;
        }
        else{
            if(is_inside(this->data->ballPos,this->data->area[RIGHT_SIDE]))
                data->mode = bola_area;
            else
                data->mode = bola_defesa;
        }
    }

    if(!this->getTestCondition()){
        if(this->data->playStatus == ON){
            /////////////////////////////////////////Penalty action/////////////////////////////////////////////////
            if(data->robotPenalty[att_id] >= 0 || data->robotPenalty[att_id +1] >= 0 || data->robotPenalty[att_id+2] >=0){
                /*cout << "PENALTY: " << data->penaltyAction << endl;*/
                if(data->penaltyAction == 0){
                    player[data->playTeam][att_id]->kick();
                    data->robotPenalty[att_id]++;
                }
                else if(data->penaltyAction == 1){
                    player[data->playTeam][att_id]->intercept();
                    data->robotPenalty[att_id+1]++;
                }
                else{
                    player[data->playTeam][att_id]->go_to_penalty();
                    data->robotPenalty[att_id+2]++;
                }
                if(data->robotPenalty[att_id] >= max_cont_penalty || data->robotPenalty[att_id+1] >= max_cont2_penalty || data->robotPenalty[att_id+2] >= max_cont2_penalty){
                    data->robotPenalty[att_id] = -1;
                    data->robotPenalty[att_id+1] = -1;
                    data->robotPenalty[att_id+2] = -1;
                    data->penalty = false;
                    data->speed_spin_increment = 3;
                    this->player[data->playTeam][att_id]->position(data->middle_field,180,1,2);
                }
            }

            //Starting threads of each robot
            this->player[data->playTeam][att_id]->start(QThread::HighestPriority);
            this->player[data->playTeam][goal_id]->start(QThread::HighestPriority);
            this->player[data->playTeam][def_id]->start();

            ///////////////////////////////////////////////////////////////////////////////////////////////////////
/*
            if(this->data->formation1){ // 1-0-2
                //STRICKER
                switch(data->mode){
                case bola_ataque:
                    if(!data->penalty)
                        this->player[data->playTeam][att_id]->attack();
                    break;
                case bola_defesa:
                    this->player[data->playTeam][att_id]->intercept();
                    break;
                case bola_area:
                    this->player[data->playTeam][att_id]->intercept();
                    break;
                }
                //DEFENDER
                switch(data->mode){
                case bola_ataque:
                    this->player[this->data->playTeam][def_id]->attack();
                    break;
                case bola_defesa:
                    this->player[this->data->playTeam][def_id]->defend_middle_attack();
                    break;
                case bola_area:
                    this->player[this->data->playTeam][def_id]->defend_middle_attack();
                    break;
                }
                //GOALKEEPER
                switch(data->mode){
                case bola_ataque:
                    this->player[this->data->playTeam][goal_id]->defend_goal_LARC();
                    break;
                case bola_defesa:
                    this->player[this->data->playTeam][goal_id]->defend_goal_LARC();
                    break;
                case bola_area:
                    this->player[this->data->playTeam][goal_id]->defend_goal_LARC();
                    break;
                }
            }

            else if (this->data->formation2) { //Safe 1-1-1
                //ATACANTE
                switch(data->mode){
                case bola_ataque:
                    this->player[data->playTeam][att_id]->attack();
                    break;
                case bola_defesa:
                    this->player[data->playTeam][att_id]->defend_middle_attack();
                    break;
                case bola_area:
                    // 1 segura o que esta perto da bola
                    // 2 segura o que esta perto do gol
                    // 3 segura o que esta mais longe
                    this->player[data->playTeam][att_id]->defend_middle_attack();
                    break;
                }
                //DEFENSOR
                switch(data->mode){
                case bola_ataque:
                    this->player[data->playTeam][def_id]->defend_middle();
                    break;
                case bola_defesa:
                    this->player[data->playTeam][def_id]->intercept();
                    break;
                case bola_area:
                    // 1 segura o que esta perto da bola
                    // 2 segura o que esta perto do gol
                    // 3 segura o que esta mais longe
                    this->player[data->playTeam][def_id]->defend_middle();
                    break;
                }
                //GOLEIRO
                switch(data->mode){
                case bola_ataque:
                    this->player[data->playTeam][goal_id]->attack();
                    break;
                case bola_defesa:
                    this->player[data->playTeam][goal_id]->attack();
                    break;
                case bola_area:
                    this->player[data->playTeam][goal_id]->defend_goal_new();
                    break;
                }
            }

            else if (this->data->formation3) {
                //ATACANTE
                switch(data->mode){
                case bola_ataque:
                    this->player[data->playTeam][att_id]->attack();
                    break;
                case bola_defesa:
                    this->player[data->playTeam][att_id]->defend_middle_attack();
                    break;
                case bola_area:
                    // 1 segura o que esta perto da bola
                    // 2 segura o que esta perto do gol
                    // 3 segura o que esta mais longe
                    this->player[data->playTeam][att_id]->defend_block(1);
                    break;
                }
                //DEFENSOR
                switch(data->mode){
                case bola_ataque:
                    this->player[data->playTeam][def_id]->defend_middle();
                    break;
                case bola_defesa:
                    this->player[data->playTeam][def_id]->intercept();
                    break;
                case bola_area:
                    // 1 segura o que esta perto da bola
                    // 2 segura o que esta perto do gol
                    // 3 segura o que esta mais longe
                    this->player[data->playTeam][def_id]->defend_block(2);
                    break;
                }
                //GOLEIRO
                switch(data->mode){
                case bola_ataque:
                    this->player[data->playTeam][goal_id]->attack();
                    break;
                case bola_defesa:
                    this->player[data->playTeam][goal_id]->defend_goal_new();
                    break;
                case bola_area:
                    this->player[data->playTeam][goal_id]->defend_goal_new();
                    break;
                }
            }
            else if(this->data->formation4){
                //ATACANTE
                switch(data->mode){
                case bola_ataque:
                    this->player[data->playTeam][att_id]->attack();
                    break;
                case bola_defesa:
                    this->player[data->playTeam][att_id]->intercept();
                    break;
                case bola_area:
                    // 1 segura o que esta perto da bola
                    // 2 segura o que esta perto do gol
                    // 3 segura o que esta mais longe
                    this->player[data->playTeam][att_id]->defend_middle();
                    break;
                }
                //DEFENSOR
                switch(data->mode){
                case bola_ataque:
                    this->player[data->playTeam][def_id]->go_to_debug();
                    break;
                case bola_defesa:
                    this->player[data->playTeam][def_id]->go_to_debug(); //intercept_antigo()
                    break;
                case bola_area:
                    // 1 segura o que esta perto da bola
                    // 2 segura o que esta perto do gol
                    // 3 segura o que esta mais longe
                    this->player[data->playTeam][def_id]->defend_middle();
                    break;
                }
                //GOLEIRO
                switch(data->mode){
                case bola_ataque:
                    this->player[data->playTeam][goal_id]->defend_goal();
                    break;
                case bola_defesa:
                    this->player[data->playTeam][goal_id]->defend_goal();
                    break;
                case bola_area:
                    this->player[data->playTeam][goal_id]->defend_goal();
                    break;
                }
            }*/
        }
        else{
            this->player[this->data->playTeam][goal_id]->stopRobot();
            this->player[this->data->playTeam][def_id]->stopRobot();
            this->player[this->data->playTeam][att_id]->stopRobot();
        }
    }
    else{
        this->player[this->data->playTeam][goal_id]->stopRobot();
        this->player[this->data->playTeam][def_id]->stopRobot();
        if(this->data->formation1){ // 1-0-2
            //STRICKER
            switch(data->mode){
            case bola_ataque:
                if(!data->penalty)
                    this->player[this->data->playTeam][att_id]->attack();
                break;
            case bola_defesa:
                this->player[this->data->playTeam][att_id]->intercept_antigo();
                break;
            case bola_area:
                this->player[this->data->playTeam][att_id]->intercept_antigo();
                break;
            }
        }
    }
}

void Core::reposition(){
    //REPOSITIONING FREE_BALL
    if(refereeClient->getLastFoul() == VSSRef::Foul::FREE_BALL){
        if(refereeClient->getLastFoulQuadrant() == VSSRef::Quadrant::QUADRANT_1){
            if(this->data->playTeam == TEAM_BLUE){
                player[TEAM_BLUE][att_id]->setX(150);
                player[TEAM_BLUE][att_id]->setY(400);
                player[TEAM_BLUE][def_id]->setX(-100);
                player[TEAM_BLUE][def_id]->setY(0);
                player[TEAM_BLUE][def_id]->setAngle(0);
                player[TEAM_BLUE][att_id]->setAngle(0);
                player[TEAM_BLUE][goal_id]->setX(-595);
                player[TEAM_BLUE][goal_id]->setY(0);
                player[TEAM_BLUE][goal_id]->setAngle(90);
            }
            else{
                player[TEAM_YELLOW][att_id]->setX(570);
                player[TEAM_YELLOW][att_id]->setY(400);
                player[TEAM_YELLOW][att_id]->setAngle(180);
                player[TEAM_YELLOW][def_id]->setX(450);
                player[TEAM_YELLOW][def_id]->setY(-50);
                player[TEAM_YELLOW][def_id]->setAngle(175);
                player[TEAM_YELLOW][goal_id]->setX(610);
                player[TEAM_YELLOW][goal_id]->setY(150);
                player[TEAM_YELLOW][goal_id]->setAngle(135);
            }
        }
        else if(refereeClient->getLastFoulQuadrant() == VSSRef::Quadrant::QUADRANT_2){
            if(this->data->playTeam == TEAM_BLUE){
                player[TEAM_BLUE][att_id]->setX(-570);
                player[TEAM_BLUE][att_id]->setY(400);
                player[TEAM_BLUE][att_id]->setAngle(0);
                player[TEAM_BLUE][def_id]->setX(-450);
                player[TEAM_BLUE][def_id]->setY(-50);
                player[TEAM_BLUE][def_id]->setAngle(180);
                player[TEAM_BLUE][goal_id]->setX(-610);
                player[TEAM_BLUE][goal_id]->setY(150);
                player[TEAM_BLUE][goal_id]->setAngle(45);
            }
            else{
                player[TEAM_YELLOW][att_id]->setX(-180);
                player[TEAM_YELLOW][att_id]->setY(400);
                player[TEAM_YELLOW][def_id]->setX(100);
                player[TEAM_YELLOW][def_id]->setY(0);
                player[TEAM_YELLOW][def_id]->setAngle(180);
                player[TEAM_YELLOW][att_id]->setAngle(180);
                player[TEAM_YELLOW][goal_id]->setX(595);
                player[TEAM_YELLOW][goal_id]->setY(0);
                player[TEAM_YELLOW][goal_id]->setAngle(90);
            }
        }
        else if(refereeClient->getLastFoulQuadrant() == VSSRef::Quadrant::QUADRANT_3){
            if(this->data->playTeam == TEAM_BLUE){
                player[TEAM_BLUE][att_id]->setX(-570);
                player[TEAM_BLUE][att_id]->setY(-400);
                player[TEAM_BLUE][att_id]->setAngle(0);
                player[TEAM_BLUE][def_id]->setX(-300);
                player[TEAM_BLUE][def_id]->setY(100);
                player[TEAM_BLUE][def_id]->setAngle(0);
                player[TEAM_BLUE][goal_id]->setX(-610);
                player[TEAM_BLUE][goal_id]->setY(-150);
                player[TEAM_BLUE][goal_id]->setAngle(-45);
            }
            else{
                player[TEAM_YELLOW][def_id]->setX(150);
                player[TEAM_YELLOW][def_id]->setY(-100);
                player[TEAM_YELLOW][att_id]->setX(-180);
                player[TEAM_YELLOW][att_id]->setY(-400);
                player[TEAM_YELLOW][att_id]->setAngle(180);
                player[TEAM_YELLOW][def_id]->setAngle(180);
                player[TEAM_YELLOW][goal_id]->setX(595);
                player[TEAM_YELLOW][goal_id]->setY(0);
                player[TEAM_YELLOW][goal_id]->setAngle(90);
            }
        }
        else{
            if(this->data->playTeam == TEAM_BLUE){
                player[TEAM_BLUE][def_id]->setX(-150);
                player[TEAM_BLUE][def_id]->setY(-100);
                player[TEAM_BLUE][att_id]->setX(180);
                player[TEAM_BLUE][att_id]->setY(-400);
                player[TEAM_BLUE][att_id]->setAngle(0);
                player[TEAM_BLUE][def_id]->setAngle(0);
                player[TEAM_BLUE][goal_id]->setX(-595);
                player[TEAM_BLUE][goal_id]->setY(0);
                player[TEAM_BLUE][goal_id]->setAngle(90);
            }
            else{
                player[TEAM_YELLOW][att_id]->setX(570);
                player[TEAM_YELLOW][att_id]->setY(-400);
                player[TEAM_YELLOW][att_id]->setAngle(180);
                player[TEAM_YELLOW][def_id]->setX(550);
                player[TEAM_YELLOW][def_id]->setY(50);
                player[TEAM_YELLOW][def_id]->setAngle(180);
                player[TEAM_YELLOW][goal_id]->setX(610);
                player[TEAM_YELLOW][goal_id]->setY(-150);
                player[TEAM_YELLOW][goal_id]->setAngle(45);
            }
        }
        this->sendRef();
    }

    //ADICIONAR ORIENTATION
    //REPOSICIONAMENTO GOAL_KICK
    else if(refereeClient->getLastFoul() == VSSRef::Foul::GOAL_KICK){
        if(refereeClient->getLastFoulColor() == VSSRef::Color::BLUE && data->playTeam == TEAM_BLUE){
            player[TEAM_BLUE][def_id]->setX(-300);
            player[TEAM_BLUE][def_id]->setY(-400);
            player[TEAM_BLUE][def_id]->setAngle(0);
            player[TEAM_BLUE][att_id]->setX(-676);
            player[TEAM_BLUE][att_id]->setY(500);
            player[TEAM_BLUE][att_id]->setAngle(0);
            player[TEAM_BLUE][goal_id]->setX(-660);
            player[TEAM_BLUE][goal_id]->setY(283);
            player[TEAM_BLUE][goal_id]->setAngle(45);
            this->sendRef();
        }
        else if(refereeClient->getLastFoulColor() == VSSRef::Color::YELLOW && data->playTeam == TEAM_YELLOW){
            player[TEAM_YELLOW][att_id]->setX(676);
            player[TEAM_YELLOW][att_id]->setY(500);
            player[TEAM_YELLOW][att_id]->setAngle(180);
            player[TEAM_YELLOW][def_id]->setX(300);
            player[TEAM_YELLOW][def_id]->setY(-400);
            player[TEAM_YELLOW][def_id]->setAngle(180);
            player[TEAM_YELLOW][goal_id]->setX(660);
            player[TEAM_YELLOW][goal_id]->setY(283);
            player[TEAM_YELLOW][goal_id]->setAngle(135);
            this->sendRef();
        }
    }

    else if(refereeClient->getLastFoul() == VSSRef::Foul::PENALTY_KICK){
        if(refereeClient->getLastGameHalf() == VSSRef::Half::PENALTY_SHOOTOUTS){
            //cout << "PENALTY_SHOOTOUTS" << endl;
            if(refereeClient->getLastFoulColor() == VSSRef::Color::BLUE){
                if(data->playTeam == TEAM_BLUE){
                    data->penalty = true;
                    data->robotPenalty[att_id+2] = 0;
                    data->penaltyAction = 2;
                    player[TEAM_BLUE][att_id]->setX(300);
                    player[TEAM_BLUE][att_id]->setY(-200);
                    player[TEAM_BLUE][att_id]->setAngle(25);
                }
                else{
                    player[TEAM_YELLOW][goal_id]->setX(725);
                    player[TEAM_YELLOW][goal_id]->setY(0);
                    player[TEAM_YELLOW][goal_id]->setAngle(180);
                    defende_penalty = true;
                }
            }
            else{
                if(data->playTeam == TEAM_YELLOW){
                    data->penalty = true;
                    data->robotPenalty[att_id+2] = 0;
                    data->penaltyAction = 2;
                    player[TEAM_YELLOW][att_id]->setX(-300);
                    player[TEAM_YELLOW][att_id]->setY(-20);
                    player[TEAM_YELLOW][att_id]->setAngle(155);
                }
                else{
                    player[TEAM_BLUE][goal_id]->setX(-725);
                    player[TEAM_BLUE][goal_id]->setY(0);
                    player[TEAM_BLUE][goal_id]->setAngle(0);
                    defende_penalty = true;
                }
            }
        }
        else{
            if(refereeClient->getLastFoulColor() == VSSRef::Color::BLUE){
                if(this->data->playTeam == TEAM_BLUE){
                    this->data->penalty = true;
                    data->penaltyAction = rand() % 2;
                    //data->penaltyAction = 0;
                    //cout << data->penaltyAction << endl;
                    if(data->penaltyAction == 0){
                        data->robotPenalty[att_id] = 0;
                        player[TEAM_BLUE][att_id]->setX(200);
                        player[TEAM_BLUE][att_id]->setY(-20);
                        player[TEAM_BLUE][att_id]->setAngle(8);
                        player[TEAM_BLUE][def_id]->setX(-200);
                        player[TEAM_BLUE][def_id]->setY(550);
                        player[TEAM_BLUE][def_id]->setAngle(-20);
                        player[TEAM_BLUE][goal_id]->setX(-650);
                        player[TEAM_BLUE][goal_id]->setY(0);
                        player[TEAM_BLUE][goal_id]->setAngle(90);
                    }
                    else{
                        data->robotPenalty[att_id+1] = 0;
                        player[TEAM_BLUE][att_id]->setX(290);
                        player[TEAM_BLUE][att_id]->setY(-130);
                        player[TEAM_BLUE][att_id]->setAngle(-120);
                        player[TEAM_BLUE][def_id]->setX(-50);
                        player[TEAM_BLUE][def_id]->setY(600);
                        player[TEAM_BLUE][def_id]->setAngle(-20);
                        player[TEAM_BLUE][goal_id]->setX(-650);
                        player[TEAM_BLUE][goal_id]->setY(0);
                        player[TEAM_BLUE][goal_id]->setAngle(90);
                    }
                }
                else{
                    player[TEAM_YELLOW][att_id]->setX(-300);
                    player[TEAM_YELLOW][att_id]->setY(-30);
                    player[TEAM_YELLOW][att_id]->setAngle(180);
                    player[TEAM_YELLOW][def_id]->setX(-100);
                    player[TEAM_YELLOW][def_id]->setY(-300);
                    player[TEAM_YELLOW][def_id]->setAngle(90);
                    player[TEAM_YELLOW][goal_id]->setX(725);
                    player[TEAM_YELLOW][goal_id]->setY(0);
                    player[TEAM_YELLOW][goal_id]->setAngle(180);
                    defende_penalty = true;
                }
            }
            else{
                if(this->data->playTeam == TEAM_YELLOW){
                    this->data->penalty = true;
                    data->penaltyAction = rand() % 2;
                    //cout << data->penaltyAction << endl;
                    if(data->penaltyAction == 0){
                        data->robotPenalty[att_id] = 0;
                        player[TEAM_YELLOW][att_id]->setX(-200);
                        player[TEAM_YELLOW][att_id]->setY(-20);
                        player[TEAM_YELLOW][att_id]->setAngle(-8);
                        player[TEAM_YELLOW][def_id]->setX(200);
                        player[TEAM_YELLOW][def_id]->setY(550);
                        player[TEAM_YELLOW][def_id]->setAngle(20);
                        player[TEAM_YELLOW][goal_id]->setX(650);
                        player[TEAM_YELLOW][goal_id]->setY(0);
                        player[TEAM_YELLOW][goal_id]->setAngle(90);
                    }
                    else{
                        data->robotPenalty[att_id+1] = 0;
                        player[TEAM_YELLOW][att_id]->setX(-290);
                        player[TEAM_YELLOW][att_id]->setY(-130);
                        player[TEAM_YELLOW][att_id]->setAngle(120);
                        player[TEAM_YELLOW][def_id]->setX(50);
                        player[TEAM_YELLOW][def_id]->setY(600);
                        player[TEAM_YELLOW][def_id]->setAngle(20);
                        player[TEAM_YELLOW][goal_id]->setX(650);
                        player[TEAM_YELLOW][goal_id]->setY(0);
                        player[TEAM_YELLOW][goal_id]->setAngle(90);
                    }
                }
                else{
                    player[TEAM_BLUE][att_id]->setX(100);
                    player[TEAM_BLUE][att_id]->setY(-30);
                    player[TEAM_BLUE][att_id]->setAngle(0);
                    player[TEAM_BLUE][def_id]->setX(100);
                    player[TEAM_BLUE][def_id]->setY(-300);
                    player[TEAM_BLUE][def_id]->setAngle(90);
                    player[TEAM_BLUE][goal_id]->setX(-725);
                    player[TEAM_BLUE][goal_id]->setY(0);
                    player[TEAM_BLUE][goal_id]->setAngle(0);
                    defende_penalty = true;
                }
            }
        }
        this->sendRef();
    }
    //REPOSICIONAMENTO KICKOFF
    else if(refereeClient->getLastFoul() == VSSRef::Foul::KICKOFF){
        if(this->data->playTeam == TEAM_BLUE){
            if(this->refereeClient->getLastFoulColor() == VSSRef::Color::BLUE){
                player[TEAM_BLUE][att_id]->setX(-70);
                player[TEAM_BLUE][att_id]->setY(-30);
                player[TEAM_BLUE][def_id]->setX(-350);
                player[TEAM_BLUE][def_id]->setY(0);
                player[TEAM_BLUE][def_id]->setAngle(0);
                player[TEAM_BLUE][att_id]->setAngle(27.5);
                player[TEAM_BLUE][goal_id]->setX(-700);
                player[TEAM_BLUE][goal_id]->setY(0);
                player[TEAM_BLUE][goal_id]->setAngle(90);
            }
            else{
                player[TEAM_BLUE][att_id]->setX(-230);
                player[TEAM_BLUE][att_id]->setY(-70);
                player[TEAM_BLUE][goal_id]->setX(-700);
                player[TEAM_BLUE][goal_id]->setY(0);
                player[TEAM_BLUE][goal_id]->setAngle(90);
                player[TEAM_BLUE][att_id]->setAngle(20);
                player[TEAM_BLUE][def_id]->setX(-500);
                player[TEAM_BLUE][def_id]->setY(0);
                player[TEAM_BLUE][def_id]->setAngle(0);
            }
        }
        else {
            if(this->refereeClient->getLastFoulColor() == VSSRef::Color::YELLOW){
                player[TEAM_YELLOW][att_id]->setX(100);
                player[TEAM_YELLOW][att_id]->setY(-15);
                player[TEAM_YELLOW][def_id]->setX(350);
                player[TEAM_YELLOW][def_id]->setY(0);
                player[TEAM_YELLOW][def_id]->setAngle(180);
                player[TEAM_YELLOW][att_id]->setAngle(165);
                player[TEAM_YELLOW][goal_id]->setX(700);
                player[TEAM_YELLOW][goal_id]->setY(0);
                player[TEAM_YELLOW][goal_id]->setAngle(90);
            }
            else{
                player[TEAM_YELLOW][att_id]->setX(220);
                player[TEAM_YELLOW][att_id]->setY(100);
                player[TEAM_YELLOW][att_id]->setAngle(210);
                player[TEAM_YELLOW][goal_id]->setX(700);
                player[TEAM_YELLOW][goal_id]->setY(0);
                player[TEAM_YELLOW][goal_id]->setAngle(90);
                player[TEAM_YELLOW][def_id]->setX(275);
                player[TEAM_YELLOW][def_id]->setY(-50);
                player[TEAM_YELLOW][def_id]->setAngle(0);
            }
        }
        this->sendRef();
    }
}

void Core::sendRef(){
    this->replacerClient->placeRobot(goal_id,this->player[this->data->playTeam][goal_id]->getX() / 1000.0,
                                    this->player[this->data->playTeam][goal_id]->getY() / 1000.0,
                                    this->player[this->data->playTeam][goal_id]->getAngle());
    this->replacerClient->placeRobot(def_id,this->player[this->data->playTeam][def_id]->getX() / 1000.0,
                                    this->player[this->data->playTeam][def_id]->getY() / 1000.0,
                                    this->player[this->data->playTeam][def_id]->getAngle());
    this->replacerClient->placeRobot(att_id,this->player[this->data->playTeam][att_id]->getX() / 1000.0,
                                    this->player[this->data->playTeam][att_id]->getY() / 1000.0,
                                    this->player[this->data->playTeam][att_id]->getAngle());
    this->replacerClient->sendFrame();
}
