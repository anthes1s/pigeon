#include "pigeonserver.h"
#include "./ui_pigeonserver.h"

#include "hostconstants.h"
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

    if(!m_server->listen(QHostAddress::Any, hostconstants::HOST_PORT)) {
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
    } break;

    case SEND_PRIVATE_MESSAGE:
    {
        QString username;
        QString receiver;
        QString message;

        out >> username >> receiver >> message;
        qDebug() << "received: " << username << receiver << message;

        ui->teMessageBox->append(username + " sent " + message + " to " + receiver);
        //also add it to the database
        if(database.chatroomExists(username + receiver + "_msghistory")) {
            database.messageAdd(username + receiver + "_msghistory", username, message);
        } else {
            database.messageAdd(receiver + username + "_msghistory", username, message);
        }

        //send this back to receiver and sender
        {
            QByteArray bytearr;
            QDataStream in(&bytearr, QIODevice::WriteOnly);
            in << SEND_PRIVATE_MESSAGE << username << message;

            quint16 byteswritten = sender_socket->write(bytearr);
            if(byteswritten < 0) {
                QMessageBox::critical(this, "Pigeon", sender_socket->errorString());
                return;
            }
            sender_socket->waitForBytesWritten();

            if(isOnline(receiver)) {
                QByteArray bytearr;
                QDataStream in(&bytearr, QIODevice::WriteOnly);
                in << SEND_PRIVATE_MESSAGE << username << message;

                quint16 byteswritten = m_clients[receiver]->write(bytearr);
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

        if(database.userExists(username)) {

            ui->teMessageBox->append(username + " failed registration: USER ALREADY EXISTS");

            QByteArray reqarr;
            QDataStream in(&reqarr, QIODevice::WriteOnly);

            in << USER_REGISTRATION_FAIL;

            quint16 byteswritten = sender_socket->write(reqarr);
            if(byteswritten < 0) {
                QMessageBox::critical(this, "Pigeon Server", sender_socket->errorString());
                return;
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
                return;
            }
            sender_socket->waitForBytesWritten();
        }

    } break;
    case USER_SEARCH:
    {
        QString search;
        out >> search;
        qDebug() << search;

        {
            QStringList usersFound {database.userPrint(search)};

            QByteArray tmparr;
            QDataStream in(&tmparr, QIODevice::WriteOnly);

            in << USER_SEARCH << usersFound;

            quint16 byteswritten = sender_socket->write(tmparr);
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
        qDebug() << sender << receiver;

        QStringList msgHistory;

        if(database.chatroomExists(sender + receiver + "_msghistory")) {
            msgHistory = database.messageGetHistory(sender + receiver + "_msgHistory");
        } else if(database.chatroomExists(receiver + sender + "_msghistory")) {
            msgHistory = database.messageGetHistory(receiver + sender + "_msgHistory");
        } else {
            database.chatroomCreate(sender + receiver + "_msghistory");
        }

        {
            QByteArray tmparr;
            QDataStream in(&tmparr, QIODevice::WriteOnly);

            in << GET_PRIVATE_MESSAGE_HISTORY << msgHistory;
            qDebug() << sender + receiver << msgHistory;

            quint16 byteswritten = sender_socket->write(tmparr);
            if(byteswritten < 0) {
                QMessageBox::critical(this, "Pigeon Server", sender_socket->errorString());
                return;
            }
            sender_socket->waitForBytesWritten();
        }
    }
    default:
    break;
    }
}

bool pigeonserver::isOnline(const QString& username) {
    if(m_clients.find(username) != m_clients.end()) {
        return true;
    } else {
        return false;
    }
}
