#ifndef PIGEONCLIENT_MAIN_H
#define PIGEONCLIENT_MAIN_H

#include <QMainWindow>
#include <QtNetwork>
#include <QMessageBox>

namespace Ui {
class pigeonclient_main;
}

class pigeonclient_main : public QMainWindow
{
    Q_OBJECT

public:
    explicit pigeonclient_main(QWidget *parent = nullptr);
    explicit pigeonclient_main(QTcpSocket* socket = nullptr);
    ~pigeonclient_main();
    void setUsername(const QString&);

public slots:
    void sendConnected();

private slots:
    void sendMessage();
    void readFromServer();

private:
    Ui::pigeonclient_main *ui;
    QString m_username;
    QTcpSocket* m_socket = nullptr;
};

#endif // PIGEONCLIENT_MAIN_H
