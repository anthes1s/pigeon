#ifndef PIGEONCLIENT_H
#define PIGEONCLIENT_H

#include <QMainWindow>
#include <QtNetwork>

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
    QString m_username;
};
#endif // PIGEONCLIENT_H
