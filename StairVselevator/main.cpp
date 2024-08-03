#include <QApplication>
#include <QScreen>

#include "SingleInstanceApp.h"
#include "efloors.h"
#ifdef VLD_MODULE
#include "vld.h"
#endif

int main(int argc, char *argv[])
{
    QApplication       a(argc, argv);
    efloors           *e              = new efloors();
    SingleInstanceApp *singleInstance = new SingleInstanceApp(e);
    QObject::connect(singleInstance, &SingleInstanceApp::showExistingWindow, e, [=]() {
        e->activateWindow();
        e->show();
    });
    e->show();
    return a.exec();
}
