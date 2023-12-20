#include "pigeonclient_login.h"
#include "./ui_pigeonclient_login.h"

#include "requesttype.h"

pigeonclient::pigeonclient(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::pigeonclient)
{
    ui->setupUi(this);
    this->setFixedSize(this->size());    
    connect(ui->pbSignIn, &QPushButton::clicked, this, &pigeonclient::signIn);
    connect(ui->pbSignUp, &QPushButton::clicked, this, &pigeonclient::signUp);
}

pigeonclient::~pigeonclient()
{
    delete ui;
}

void pigeonclient::signIn() {
    QString username {ui->leLogin->text()};
    QString password {ui->lePassword->text()};

    if(!username.isEmpty() || !password.isEmpty()) {
        // send USER_LOGIN with username and password to server to process it
        QTcpSocket* tmp_socket = new QTcpSocket();
        qDebug() << tmp_socket;

        connect(tmp_socket, &QTcpSocket::readyRead, this, &pigeonclient::loginSuccessful);

        tmp_socket->connectToHost(HOST_IP, HOST_PORT);

        QByteArray bytearr;
        QDataStream in(&bytearr, QIODevice::WriteOnly);

        in << USER_LOGIN << username << password;

        quint16 byteswritten = tmp_socket->write(bytearr);
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

void pigeonclient::signUp() {
    QString username {ui->leLogin->text()};
    QString password {ui->lePassword->text()};

    if(username.isEmpty() || password.isEmpty()) {
        QMessageBox::critical(this, "Pigeon", "Login and Password cannot be empty");
        return;
    }

    QTcpSocket* tmp_socket = new QTcpSocket();

    connect(tmp_socket, &QTcpSocket::readyRead, this, &pigeonclient::registrationSuccessful);

    tmp_socket->connectToHost(HOST_IP, HOST_PORT);

    QByteArray bytearr;
    QDataStream datastream(&bytearr, QIODevice::ReadWrite);

    datastream << USER_REGISTRATION << username << password;

    quint16 byteswritten = tmp_socket->write(bytearr);
    if(byteswritten < 0) {
        QMessageBox::critical(this, "Pigeon", tmp_socket->errorString());
        return;
    }
    tmp_socket->waitForBytesWritten();
    tmp_socket->waitForReadyRead();
}

bool pigeonclient::loginSuccessful() {
    QString username {ui->leLogin->text()};

    QTcpSocket* sender_socket = qobject_cast<QTcpSocket*>(sender());

    QByteArray bytearr = sender_socket->readAll();
    QDataStream out(&bytearr, QIODevice::ReadOnly);

    RequestType req_t;
    out >> req_t;

    qDebug() << req_t << "ReadyRead _login";

    switch(req_t) {
    case USER_LOGIN_SUCCESS:
    {
        qDebug() << "Login successful!";

        sender_socket->disconnectFromHost();
        disconnect(sender_socket, nullptr, nullptr, nullptr);

        qDebug() << "socket" << sender_socket << "should be disconnected";

        m_main = new pigeonclient_main(sender_socket);
        m_main->setUsername(username);
        m_main->sendConnected();
        m_main->show();
        this->close();

        return true;
    } break;
    case USER_LOGIN_FAIL:
    {
        qDebug() << "Login failed!";
        sender_socket->disconnectFromHost();
        disconnect(sender_socket, nullptr, nullptr, nullptr);
        QMessageBox::critical(this, "Pigeon", "Login failed!");
        return false;
    } break;
    //these were added just to avoid a retarded Qt warning T_T
    case USER_LOGIN:
    case USER_CONNECTED:
    case USER_DISCONNECTED:
    case USER_REGISTRATION:
    case USER_REGISTRATION_SUCCESS:
    case USER_REGISTRATION_FAIL:
    case SEND_MESSAGE:
        break;
    }
    return false;
}

bool pigeonclient::registrationSuccessful() {
    QTcpSocket* sender_socket = qobject_cast<QTcpSocket*>(sender());

    QByteArray bytearr = sender_socket->readAll();
    QDataStream out(&bytearr, QIODevice::ReadOnly);

    RequestType req_t;
    out >> req_t;

    qDebug() << req_t;

    switch(req_t) {
    case USER_REGISTRATION_FAIL:
    {
        QMessageBox::critical(this, "Pigeon", "User with such login and/or password already exists!");
        sender_socket->disconnectFromHost();
        disconnect(sender_socket, nullptr, nullptr, nullptr);
        return false;
    } break;
    case USER_REGISTRATION_SUCCESS:
    {
        QMessageBox::information(this, "Pigeon", "User registered successfully!");
        sender_socket->disconnectFromHost();
        disconnect(sender_socket, nullptr, nullptr, nullptr); // this disconnects EVERY signal/slot relationship
        return true;
    } break;


        //these were added just to avoid a retarded Qt warning T_T
    case USER_LOGIN:
    case USER_LOGIN_SUCCESS:
    case USER_LOGIN_FAIL:
    case USER_CONNECTED:
    case USER_DISCONNECTED:
    case USER_REGISTRATION:
    case SEND_MESSAGE:
        break;
    }
    return false;
}
