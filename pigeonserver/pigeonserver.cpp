#include "pigeonserver.h"
#include "./ui_pigeonserver.h"

enum RequestType {
    USER_CONNECTED,
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
    connect(client_connection, &QTcpSocket::disconnected, this, &QObject::deleteLater);
}

void pigeonserver::readFromClient() {
    QByteArray bytearr = qobject_cast<QTcpSocket*>(sender())->readAll();
    QDataStream out(&bytearr, QIODevice::ReadOnly);

    RequestType req_t;
    out >> req_t;

    switch(req_t) {
    case USER_CONNECTED: {
        QString username;
        out >> username;
        ui->teMessageBox->append(username + " connected!");
        //send this back to clients

        for(auto client : m_userConnections) {
        QByteArray bytearr;
        QDataStream in(&bytearr, QIODevice::WriteOnly);
        in << username + " connected!";
        quint16 byteswritten = client->write(bytearr);
        if(byteswritten < 0) {
            QMessageBox::critical(this, "Pigeon", client->errorString());
            return;
        }
        client->waitForBytesWritten();
        }

    } break;
    case SEND_MESSAGE: {
        QString username;
        QString message;
        out >> username >> message;
        ui->teMessageBox->append(username + " sent " + message);
        //send this back to clients
        for(auto client : m_userConnections) {
            QByteArray bytearr;
            QDataStream in(&bytearr, QIODevice::WriteOnly);
            in << username + " - " + message;
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
