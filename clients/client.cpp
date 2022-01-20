#include "client.h"

Client::Client(QString serverAddress, quint16 serverPort) {
    // Creating socket pointer
    _clientSocket = new QUdpSocket();
    //Setting network parameters
    _serverAddress = serverAddress;
    _serverPort = serverPort;
    _isConnected = false;
    //Create a event and action when readyRead event occurs
    QObject::connect(_clientSocket,SIGNAL(readyRead()), this, SLOT(run()));
}

void Client::run() {
    if(!_isConnected) {
        connectToNetwork();
        _isConnected = true;
    }
    runClient();
}

void Client::connect(){
    if(!_isConnected) {
        connectToNetwork();
        _isConnected = true;
    }
}

void Client::close() {
    if(_isConnected) {
        disconnectFromNetwork();
        _isConnected = false;
    }
}
