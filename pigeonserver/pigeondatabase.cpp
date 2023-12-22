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

    //create a table (IF NOT EXIST) for a global chatroom message history?
    createChatroom("global");
    messageGetHistory("global");

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

QStringList pigeondatabase::messageGetHistory(const QString& table) {
    QSqlQuery query;
    QStringList msgHistory;
    if(query.exec("SELECT * FROM " + table)) {
        while(query.next()) {
            msgHistory.push_back(query.value(0).toString() + " - " + query.value(1).toString());
            qDebug() << "MSGGETHISTORY" << query.value(0).toString() + " - " + query.value(1).toString();
        }
    } else {
        qDebug() << "Failed to execute a query";
    }


    return msgHistory;
}

void pigeondatabase::createChatroom(const QString& table) {
    QSqlQuery query;
    if(query.exec("CREATE TABLE IF NOT EXISTS " + table +
                   " (username VARCHAR(255) NOT NULL," +
                   "  message  VARCHAR(512) NOT NULL)")) {
    } else {
        qDebug() << "Failed to execute a query";
    }
}

void pigeondatabase::messageAdd(const QString& username, const QString& message) {
    QSqlQuery query;
    // i'll need to add a chatroomName parameter later
    if(query.exec("INSERT INTO global (username, message) VALUES ('" + username + "','" + message + "')")) {
        messageGetHistory("global");
    } else {
        qDebug() << "Failed to execute a query";
    }
}
