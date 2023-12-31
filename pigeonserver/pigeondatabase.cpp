#include "pigeondatabase.h"

PigeonDatabase::PigeonDatabase(QObject *parent)
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

    QSqlQuery query;
    if(query.exec("CREATE TABLE IF NOT EXISTS users (id BIGSERIAL NOT NULL, username VARCHAR(255) NOT NULL, password VARCHAR(512) NOT NULL)")) {
    } else {
        qDebug() << "Failed to execute a query";
    }

    userPrintEveryone();
}

PigeonDatabase::~PigeonDatabase()
{
}

///         USERS TABLE SCHEMA
///|------|--------------|-------------|
///|  ID  |   USERNAME   |  PASSWORD   |
///|------|--------------|-------------|

void PigeonDatabase::userPrintEveryone() {
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

QStringList PigeonDatabase::userPrint(const QString& username) {
    QSqlQuery query;
    QStringList usersFound;
    if(query.exec("SELECT username FROM users WHERE username LIKE '%" + username + "%'")) {
        while(query.next()) {
            usersFound.push_back(query.value(0).toString());
        }
    } else {
        qDebug() << "Failed to execute a query";
    }
    return usersFound;
}

bool PigeonDatabase::userExists(const QString& username, const QString& password) {
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

bool PigeonDatabase::userExists(const QString& username) {
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

void PigeonDatabase::userAdd(const QString& username, const QString& password) {
    QSqlQuery query;
    if(query.exec("INSERT INTO users (username, password) VALUES ('" + username + "','" + password + "')")) {
        userPrintEveryone();
    } else {
        qDebug() << "Failed to execute a query";
    }
}

QStringList PigeonDatabase::messageGetHistory(const QString& table) {
    QSqlQuery query;
    QStringList msgHistory;
    if(query.exec("SELECT * FROM " + table)) {
        while(query.next()) {
            msgHistory.push_back(query.value(0).toString() + " - " + query.value(1).toString());
        }
    } else {
        qDebug() << "Failed to execute a query";
    }

    qDebug() << msgHistory;

    return msgHistory;
}

void PigeonDatabase::chatroomCreate(const QString& table) {
    QSqlQuery query;
    if(query.exec("CREATE TABLE IF NOT EXISTS " + table +
                   " (username VARCHAR(255) NOT NULL," +
                   "  message  VARCHAR(512) NOT NULL)")) {
    } else {
        qDebug() << "Failed to execute a query";
    }
}

/*
void PigeonDatabase::messageAdd(const QString& username, const QString& message) {
    QSqlQuery query;
    // i'll need to add a chatroomName parameter later
    if(query.exec("INSERT INTO global (username, message) VALUES ('" + username + "','" + message + "')")) {
        messageGetHistory("global");
    } else {
        qDebug() << "Failed to execute a query";
    }
}
*/

void PigeonDatabase::messageAdd(const QString& table, const QString& username, const QString& message) {
    QSqlQuery query;
    // i'll need to add a chatroomName parameter later
    if(query.exec("INSERT INTO " + table + " (username, message) VALUES ('" + username + "','" + message + "')")) {
        messageGetHistory(table);
    } else {
        qDebug() << "messageAdd: Failed to execute a query";
    }
}

bool PigeonDatabase::chatroomExists(const QString& chatroomName) {
    QSqlQuery query;
    if(query.exec("SELECT table_name FROM information_schema.tables")) {
        while(query.next()) {
            if(query.value(0).toString() == chatroomName) {
                return true;
            }
        }
    } else {
        qDebug() << "Failed to execute a query";
    }
    return false;
}


