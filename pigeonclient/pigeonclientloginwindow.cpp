#include "pigeonclientloginwindow.h"
#include "./ui_pigeonclientloginwindow.h"

#include "hostconstants.h"
#include "requesttype.h"

PigeonClientLoginWindow::PigeonClientLoginWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::pigeonclient)
{
    ui->setupUi(this);
    this->setFixedSize(this->size());
    connect(ui->pbSignIn, &QPushButton::clicked, this, &PigeonClientLoginWindow::signIn);
    connect(ui->pbSignUp, &QPushButton::clicked, this, &PigeonClientLoginWindow::signUp);
}

PigeonClientLoginWindow::~PigeonClientLoginWindow()
{
    delete ui;
}

void PigeonClientLoginWindow::signIn() {
    QString username {ui->leLogin->text()};
    QString password {ui->lePassword->text()};

    if(!username.isEmpty() || !password.isEmpty()) {
        // send USER_LOGIN with username and password to server to process it
        QTcpSocket* tmp_socket = new QTcpSocket();
        qDebug() << "_login socket" << tmp_socket;
        
        connect(tmp_socket, &QTcpSocket::readyRead, this, &PigeonClientLoginWindow::loginSuccessful);

        tmp_socket->connectToHost(HostConstants::HOST_IP, HostConstants::HOST_PORT);

        QByteArray bytearr;
        QDataStream in(&bytearr, QIODevice::WriteOnly);

        in << USER_LOGIN << username << password;

        qint16 byteswritten = tmp_socket->write(bytearr);
        if(byteswritten < 0) {
            QMessageBox::critical(this, "Pigeon", tmp_socket->errorString());
            return;
        }
        tmp_socket->waitForBytesWritten();
        tmp_socket->waitForReadyRead();
    } else {
        QMessageBox::critical(this, "Pigeon", "Incorrect Login and/or Password");
    }

}

void PigeonClientLoginWindow::signUp() {
    QString username {ui->leLogin->text()};
    QString password {ui->lePassword->text()};

    if(username.isEmpty() || password.isEmpty()) {
        QMessageBox::critical(this, "Pigeon", "Login and Password cannot be empty");
        return;
    }

    QTcpSocket* tmp_socket = new QTcpSocket();
    
    connect(tmp_socket, &QTcpSocket::readyRead, this, &PigeonClientLoginWindow::registrationSuccessful);

    tmp_socket->connectToHost(HostConstants::HOST_IP, HostConstants::HOST_PORT);

    QByteArray bytearr;
    QDataStream datastream(&bytearr, QIODevice::ReadWrite);

    datastream << USER_REGISTRATION << username << password;

    qint16 byteswritten = tmp_socket->write(bytearr);
    if(byteswritten < 0) {
        QMessageBox::critical(this, "Pigeon", tmp_socket->errorString());
        return;
    }
    tmp_socket->waitForBytesWritten();
    tmp_socket->waitForReadyRead();
}

bool PigeonClientLoginWindow::loginSuccessful() {
    QString username {ui->leLogin->text()};

    QTcpSocket* sender_socket = qobject_cast<QTcpSocket*>(sender());

    QByteArray bytearr = sender_socket->readAll();
    QDataStream out(&bytearr, QIODevice::ReadOnly);

    RequestType req_t;
    out >> req_t;

    switch(req_t) {
    case USER_LOGIN_SUCCESS:
    {
        sender_socket->disconnectFromHost();
        disconnect(sender_socket, &QTcpSocket::readyRead, nullptr, nullptr);

        m_main = new PigeonClientMainWindow(qMove(sender_socket));
        m_main->setUsername(username);
        m_main->sendConnected();
        m_main->show();
        this->close();

        return true;
    } break;
    case USER_LOGIN_FAIL:
    {
        sender_socket->disconnectFromHost();
        disconnect(sender_socket, &QTcpSocket::readyRead, nullptr, nullptr);
        delete sender_socket;
        QMessageBox::critical(this, "Pigeon", "Login failed!");
        return false;
    } break;

    default:
        break;
    }
    return false;
}

bool PigeonClientLoginWindow::registrationSuccessful() {
    QTcpSocket* sender_socket = qobject_cast<QTcpSocket*>(sender());

    QByteArray bytearr = sender_socket->readAll();
    QDataStream out(&bytearr, QIODevice::ReadOnly);

    RequestType req_t;
    out >> req_t;

    switch(req_t) {
    case USER_REGISTRATION_FAIL:
    {
        QMessageBox::critical(this, "Pigeon", "User with such login and/or password already exists!");
        sender_socket->disconnectFromHost();
        disconnect(sender_socket, &QTcpSocket::readyRead, nullptr, nullptr);
        delete sender_socket;
        return false;
    } break;
    case USER_REGISTRATION_SUCCESS:
    {
        QMessageBox::information(this, "Pigeon", "User registered successfully!");
        sender_socket->disconnectFromHost();
        disconnect(sender_socket, &QTcpSocket::readyRead, nullptr, nullptr);
        delete sender_socket;
        return true;
    } break;

    default:
        break;
    }
    return false;
}
