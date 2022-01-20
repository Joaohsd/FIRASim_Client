#include "arguments.h"

Arguments::Arguments()
{

}

void Arguments::set_ip_Multicast_FIRA_Vision(QString _ip){
    this->info.network.ip_Multicast_FIRA_Vision = _ip;
}

void Arguments::set_ip_Local_Machine_FIRA(QString _ip){
    this->info.network.ip_Local_Machine_FIRA = _ip;
}

void Arguments::set_ip_Multicast_REF(QString _ip){
    this->info.network.ip_Multicast_REF = _ip;
}

void Arguments::set_port_FIRA_Vision(quint16 _port){
    this->info.network.port_FIRA_Vision = _port;
}

void Arguments::set_port_FIRA_Actuator(quint16 _port){
    this->info.network.port_FIRA_Actuator = _port;
}

void Arguments::set_port_REF(quint16 _port){
    this->info.network.port_REF = _port;
}

void Arguments::set_port_REF_REPLACER(quint16 _port){
    this->info.network.port_REF_REPLACER = _port;
}

void Arguments::set_testCondition(bool _test){
    this->info.testCondition = _test;
}

void Arguments::set_teamColor(bool _color){
    this->info.team.color = _color;
}

void Arguments::set_teamFormation(quint16 _formation){
    this->info.team.formation = _formation;
}

QString Arguments::get_ip_Multicast_FIRA_Vision(){
    return this->info.network.ip_Multicast_FIRA_Vision;
}

QString Arguments::get_ip_Local_Machine_FIRA(){
    return this->info.network.ip_Local_Machine_FIRA;
}

QString Arguments::get_ip_Multicast_REF(){
    return this->info.network.ip_Multicast_REF;
}

quint16 Arguments::get_port_FIRA_Vision(){
    return this->info.network.port_FIRA_Vision;
}

quint16 Arguments::get_port_FIRA_Actuator(){
    return this->info.network.port_FIRA_Actuator;
}

quint16 Arguments::get_port_REF(){
    return this->info.network.port_REF;
}

quint16 Arguments::get_port_REF_REPLACER(){
    return this->info.network.port_REF_REPLACER;
}

bool Arguments::getTestCondition(){
    return this->info.testCondition;
}

bool Arguments::get_teamColor(){
    return this->info.team.color;
}

quint16 Arguments::get_teamFormation(){
    return this->info.team.formation;
}

void Arguments::update(int argc, char **argv){
    if(argc > 0){
        for(int i = 1; i < argc; i++){
            std::stringstream info;
            info << argv[i];
            //std::cout << argv[i] << std::endl;
            if(i == 1){
                if(strcmp(argv[1],"blue") == 0)
                    this->info.team.color = false;
                else this->info.team.color = true;
            }
            else if(i == 2){
                this->info.network.ip_Local_Machine_FIRA = argv[2];
            }
            else if(i == 3){
                info >> this->info.network.port_FIRA_Vision;
            }
            else if(i == 4){
                info >> this->info.network.port_FIRA_Actuator;
            }
            else if(i == 5){
                this->info.network.ip_Multicast_FIRA_Vision = argv[5];
            }
            else if(i == 6){
                info >> this->info.network.port_REF;
            }
            else if(i == 7){
                info >> this->info.network.port_REF_REPLACER;
            }
            else if(i == 8){
                this->info.network.ip_Multicast_REF = argv[8];
            }
            else if(i == 9){
                info >> this->info.team.formation;
            }
            else if(i == 10){
                info >> this->info.testCondition;
            }
        }
    }
}

void Arguments::print(){
    QString color = this->info.team.color ? "YELLOW" : "BLUE";
    std::cout << "TIME = " << color.toStdString() << std::endl;
    std::cout << "FORMAÇÃO = " << this->info.team.formation << std::endl;
    std::cout << "IP_FIRASIM = " << this->info.network.ip_Local_Machine_FIRA.toStdString() << std::endl;
    std::cout << "PORT_VISAO = " << this->info.network.port_FIRA_Vision << std::endl;
    std::cout << "PORT_COMMAND = " << this->info.network.port_FIRA_Actuator << std::endl;
    std::cout << "MULTICAST_VISAO_FIRA = " << this->info.network.ip_Multicast_FIRA_Vision.toStdString() << std::endl;
    std::cout << "PORT_REF = " << this->info.network.port_REF << std::endl;
    std::cout << "PORT_REF_REPLACER = " << this->info.network.port_REF_REPLACER << std::endl;
    std::cout << "MULTICAST_REF = " << this->info.network.ip_Multicast_REF.toStdString() << std::endl;
    QString test = this->info.testCondition ? "YES" : "NO";
    std::cout << "TEST = " << test.toStdString() << std::endl;
}
