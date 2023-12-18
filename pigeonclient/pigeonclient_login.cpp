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

    if(ui->leLogin->text() != "") {
        m_main = new pigeonclient_main();
        m_main->setUsername(ui->leLogin->text());
        m_main->show();
        m_main->sendConnected();
        this->hide();
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

    QTcpSocket* tmp_socket = new QTcpSocket(this);

    connect(tmp_socket, &QTcpSocket::readyRead, this, [=](){
        QByteArray bytearr = tmp_socket->readAll();
        QDataStream out(&bytearr, QIODevice::ReadOnly);

        RequestType req_t;
        out >> req_t;

        qDebug() << req_t;

        switch(req_t) {
        case USER_REGISTRATION_FAIL:
        {
            QMessageBox::critical(this, "Pigeon", "User with such login and/or password already exists!");
        } break;
        case USER_REGISTRATION_SUCCESS:
        {
            QMessageBox::information(this, "Pigeon", "User registered successfully!");
            //then login into the server with username and password (create a public slot for that?)
        } break;


        //these were added just to avoid a retarded Qt warning T_T
        case USER_CONNECTED:
        case USER_DISCONNECTED:
        case USER_REGISTRATION:
        case SEND_MESSAGE:
        break;
        }

        tmp_socket->abort();
        delete tmp_socket;
    });

    connect(tmp_socket, &QTcpSocket::disconnected, tmp_socket, &QObject::deleteLater);

    tmp_socket->connectToHost(HOST_IP, 55030);

    QByteArray bytearr;
    QDataStream datastream(&bytearr, QIODevice::ReadWrite);

    datastream << USER_REGISTRATION << username << password;

    quint16 byteswritten = tmp_socket->write(bytearr);
    if(byteswritten < 0) {
        QMessageBox::critical(this, "Pigeon", tmp_socket->errorString());
        return;
    }
    tmp_socket->waitForBytesWritten();


}
