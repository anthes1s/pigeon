#include "pigeonserver.h"
#include "./ui_pigeonserver.h"

#include "requesttype.h"

pigeonserver::pigeonserver(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::pigeonserver), m_server(new QTcpServer(this))
{
    ui->setupUi(this);
    ui->teMessageBox->setReadOnly(true);
    this->setFixedSize(this->size());

    QString ipAddress;
    QList<QHostAddress> ipAddressList = QNetworkInterface::allAddresses();

    for(QHostAddress& ip : ipAddressList) {
        if(ip != QHostAddress::LocalHost && ip.toIPv4Address()) {
            ipAddress = ip.toString();
            break;
        }
    }

    if(!m_server->listen(QHostAddress::Any, HOST_PORT)) {
        QMessageBox::critical(this, "Pigeon Server", m_server->errorString());
        close();
    } else {
        ui->teMessageBox->append("Server is listening at: " + ipAddress + ' ' + "Port: " + QString::number(m_server->serverPort()));
    }

    connect(m_server, &QTcpServer::newConnection, this, &pigeonserver::newClient); /// when new tcpsocket is connected, add it to the qlist
}

pigeonserver::~pigeonserver()
{
    delete m_server;
    delete ui;
}

void pigeonserver::newClient() {
    QTcpSocket* client_connection = m_server->nextPendingConnection();

    connect(client_connection, &QTcpSocket::readyRead, this, &pigeonserver::readFromClient);

    connect(client_connection, &QTcpSocket::disconnected, this, [=](){
        //remove client from m_clients upon disconnection
        m_clients.removeIf([&, client_connection](const auto& client) {            
            return client.value() == client_connection;
        });
    });
}

void pigeonserver::readFromClient() {
    QTcpSocket* sender_socket = qobject_cast<QTcpSocket*>(sender());

    QByteArray bytearr = sender_socket->readAll();
    QDataStream out(&bytearr, QIODevice::ReadOnly);

    RequestType req_t;
    out >> req_t;

    switch(req_t) {
    case USER_LOGIN:
    {
        QString username;
        QString password;
        out >> username >> password;

        //if user exists in database and is not online
        if(database.userExists(username, password) && m_clients.find(username) == m_clients.end()) {
            //send USER_LOGIN_SUCCESS back

            qDebug() << "User" << username << "with password" << password << "exists!";

            QByteArray tmparr;
            QDataStream in(&tmparr, QIODevice::WriteOnly);

            RequestType req_t;

            in << USER_LOGIN_SUCCESS;

            quint16 byteswritten = sender_socket->write(tmparr);
            if(byteswritten < 0) {
                QMessageBox::critical(this, "Pigeon", sender_socket->errorString());
                return;
            }
            sender_socket->waitForBytesWritten();

        } else {
            //send USER_LOGIN_FAIL back
            qDebug() << "User" << username << "with password" << password << "don't exists!";

            QByteArray tmparr;
            QDataStream in(&tmparr, QIODevice::WriteOnly);

            RequestType req_t;

            in << USER_LOGIN_FAIL;

            quint16 byteswritten = sender_socket->write(tmparr);
            if(byteswritten < 0) {
                QMessageBox::critical(this, "Pigeon", sender_socket->errorString());
                return;
            }
            sender_socket->waitForBytesWritten();
        }
    } break;
    case USER_CONNECTED:
    {
        QString username;
        out >> username;
        ui->teMessageBox->append(username + " connected!");
        m_clients.insert(username, sender_socket);
        qDebug() << "QMAP" << m_clients;

        //send this back to clients
        for(auto& client : m_clients) {
        QByteArray bytearr;
        QDataStream in(&bytearr, QIODevice::WriteOnly);

        in << USER_CONNECTED << username;

        quint16 byteswritten = client->write(bytearr);
        if(byteswritten < 0) {
            QMessageBox::critical(this, "Pigeon", client->errorString());
            return;
        }
        client->waitForBytesWritten();
        }

    } break;

    case SEND_MESSAGE:
    {
        QString username;
        QString message;

        out >> username >> message;

        ui->teMessageBox->append(username + " sent " + message);
        qDebug() << m_clients;
        //send this back to clients
        for(auto& client : m_clients) {
            QByteArray bytearr;
            QDataStream in(&bytearr, QIODevice::WriteOnly);
            in << SEND_MESSAGE << username << message;
            quint16 byteswritten = client->write(bytearr);
            if(byteswritten < 0) {
                QMessageBox::critical(this, "Pigeon", client->errorString());
                return;
            }
            client->waitForBytesWritten();
        }
    } break;

    case USER_REGISTRATION:
    {
        QString username;
        QString password;

        out >> username >> password;

        if(database.userExists(username)) {

            ui->teMessageBox->append(username + " failed registration: USER ALREADY EXISTS");

            QByteArray reqarr;
            QDataStream in(&reqarr, QIODevice::WriteOnly);

            in << USER_REGISTRATION_FAIL;

            quint16 byteswritten = sender_socket->write(reqarr);
            if(byteswritten < 0) {
                QMessageBox::critical(this, "Pigeon Server", sender_socket->errorString());
            }
            sender_socket->waitForBytesWritten();
        } else {
            ui->teMessageBox->append(username + " successfuly finished registration!");
            database.userAdd(username, password);

            QByteArray reqarr;
            QDataStream in(&reqarr, QIODevice::WriteOnly);

            in << USER_REGISTRATION_SUCCESS;

            quint16 byteswritten = sender_socket->write(reqarr);
            if(byteswritten < 0) {
                QMessageBox::critical(this, "Pigeon Server", sender_socket->errorString());
            }
            sender_socket->waitForBytesWritten();
        }

    } break;

    default:
    break;
    }
}

bool pigeonserver::isOnline(const QString& username) {
    return false;
}
