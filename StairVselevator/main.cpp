#include "efloors.h"

#include <QApplication>
#ifdef VLD_MODULE
#include "vld.h"
#endif
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    efloors e;
    e.show();
    return a.exec();
}
