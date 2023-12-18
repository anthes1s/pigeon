#include "pigeondatabase.h"

pigeondatabase::pigeondatabase(QObject *parent)
    : QObject{parent}, database{QSqlDatabase::addDatabase("QPSQL")}
{
    database.setHostName("localhost");
    database.setDatabaseName("pigeondatabase");
    database.setUserName("postgres");
    database.setPassword("postgres");
    database.setPort(5432);

    if(database.open()) {
        qDebug() << "Database connected!";
    } else qDebug() << "Database not connected!";

    userPrintEveryone();
    qDebug() << userExists("root", "root");
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

bool pigeondatabase::userExists(const QString& username) {
    QSqlQuery query;
    if(query.exec("SELECT username FROM users")) {
        while(query.next()) {
            if(query.value(0).toString() == username) {
                return true; /// user exists
            }
        }
        return false; /// user don't exist
    } else {
        qDebug() << "Failed to execute a query";
    }
    return false;
}

void pigeondatabase::userAdd(const QString& username, const QString& password) {
    QSqlQuery query;
    if(query.exec("INSERT INTO users (username, password) VALUES ('" + username + "','" + password + "')")) {
        userPrintEveryone();
    } else {
        qDebug() << "Failed to execute a query";
    }
}
