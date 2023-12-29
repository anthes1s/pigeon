#include "pigeonclient_main.h"
#include "ui_pigeonclient_main.h"

#include "hostconstants.h"
#include "requesttype.h"

pigeonclient_main::pigeonclient_main(QWidget *parent) : QMainWindow(parent), ui(new Ui::pigeonclient_main)
{
}

pigeonclient_main::pigeonclient_main(QTcpSocket* socket)
    : ui(new Ui::pigeonclient_main), m_socket(qMove(socket))
{
    ui->setupUi(this);
    this->setFixedSize(this->size());
    ui->teMessageBox->setReadOnly(true);
    ui->teMessageBox->append("Try to find someone in the left corner!1");

    m_socket->connectToHost(hostconstants::HOST_IP, hostconstants::HOST_PORT);
    disconnect(m_socket, nullptr, nullptr, nullptr);

    connect(ui->pbSend, &QPushButton::clicked, this, &pigeonclient_main::sendMessage);

    connect(ui->leSearch, &QLineEdit::textChanged, this, [&](){
        ui->lwSearchResults->clear();
        if(ui->leSearch->text().isEmpty()) return;

        ui->leSearch->setReadOnly(true);


        QString search {ui->leSearch->text()};

        QByteArray bytearr;
        QDataStream in(&bytearr, QIODevice::WriteOnly);

        in << USER_SEARCH << search;

        qDebug() << search;

        quint16 byteswritten = m_socket->write(bytearr);
        if(byteswritten < 0) QMessageBox::critical(this, "Pigeon", m_socket->errorString());
        m_socket->waitForBytesWritten();
        m_socket->waitForReadyRead();

        ui->leSearch->setReadOnly(false);

    });
    connect(ui->lwSearchResults, &QListWidget::itemDoubleClicked, this, [&](QListWidgetItem* item){
    //ask for a message history between sender and receiver
        QString receiver {item->text()};
        m_receiver = receiver;
        qDebug() << "itemSelect:" << m_receiver;

        QByteArray bytearr;
        QDataStream in(&bytearr, QIODevice::WriteOnly);

        in << GET_PRIVATE_MESSAGE_HISTORY << m_username << receiver;

        quint16 byteswritten = m_socket->write(bytearr);
        if(byteswritten < 0) {
            QMessageBox::critical(this, "Pigeon", m_socket->errorString());
            return;
        }
        m_socket->waitForBytesWritten();
        m_socket->waitForReadyRead();
    });
    connect(m_socket, &QTcpSocket::readyRead, this, &pigeonclient_main::readFromServer);
    connect(m_socket, &QTcpSocket::disconnected, this, &QObject::deleteLater);

    qDebug() << "_main socket" << m_socket;
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

    datastream << SEND_PRIVATE_MESSAGE << m_username << m_receiver << ui->leMessage->text();
    qDebug() << "sendMessage:" << m_username << m_receiver << ui->leMessage->text();

    quint16 byteswritten = m_socket->write(bytearr);
    if(byteswritten < 0) {
        QMessageBox::critical(this, "Pigeon", m_socket->errorString());
    }
    m_socket->waitForBytesWritten();

    ui->leMessage->clear();
}

void pigeonclient_main::readFromServer() {
    QByteArray bytearr = m_socket->readAll();
    QDataStream out (&bytearr, QIODevice::ReadOnly);

    RequestType req_t;
    out >> req_t;

    switch(req_t) {
    case USER_CONNECTED:
    {
        QString username;
        QStringList msgHistory;
        out >> username >> msgHistory;
        for(auto& msg : msgHistory) ui->teMessageBox->append(msg);

    } break;
    case USER_DISCONNECTED:
    {
        QString username;
        out >> username;
        ui->teMessageBox->append(username + " disconnected!");

    } break;
    case SEND_MESSAGE:
    {
        QString username;
        QString message;
        out >> username >> message;       
        ui->teMessageBox->append(username + " - " + message);
    } break;
    case USER_SEARCH:
    {
        QStringList usersFound;
        out >> usersFound;
        for(auto& user : usersFound) ui->lwSearchResults->addItem(user);
    } break;
    case GET_PRIVATE_MESSAGE_HISTORY:
    {
        QStringList msgHistory;
        out >> msgHistory;
        qDebug() << msgHistory;
        ui->teMessageBox->clear();
        for(auto& msg : msgHistory) ui->teMessageBox->append(msg);
    } break;
    case SEND_PRIVATE_MESSAGE:
    {
        QString username;
        QString message;
        out >> username >> message;
        ui->teMessageBox->append(username + " - " + message);
    } break;
    default:
    break;
    }
}
