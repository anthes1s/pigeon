#include "pigeonserver.h"
#include "./ui_pigeonserver.h"

enum RequestType {
    USER_CONNECTED,
    USER_DISCONNECTED,
    SEND_MESSAGE,
};

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

    if(!m_server->listen(QHostAddress::Any, 55030)) {
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
    m_userConnections.push_back(client_connection);
    connect(client_connection, &QTcpSocket::readyRead, this, &pigeonserver::readFromClient);

    connect(client_connection, &QTcpSocket::disconnected, this, [=](){
        for(int i{0}; i < m_userConnections.size(); ++i) {
            if(m_userConnections[i] == client_connection) {
                //now we have found the client thats disconnected and deleting it
                QString disconnectedUser = m_usersAvailable[i]; // we're saving this clients name to notify other clients that this user has disconnected
                ui->teMessageBox->append(m_usersAvailable[i] + " disconnected!");
                m_userConnections.erase(m_userConnections.cbegin() + i);
                m_usersAvailable.erase(m_usersAvailable.cbegin() + i);
                //now we send back to available users that certain client has disconnected;
                for(auto& client : m_userConnections) {
                    QByteArray bytearr;
                    QDataStream in(&bytearr, QIODevice::WriteOnly);
                    in << USER_DISCONNECTED << disconnectedUser << m_usersAvailable;

                    quint16 byteswritten = client->write(bytearr);
                    if(byteswritten < 0) {
                        QMessageBox::critical(this, "Pigeon Server", client->errorString());
                    }
                    client->waitForBytesWritten();
                }
                break;
            }
        }
    });
    qDebug() << m_userConnections;
}

void pigeonserver::readFromClient() {
    QByteArray bytearr = qobject_cast<QTcpSocket*>(sender())->readAll();
    QDataStream out(&bytearr, QIODevice::ReadOnly);

    RequestType req_t;
    out >> req_t;

    switch(req_t) {
    case USER_CONNECTED:
    {
        QString username;
        out >> username;
        ui->teMessageBox->append(username + " connected!");
        m_usersAvailable << username;
        qDebug() << m_usersAvailable;
        //send this back to clients

        for(auto client : m_userConnections) {
        QByteArray bytearr;
        QDataStream in(&bytearr, QIODevice::WriteOnly);
        in << USER_CONNECTED << username << m_usersAvailable;
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
        //send this back to clients

        for(auto client : m_userConnections) {
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

    default:
    break;
    }
}
