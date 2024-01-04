#ifndef PIGEONDATABASE_H
#define PIGEONDATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>

class PigeonDatabase : public QObject
{
    Q_OBJECT
public:
    explicit PigeonDatabase(QObject *parent = nullptr);
    ~PigeonDatabase();
signals:

public slots:
    QStringList userPrint(const QString& username);
    void userPrintEveryone(); //change return type maybe?
    bool userExists(const QString& username, const QString& password);
    bool userExists(const QString& username);
    void userAdd(const QString& username, const QString& password);
    QStringList messageGetHistory(const QString& table);
    //void messageAdd(const QString& username, const QString& message);
    void messageAdd(const QString& table, const QString& username, const QString& message);
    void chatroomCreate(const QString& chatroomName);
    bool chatroomExists(const QString& chatroomName);

private:
    QSqlDatabase database;
};

#endif // PIGEONDATABASE_H
