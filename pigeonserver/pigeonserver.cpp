#include "pigeonserver.h"
#include "./ui_pigeonserver.h"

#include "hostconstants.h"
#include "requesttype.h"

PigeonServer::PigeonServer(QWidget *parent)
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

    if(!m_server->listen(QHostAddress::Any, HostConstants::HOST_PORT)) {
        QMessageBox::critical(this, "Pigeon Server", m_server->errorString());
        close();
    } else {
        ui->teMessageBox->append("Server is listening at: " + ipAddress + ' ' + "Port: " + QString::number(m_server->serverPort()));
    }

    connect(m_server, &QTcpServer::newConnection, this, &PigeonServer::newClient); /// when new tcpsocket is connected, add it to the qlist
}

PigeonServer::~PigeonServer()
{
    delete m_server;
    delete ui;
}

void PigeonServer::newClient() {
    QTcpSocket* client_connection = m_server->nextPendingConnection();

    connect(client_connection, &QTcpSocket::readyRead, this, &PigeonServer::readFromClient);

    connect(client_connection, &QTcpSocket::disconnected, this, [=](){
        //remove client from m_clients upon disconnection
        m_clients.removeIf([&, client_connection](const auto& client) {            
            return client.value() == client_connection;
        });
    });
}

void PigeonServer::readFromClient() {
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
        if(m_database.userExists(username, password) && !isOnline(username)) {
            //send USER_LOGIN_SUCCESS back
            QByteArray tmparr;
            QDataStream in(&tmparr, QIODevice::WriteOnly);

            in << USER_LOGIN_SUCCESS;

            qint16 byteswritten = sender_socket->write(tmparr);
            if(byteswritten < 0) {
                QMessageBox::critical(this, "Pigeon", sender_socket->errorString());
                return;
            }
            sender_socket->waitForBytesWritten();

        } else {
            //send USER_LOGIN_FAIL back
            QByteArray tmparr;
            QDataStream in(&tmparr, QIODevice::WriteOnly);

            in << USER_LOGIN_FAIL;

            qint16 byteswritten = sender_socket->write(tmparr);
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
    } break;

    case SEND_PRIVATE_MESSAGE:
    {
        QString username;
        QString receiver;
        QString message;

        out >> username >> receiver >> message;

        ui->teMessageBox->append(username + " sent " + message + " to " + receiver);
        //also add it to the database
        if(m_database.chatroomExists(username + receiver + "_msghistory")) {
            m_database.messageAdd(username + receiver + "_msghistory", username, message);
        } else {
            m_database.messageAdd(receiver + username + "_msghistory", username, message);
        }

        //send this back to receiver and sender
        {
            QByteArray tmparr;
            QDataStream in(&tmparr, QIODevice::WriteOnly);
            in << SEND_PRIVATE_MESSAGE << username << message;

            qint16 byteswritten = sender_socket->write(tmparr);
            if(byteswritten < 0) {
                QMessageBox::critical(this, "Pigeon", sender_socket->errorString());
                return;
            }
            sender_socket->waitForBytesWritten();
        }
        {
            if(isOnline(receiver)) {
                QByteArray tmparr;
                QDataStream in(&tmparr, QIODevice::WriteOnly);
                in << SEND_PRIVATE_MESSAGE << username << message;

                qint16 byteswritten = m_clients[receiver]->write(tmparr);
                if(byteswritten < 0) {
                    QMessageBox::critical(this, "Pigeon", m_clients[receiver]->errorString());
                    return;
                }
                m_clients[receiver]->waitForBytesWritten();
            }
        }

    } break;

    case USER_REGISTRATION:
    {
        QString username;
        QString password;

        out >> username >> password;

        if(m_database.userExists(username)) {

            ui->teMessageBox->append(username + " failed registration: USER ALREADY EXISTS");

            QByteArray reqarr;
            QDataStream in(&reqarr, QIODevice::WriteOnly);

            in << USER_REGISTRATION_FAIL;

            qint16 byteswritten = sender_socket->write(reqarr);
            if(byteswritten < 0) {
                QMessageBox::critical(this, "Pigeon Server", sender_socket->errorString());
                return;
            }
            sender_socket->waitForBytesWritten();
        } else {
            ui->teMessageBox->append(username + " successfuly finished registration!");
            m_database.userAdd(username, password);

            QByteArray reqarr;
            QDataStream in(&reqarr, QIODevice::WriteOnly);

            in << USER_REGISTRATION_SUCCESS;

            qint16 byteswritten = sender_socket->write(reqarr);
            if(byteswritten < 0) {
                QMessageBox::critical(this, "Pigeon Server", sender_socket->errorString());
                return;
            }
            sender_socket->waitForBytesWritten();
        }

    } break;
    case USER_SEARCH:
    {
        QString search;
        out >> search;

        {
            QStringList usersFound {m_database.userPrint(search)};

            QByteArray tmparr;
            QDataStream in(&tmparr, QIODevice::WriteOnly);

            in << USER_SEARCH << usersFound;

            qint16 byteswritten = sender_socket->write(tmparr);
            if(byteswritten < 0) {
                QMessageBox::critical(this, "Pigeon Server", sender_socket->errorString());
                return;
            }
            sender_socket->waitForBytesWritten();
        }

    } break;
    case GET_PRIVATE_MESSAGE_HISTORY:
    {
        QString sender;
        QString receiver;

        out >> sender >> receiver;
        //check if receiver+sender chatroom already exists and if so, send it

        QStringList msgHistory;

        if(m_database.chatroomExists(sender + receiver + "_msghistory")) {
            msgHistory = m_database.messageGetHistory(sender + receiver + "_msgHistory");
        } else if(m_database.chatroomExists(receiver + sender + "_msghistory")) {
            msgHistory = m_database.messageGetHistory(receiver + sender + "_msgHistory");
        } else {
            m_database.chatroomCreate(sender + receiver + "_msghistory");
        }

        {
            QByteArray tmparr;
            QDataStream in(&tmparr, QIODevice::WriteOnly);

            in << GET_PRIVATE_MESSAGE_HISTORY << msgHistory;

            qint16 byteswritten = sender_socket->write(tmparr);
            if(byteswritten < 0) {
                QMessageBox::critical(this, "Pigeon Server", sender_socket->errorString());
                return;
            }
            sender_socket->waitForBytesWritten();
        }
    } break;
    default:
    break;
    }
}

bool PigeonServer::isOnline(const QString& username) {
    if(m_clients.find(username) != m_clients.end()) {
        return true;
    } else {
        return false;
    }
}
