#ifndef CONFIGDAO_H
#define CONFIGDAO_H

#include <QObject>
#include <QFile>
#include <QString>
#include <QTextStream>
#include <QJsonDocument>
#include <QVariantMap>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <iostream>
#include <src/staticdata.h>

using namespace std;

class ConfigDAO : public QObject
{
    Q_OBJECT
public:
    ConfigDAO();
    static ConfigDAO *Instance();

    bool readJSON();
    void print();

    // GETTERS
    QJsonObject getObjectJson();

    // Network
    QString get_ip_Local_Machine_FIRA();
    QString get_ip_Multicast_FIRA_Vision();
    QString get_ip_Multicast_REF();
    quint16 get_port_FIRA_Vision();
    quint16 get_port_FIRA_Actuator();
    quint16 get_port_REF();
    quint16 get_port_REF_REPLACER();
    // Team
    bool getTestCondition();
    bool get_teamColor();
    quint16 get_teamFormation();

private:
    // Management of files
    QString fileName;
    QString fileBuffer;
    QFile *file;

    // Internal json parse vars
    QJsonDocument document;
    QVariantMap documentMap;

    // Network data
    // IP
    QString ip_Local_Machine_FIRA;
    QString ip_Multicast_FIRA_Vision;
    QString ip_Multicast_REF;
    // PORT
    quint16 port_FIRA_Vision;
    quint16 port_FIRA_Actuator;
    quint16 port_REF;
    quint16 port_REF_REPLACER;

    // Team data
    quint16 formation;
    quint16 qtdRobots;
    bool color;
    bool testCondition;

    // Self object
    static ConfigDAO *object;

    QString readFile(QString &filename);
    void readNetwork();
    void readTeam();
};

#endif // CONFIGDAO_H
