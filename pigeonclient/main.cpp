#include "pigeonclient.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    pigeonclient w;
    w.show();
    return a.exec();
}
