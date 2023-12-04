#include "pigeonclient_main.h"
#include "ui_pigeonclient_main.h"

enum RequestType {
    USER_CONNECTED,
    SEND_MESSAGE,
};

pigeonclient_main::pigeonclient_main(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::pigeonclient_main), m_socket(new QTcpSocket(this))
{
    ui->setupUi(this);
    this->setFixedSize(this->size());
    ui->teMessageBox->setReadOnly(true);

    m_socket->connectToHost("192.168.1.109", 55030);

    connect(ui->pbSend, &QPushButton::clicked, this, &pigeonclient_main::sendMessage);
    connect(m_socket, &QTcpSocket::readyRead, this, &pigeonclient_main::readFromServer);
}

pigeonclient_main::~pigeonclient_main()
{
    m_socket->abort();
    delete m_socket;
    delete ui;
}

void pigeonclient_main::setUsername(const QString& username) {
    m_username = username;
}

void pigeonclient_main::sendConnected() {
    QByteArray bytearr;
    QDataStream datastream(&bytearr, QIODevice::ReadWrite);

    datastream << USER_CONNECTED << m_username;

    quint16 byteswritten = m_socket->write(bytearr);
    if(byteswritten < 0) {
        QMessageBox::critical(this, "Pigeon", m_socket->errorString());
    }

    m_socket->waitForBytesWritten();
}

void pigeonclient_main::sendMessage() {
    QByteArray bytearr;
    QDataStream datastream(&bytearr, QIODevice::WriteOnly);

    datastream << SEND_MESSAGE << m_username << ui->leMessage->text();

    ui->leMessage->clear();

    quint16 byteswritten = m_socket->write(bytearr);
    if(byteswritten < 0) {
        QMessageBox::critical(this, "Pigeon", m_socket->errorString());
    }

    m_socket->waitForBytesWritten();
}

void pigeonclient_main::readFromServer() {
    QByteArray bytearr = m_socket->readAll();
    QDataStream out (&bytearr, QIODevice::ReadOnly);

    QString message;
    out >> message;

    ui->teMessageBox->append(message);
}
