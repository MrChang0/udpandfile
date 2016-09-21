#include "udpmutlticastsever.h"

const int SEVUDPPORT = 9632;
const int CLIUDPPORT = 9631;
const char* REQUEST = "request";
const char* RESPONSE = "response";
const char* SENDSTART = "sendstart";

UDPMutlticastSever::UDPMutlticastSever(QObject *parent) : QObject(parent),port_(SEVUDPPORT)
{
    QObject::connect(&this->udpsocket_,&QUdpSocket::readyRead,this,&UDPMutlticastSever::gotmsg);
}

void UDPMutlticastSever::gotmsg()
{
    QHostAddress clientaddress;
    quint16 port;
    QByteArray data;
    int size = this->udpsocket_.pendingDatagramSize();
    data.reserve(size);
    this->udpsocket_.readDatagram(data.data(),size,&clientaddress,&port);
    if(strncmp(data.data(),RESPONSE,size) == 0)
    {
        emit getclient(clientaddress);
    }
}

void UDPMutlticastSever::bind(QHostAddress addr)
{
    this->udpsocket_.bind(addr,this->port_);
}

void UDPMutlticastSever::scan()
{
    QByteArray data(REQUEST);
    this->udpsocket_.writeDatagram(data,QHostAddress::Broadcast,CLIUDPPORT);
}


 void UDPMutlticastSever::sendfile(QList<QHostAddress> hosts)
 {
     QByteArray data(SENDSTART);
    foreach(QHostAddress address,hosts)
    {
        this->udpsocket_.writeDatagram(data,address,CLIUDPPORT);
    }
 }
