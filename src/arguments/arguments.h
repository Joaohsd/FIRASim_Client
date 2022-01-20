#ifndef ARGUMENTS_H
#define ARGUMENTS_H
#include <QString>
#include <sstream>
#include <iostream>

struct Network{
    //VARIÁVEIS DE IP
    QString ip_Local_Machine_FIRA;
    QString ip_Multicast_FIRA_Vision;
    QString ip_Multicast_REF;
    //VARIÁVEIS DE PORT
    quint16 port_FIRA_Vision;
    quint16 port_FIRA_Actuator;
    quint16 port_REF;
    quint16 port_REF_REPLACER;
};

struct Team{
    quint16 formation;
    bool color;
};

struct Info{
    Network network;
    Team team;
    bool testCondition;
};

class Arguments
{
public:
    Arguments();

    //Setters
    void set_ip_Local_Machine_FIRA(QString _ip);
    void set_ip_Multicast_FIRA_Vision(QString _ip);
    void set_ip_Multicast_REF(QString _ip);
    void set_port_FIRA_Vision(quint16 _port);
    void set_port_FIRA_Actuator(quint16 _port);
    void set_port_REF(quint16 _port);
    void set_port_REF_REPLACER(quint16 _port);

    void set_testCondition(bool _test);
    void set_teamColor(bool _color);
    void set_teamFormation(quint16 _formation);

    //Getters
    QString get_ip_Local_Machine_FIRA();
    QString get_ip_Multicast_FIRA_Vision();
    QString get_ip_Multicast_REF();
    quint16 get_port_FIRA_Vision();
    quint16 get_port_FIRA_Actuator();
    quint16 get_port_REF();
    quint16 get_port_REF_REPLACER();

    bool getTestCondition();
    bool get_teamColor();
    quint16 get_teamFormation();

    //Methods to update and print arguments passed to this code
    void update(int argc, char** argv);
    void print();
private:
    Info info;
};

#endif // ARGUMENTS_H
