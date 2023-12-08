#ifndef PIGEONSERVER_H
#define PIGEONSERVER_H

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
    QList<QTcpSocket*> m_userConnections;   /// <-
    QStringList m_usersAvailable;           /// <- this should be wrapped into a hashmap<QString name, QTcpSocket connection>;
};
#endif // PIGEONSERVER_H
