#include "configdao.h"

ConfigDAO* ConfigDAO::object = NULL;

ConfigDAO::ConfigDAO()
{
    this->fileName = QString(QString(PROJECT_PATH) + "/utils/config/config.json");
    this->file = new QFile();
    this->file->setFileName(this->fileName);
    this->file->open(QIODevice::ReadOnly | QIODevice::Text);
    fileBuffer = file->readAll();
    file->close();
    this->readJSON();
}

QString ConfigDAO::get_ip_Multicast_FIRA_Vision(){
    return this->ip_Multicast_FIRA_Vision;
}

QString ConfigDAO::get_ip_Local_Machine_FIRA(){
    return this->ip_Local_Machine_FIRA;
}

QString ConfigDAO::get_ip_Multicast_REF(){
    return this->ip_Multicast_REF;
}

quint16 ConfigDAO::get_port_FIRA_Vision(){
    return this->port_FIRA_Vision;
}

quint16 ConfigDAO::get_port_FIRA_Actuator(){
    return this->port_FIRA_Actuator;
}

quint16 ConfigDAO::get_port_REF(){
    return this->port_REF;
}

quint16 ConfigDAO::get_port_REF_REPLACER(){
    return this->port_REF_REPLACER;
}

bool ConfigDAO::getTestCondition(){
    return this->testCondition;
}

bool ConfigDAO::get_teamColor(){
    return this->color;
}

quint16 ConfigDAO::get_teamFormation(){
    return this->formation;
}

ConfigDAO *ConfigDAO::Instance()
{
    if (!object){
        object = new ConfigDAO();
    }
    return object;
}

bool ConfigDAO::readJSON()
{
    if (this->fileBuffer.isEmpty()) {
        return false;
    }
    this->document = QJsonDocument::fromJson(this->fileBuffer.toUtf8());
    if (this->document.isNull()) {
        return false;
    }
    if (this->document.isArray()) {
        return false;
    }
    if (this->document.isObject()) {
        this->documentMap = this->document.object().toVariantMap();
    }
    else{
        return false;
    }

    // Reading data from json file
    readNetwork();
    readTeam();

    return true;
}

void ConfigDAO::readNetwork(){
    QVariantMap networkMap = documentMap["NETWORK"].toMap();
    this->ip_Local_Machine_FIRA = QString(networkMap["NETWORK_IP"].toString());

    // FIRASim
    QVariantMap firaSimMap = networkMap["FIRASIM"].toMap();
    QVariantMap visionMap = firaSimMap["VISION"].toMap();
    this->ip_Multicast_FIRA_Vision = visionMap["MULTICAST_IP"].toString();
    this->port_FIRA_Vision = visionMap["PORT"].toUInt();
    QVariantMap actuatorMap = firaSimMap["ACTUATOR"].toMap();
    this->port_FIRA_Actuator = actuatorMap["PORT"].toUInt();

    // Referee
    QVariantMap refereeMap = networkMap["REFEREE"].toMap();
    QVariantMap clientMap = refereeMap["CLIENT"].toMap();
    this->ip_Multicast_REF = clientMap["MULTICAST_IP"].toString();
    this->port_REF = clientMap["PORT"].toUInt();
    QVariantMap replacerMap = refereeMap["REPLACER"].toMap();
    this->port_REF_REPLACER = replacerMap["PORT"].toUInt();
}

void ConfigDAO::readTeam(){
    QVariantMap teamMap = documentMap["TEAM"].toMap();

    // Team
    if(strcmp(QString(teamMap["COLOR"].toString()).toStdString().c_str(),"blue") == 0){
        this->color = false;
    }
    else{
        this->color = true;
    }
    this->qtdRobots = teamMap["QTDROBOTS"].toUInt();
    this->formation = teamMap["FORMATION"].toUInt();
    this->testCondition = teamMap["TEST"].toBool();
}

void ConfigDAO::print(){
    QString color = this->color ? "YELLOW" : "BLUE";
    std::cout << "TIME = " << color.toStdString() << std::endl;
    std::cout << "FORMA????O = " << this->formation << std::endl;
    std::cout << "IP_FIRASIM = " << this->ip_Local_Machine_FIRA.toStdString() << std::endl;
    std::cout << "PORT_VISAO = " << this->port_FIRA_Vision << std::endl;
    std::cout << "PORT_COMMAND = " << this->port_FIRA_Actuator << std::endl;
    std::cout << "MULTICAST_VISAO_FIRA = " << this->ip_Multicast_FIRA_Vision.toStdString() << std::endl;
    std::cout << "PORT_REF = " << this->port_REF << std::endl;
    std::cout << "PORT_REF_REPLACER = " << this->port_REF_REPLACER << std::endl;
    std::cout << "MULTICAST_REF = " << this->ip_Multicast_REF.toStdString() << std::endl;
    QString test = this->testCondition ? "YES" : "NO";
    std::cout << "TEST = " << test.toStdString() << std::endl;
}
