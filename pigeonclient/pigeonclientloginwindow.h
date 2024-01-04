#ifndef PIGEONCLIENTLOGINWINDOW_H
#define PIGEONCLIENTLOGINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>

#include "pigeonclientmainwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class pigeonclient;
}
QT_END_NAMESPACE

class PigeonClientLoginWindow : public QMainWindow
{
    Q_OBJECT

public:
    PigeonClientLoginWindow(QWidget *parent = nullptr);
    ~PigeonClientLoginWindow();

private slots:
    void signIn();
    void signUp();
    bool loginSuccessful();
    bool registrationSuccessful();

private:
    Ui::pigeonclient *ui;
    PigeonClientMainWindow* m_main = nullptr;
    QString m_username;
};
#endif // PIGEONCLIENTLOGINWINDOW_H
