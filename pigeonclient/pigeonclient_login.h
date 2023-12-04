#ifndef PIGEONCLIENT_LOGIN_H
#define PIGEONCLIENT_LOGIN_H

#include <QMainWindow>
#include <QMessageBox>

#include "pigeonclient_main.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class pigeonclient;
}
QT_END_NAMESPACE

class pigeonclient : public QMainWindow
{
    Q_OBJECT

public:
    pigeonclient(QWidget *parent = nullptr);
    ~pigeonclient();

private slots:
    void signIn();

private:
    Ui::pigeonclient *ui;
    pigeonclient_main* m_main = nullptr;
    QString m_username;
};
#endif // PIGEONCLIENT_LOGIN_H
