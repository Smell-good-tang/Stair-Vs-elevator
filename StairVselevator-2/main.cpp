#include "hfloors.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    hfloors h;
    h.show();
    return a.exec();
}
