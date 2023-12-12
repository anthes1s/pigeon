#ifndef PIGEONSERVER_H
#define PIGEONSERVER_H

#include "pigeondatabase.h"

#include <QMainWindow>
#include <QMessageBox>
#include <QtNetwork>


QT_BEGIN_NAMESPACE
namespace Ui {
class pigeonserver;
}
QT_END_NAMESPACE

class pigeonserver : public QMainWindow
{
    Q_OBJECT

public:
    pigeonserver(QWidget *parent = nullptr);
    ~pigeonserver();

private slots:
    void newClient();
    void readFromClient();

private:
    Ui::pigeonserver *ui;
    QTcpServer* m_server = nullptr;
    pigeondatabase database;

    QMap<QString, QTcpSocket*> m_clients; //username and according socket;
};
#endif // PIGEONSERVER_H
