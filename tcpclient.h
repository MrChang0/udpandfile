#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include <QList>
#include <QFile>
#include <QDataStream>

class tcpclient : public QObject
{
    Q_OBJECT
public:
    explicit tcpclient(QFile *file,QTcpSocket *socket,QHostAddress addr,QObject *parent = 0);
    QString getaddr() const;
private:
    qint64 totalBytes_;                               //总共数据大小
    qint64 bytesWritten_;                       //已发送数据大小
    qint64 bytesToWrite_;                     //还剩数据大小
    qint64 loadSize_;                              //缓冲区大小
    QString filename_;
    QByteArray outBlock;                     //数据缓冲区，即存放每次要发送的数据

    QTcpSocket *clientsock;
    QHostAddress addr;
    QFile *localfile_;
    int row;
signals:
    void finishsend(QString addr);
    void error(QString addr);
public slots:

private slots:
    void filedatasend(qint64 numByte);
    void msgrev();
};

#endif // TCPCLIENT_H
