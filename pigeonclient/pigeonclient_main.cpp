#include "pigeonclient_main.h"
#include "ui_pigeonclient_main.h"

enum RequestType {
    USER_CONNECTED,
    USER_DISCONNECTED, /// delete user from lwUsers when a client disconnects
    SEND_MESSAGE,
};

pigeonclient_main::pigeonclient_main(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::pigeonclient_main), m_socket(new QTcpSocket(this))
{
    ui->setupUi(this);
    this->setFixedSize(this->size());
    ui->teMessageBox->setReadOnly(true);

    m_socket->connectToHost("192.168.1.123", 55030);

    connect(ui->pbSend, &QPushButton::clicked, this, &pigeonclient_main::sendMessage);
    connect(m_socket, &QTcpSocket::readyRead, this, &pigeonclient_main::readFromServer);
    connect(m_socket, &QTcpSocket::disconnected, this, &QObject::deleteLater); /// when disconnected send&get a request from a server to delete a client
    qDebug() << m_socket;
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
    ///
    /// When we connect to the socket we need to ask for a list of people that are already connected to the server and we need to refresh this list
    /// each time a new user connects/disconnects to the server; DO LOGIN/REGISTRATION FIRST YOU FAGGOT!!!
    ///
    switch(req_t) {
    case USER_CONNECTED:
    {
        QString username;
        QStringList usersAvailable;
        out >> username >> usersAvailable;
        qDebug() << usersAvailable;

        ui->lwUsers->clear();

        for(auto& user : usersAvailable) {
            ui->lwUsers->addItem(user);
        }
    } break;
    case USER_DISCONNECTED:
    {
        QString username;
        QStringList usersAvailable;
        out >> username >> usersAvailable;
        qDebug() << usersAvailable;

        ui->lwUsers->clear();

        for(auto& user : usersAvailable) {
            ui->lwUsers->addItem(user);
        }

    } break;
    case SEND_MESSAGE:
    {
        QString username;
        QString message;
        out >> username >> message;
        qDebug() << username << message;
        ///message do not display for some reason :(
        ui->teMessageBox->append(username + " - " + message);
    } break;
    default:
    break;
    }

    // ui->teMessageBox->append(message);
}
