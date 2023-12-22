#ifndef PIGEONDATABASE_H
#define PIGEONDATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>

class pigeondatabase : public QObject
{
    Q_OBJECT
public:
    explicit pigeondatabase(QObject *parent = nullptr);
    ~pigeondatabase();
signals:

public slots:
    void userPrintEveryone();
    bool userExists(const QString& username, const QString& password);
    bool userExists(const QString& username);
    void userAdd(const QString& username, const QString& password);
    QStringList messageGetHistory(const QString& table);
    void messageAdd(const QString& username, const QString& message);
    void createChatroom(const QString& chatroomName);

private:
    QSqlDatabase database;
};

#endif // PIGEONDATABASE_H
