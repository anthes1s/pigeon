#include "pigeonserver.h"
#include "./ui_pigeonserver.h"

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

    ui->teMessageBox->append(client_connection->peerAddress().toString() + " connected!");
}

void pigeonserver::readFromClient() {

}
