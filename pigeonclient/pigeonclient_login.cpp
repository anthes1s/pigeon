#include "pigeonclient_login.h"
#include "./ui_pigeonclient_login.h"

pigeonclient::pigeonclient(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::pigeonclient)
{
    ui->setupUi(this);
    this->setFixedSize(this->size());

    connect(ui->pbSignIn, &QPushButton::clicked, this, &pigeonclient::signIn);
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
