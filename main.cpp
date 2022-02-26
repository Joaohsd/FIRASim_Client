#include <QCoreApplication>

//Including Clients classes
#include <clients/vision/visionclient.h>
#include <clients/referee/refereeclient.h>
#include <clients/actuator/actuatorclient.h>
#include <clients/replacer/replacerclient.h>

//Including Core class
#include <src/core.h>

//main function
int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    //Creating a pointer to Core object
    Core *core = new Core();
    //Updating arguments passed to this code
    core->updateArguments(argc,argv);
    //Starting Core object
    core->start();
    return a.exec();
}
