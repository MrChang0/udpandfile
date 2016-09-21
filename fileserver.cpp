#include "fileserver.h"

const quint16 TCPPORT = 9633;

fileserver::fileserver(QObject *parent) : QObject(parent),\
    conneted_num(0)
{
    QObject::connect(&this->tcpServer,&QTcpServer::newConnection,this,&fileserver::newclientconneted);
}

void fileserver::listen(QHostAddress serveraddress)
{
    if(!tcpServer.isListening())
        tcpServer.listen(serveraddress,TCPPORT);
}

void fileserver::sendfile(QString filename,qint32 clientNum)
{
    this->localFile = new QFile(filename);
    if(!localFile->open(QFile::ReadOnly))
    {
        qDebug() << "open file error!";
        return;
    }
    this->clientNum_ = clientNum;
}
void fileserver::newclientconneted()
{
    QTcpSocket *peersocket = this->tcpServer.nextPendingConnection();
    QHostAddress peeraddr = peersocket->peerAddress();
    tcpclient *client = new tcpclient(this->localFile,peersocket,peeraddr);
    emit getconneted(client);
    clients_.push_back(client);
    conneted_num++;
}

void fileserver::clear()
{
    qDeleteAll(clients_.begin(),clients_.end());
    clients_.clear();
}

void fileserver::fininshone()
{
    conneted_num--;
    if(conneted_num==0)
    {
        localFile->close();
    }
}
