#ifndef PIGEONSERVER_H
#define PIGEONSERVER_H

#include "pigeondatabase.h"
#include "requesthandler.h"

#include <QMainWindow>
#include <QMessageBox>
#include <QtNetwork>


QT_BEGIN_NAMESPACE
namespace Ui {
class pigeonserver;
}
QT_END_NAMESPACE

class PigeonServer : public QMainWindow
{
    Q_OBJECT

public:
    PigeonServer(QWidget *parent = nullptr);
    ~PigeonServer();

public slots:
    void newClient();
    void readFromClient();
    bool isOnline(const QString& username);

private:
    Ui::pigeonserver *ui;
    QTcpServer* m_server = nullptr;
    PigeonDatabase m_database;
    RequestHandler m_req_handler;
    QMap<QString, QTcpSocket*> m_clients; //username and according socket;
};
#endif // PIGEONSERVER_H
