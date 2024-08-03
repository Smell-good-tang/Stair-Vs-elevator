#include <QApplication>
#include <QSharedMemory>

#include "SingleInstanceApp.h"
#include "hfloors.h"
#ifdef VLD_MODULE
#include "vld.h"
#endif

int main(int argc, char *argv[])
{
    QApplication       a(argc, argv);
    hfloors           *h              = new hfloors();
    SingleInstanceApp *singleInstance = new SingleInstanceApp(h);
    QObject::connect(singleInstance, &SingleInstanceApp::showExistingWindow, h, [=]() {
        h->activateWindow();
        h->show();
    });
    h->show();
    return a.exec();
}
