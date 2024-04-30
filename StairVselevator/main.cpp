#include "efloors.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    efloors e;
    e.show();
    return a.exec();
}
