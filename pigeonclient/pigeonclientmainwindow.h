#ifndef PIGEONCLIENTMAINWINDOW_H
#define PIGEONCLIENTMAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>
#include <QMessageBox>

namespace Ui {
class pigeonclient_main;
}

class PigeonClientMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PigeonClientMainWindow(QWidget *parent = nullptr);
    explicit PigeonClientMainWindow(QTcpSocket* socket = nullptr);
    ~PigeonClientMainWindow();
    void setUsername(const QString&);

public slots:
    void sendConnected();

private slots:
    void sendMessage();
    void readFromServer();

private:
    Ui::pigeonclient_main *ui;
    QString m_username;
    QString m_receiver;
    QTcpSocket* m_socket = nullptr;
};

#endif // PIGEONCLIENTMAINWINDOW_H
