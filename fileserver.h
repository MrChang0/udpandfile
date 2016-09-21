#ifndef FILESERVER_H
#define FILESERVER_H

#include "tcpclient.h"

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QFile>
#include <QList>
#include <QDebug>
#include<QHostAddress>

class fileserver : public QObject
{
    Q_OBJECT
public:
    explicit fileserver(QObject *parent = 0);
    void listen(QHostAddress serveraddress);
    void sendfile(QString filename,qint32 clientNum);
    void clear();
    void fininshone();
private:
    qint16 tcpPort;
    QTcpServer tcpServer;
    QFile *localFile;

    QList< tcpclient* > clients_;
    qint32 clientNum_;
    qint32 conneted_num;
signals:
    void getconneted(tcpclient *client);
public slots:
    void newclientconneted();
};

#endif // FILESERVER_H
