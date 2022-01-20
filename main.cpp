#include <QCoreApplication>

#include <thread>
#include <utils/timer/timer.h>
#include <clients/vision/visionclient.h>
#include <clients/referee/refereeclient.h>
#include <clients/actuator/actuatorclient.h>
#include <clients/replacer/replacerclient.h>
#include <src/core.h>

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    Core *core = new Core();
    core->updateArguments(argc,argv);
    core->start();
    return a.exec();
}
