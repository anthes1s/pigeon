#include "pigeonclient.h"
#include "./ui_pigeonclient.h"

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

}
