#include "pigeonclientloginwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PigeonClientLoginWindow w;
    w.show();
    return a.exec();
}
