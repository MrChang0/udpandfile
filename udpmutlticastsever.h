#ifndef UDPMUTLTICASTSEVER_H
#define UDPMUTLTICASTSEVER_H

#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>
#include <QVector>
#include <QTimer>
#include <QByteArray>
#include <QDebug>
#include <QNetworkInterface>

class UDPMutlticastSever : public QObject
{
    Q_OBJECT
public:
    explicit UDPMutlticastSever(QObject *parent = 0);
    QList<QHostAddress>& getclients();
    void bind(QHostAddress addr);
    void sendfile(QList<QHostAddress> hosts);
signals:
    void getclient(QHostAddress client_addr);
public slots:
    void scan();
private slots:
    void gotmsg();
private:
    qint16 port_;
    QUdpSocket udpsocket_;
};

#endif // UDPMUTLTICASTSEVER_H
