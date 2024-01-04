#include "pigeonserver.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PigeonServer w;
    w.show();
    return a.exec();
}
