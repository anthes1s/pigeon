#include "pigeondatabase.h"

pigeondatabase::pigeondatabase(QObject *parent)
    : QObject{parent}, database{QSqlDatabase::addDatabase("QPSQL")}
{
    database.setHostName("localhost");     // replace with your PostgreSQL server host
    database.setDatabaseName("test");   // replace with your PostgreSQL database name
    database.setUserName("anthesis");       // replace with your PostgreSQL username
    database.setPassword("anthesis");       // replace with your PostgreSQL password
    database.setPort(5432);

    if(database.open()) {
        qDebug() << "Database connected!";
    } else qDebug() << "Database not connected!";

    userPrintEveryone();
    qDebug() << userExists("anthesis", "anthesispass");
}

pigeondatabase::~pigeondatabase()
{

}

///         USERS TABLE SCHEMA
///|------|--------------|-------------|
///|  ID  |   USERNAME   |  PASSWORD   |
///|------|--------------|-------------|

void pigeondatabase::userPrintEveryone() {
    QSqlQuery query;
    if(query.exec("SELECT * FROM users")) {
        while(query.next()) {
            qDebug() << query.value(0).toString()
                     << query.value(1).toString()
                     << query.value(2).toString();
        }
    } else {
        qDebug() << "Failed to execute a query";
    }
}

bool pigeondatabase::userExists(const QString& username, const QString& password) {
    QSqlQuery query;
    if(query.exec("SELECT * FROM users")) {
        while(query.next()) {
            if(query.value(1).toString() == username && query.value(2).toString() == password) {
            return true; /// user exists
            }
        }
        return false; /// user don't exist
    } else {
        qDebug() << "Failed to execute a query";
    }
    return false;
}

void pigeondatabase::userAdd() {

}
