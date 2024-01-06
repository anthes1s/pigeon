#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include <QObject>

class RequestHandler : public QObject
{
    Q_OBJECT
public:
    explicit RequestHandler(QObject *parent = nullptr);
public slots:

signals:

private:

};

#endif // REQUESTHANDLER_H
