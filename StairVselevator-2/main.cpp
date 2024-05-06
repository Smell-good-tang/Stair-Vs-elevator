#include "hfloors.h"

#include <QApplication>
#ifdef VLD_MODULE
#include "vld.h"
#endif
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    hfloors h;
    h.show();
    return a.exec();
}
