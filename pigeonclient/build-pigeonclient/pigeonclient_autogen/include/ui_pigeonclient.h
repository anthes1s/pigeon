/********************************************************************************
** Form generated from reading UI file 'pigeonclient.ui'
**
** Created by: Qt User Interface Compiler version 6.6.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PIGEONCLIENT_H
#define UI_PIGEONCLIENT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_pigeonclient
{
public:
    QWidget *centralwidget;
    QLineEdit *leLogin;
    QLineEdit *lePassword;
    QPushButton *pbSignIn;
    QLabel *labelAppName;

    void setupUi(QMainWindow *pigeonclient)
    {
        if (pigeonclient->objectName().isEmpty())
            pigeonclient->setObjectName("pigeonclient");
        pigeonclient->resize(340, 600);
        centralwidget = new QWidget(pigeonclient);
        centralwidget->setObjectName("centralwidget");
        leLogin = new QLineEdit(centralwidget);
        leLogin->setObjectName("leLogin");
        leLogin->setGeometry(QRect(10, 180, 320, 26));
        lePassword = new QLineEdit(centralwidget);
        lePassword->setObjectName("lePassword");
        lePassword->setGeometry(QRect(10, 220, 320, 26));
        pbSignIn = new QPushButton(centralwidget);
        pbSignIn->setObjectName("pbSignIn");
        pbSignIn->setGeometry(QRect(130, 260, 87, 26));
        labelAppName = new QLabel(centralwidget);
        labelAppName->setObjectName("labelAppName");
        labelAppName->setGeometry(QRect(150, 150, 50, 18));
        pigeonclient->setCentralWidget(centralwidget);

        retranslateUi(pigeonclient);

        QMetaObject::connectSlotsByName(pigeonclient);
    } // setupUi

    void retranslateUi(QMainWindow *pigeonclient)
    {
        pigeonclient->setWindowTitle(QCoreApplication::translate("pigeonclient", "pigeonclient", nullptr));
        leLogin->setPlaceholderText(QCoreApplication::translate("pigeonclient", "Login", nullptr));
        lePassword->setPlaceholderText(QCoreApplication::translate("pigeonclient", "Password", nullptr));
        pbSignIn->setText(QCoreApplication::translate("pigeonclient", "Sign In", nullptr));
        labelAppName->setText(QCoreApplication::translate("pigeonclient", "Pigeon", nullptr));
    } // retranslateUi

};

namespace Ui {
    class pigeonclient: public Ui_pigeonclient {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PIGEONCLIENT_H
