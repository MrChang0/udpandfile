#include "tcpclient.h"
const char* ERRORREV = "rev_error";

tcpclient::tcpclient(QFile *file,QTcpSocket *socket,QHostAddress addr,QObject *parent) : \
    QObject(parent),
    totalBytes_(file->size()),
    bytesToWrite_(totalBytes_),
    bytesWritten_(0),
    loadSize_(4*1024),
    localfile_(file),
    clientsock(socket),
    addr(addr)
{
    QString name = file->fileName();
    this->filename_ = name.right(name.size() - name.lastIndexOf('/') - 1);
    QObject::connect(clientsock,&QTcpSocket::bytesWritten,this,&tcpclient::filedatasend);
    QObject::connect(clientsock,&QTcpSocket::readyRead,this,&tcpclient::msgrev);

    QString currentFileName = filename_;
    QString datasend = QString::number(totalBytes_) + QString(" ") + currentFileName + QString("\n");

    bytesWritten_ -= datasend.size();
    clientsock->write(datasend.toLocal8Bit().data(),datasend.size());
    outBlock.resize(4*1024);
}

void tcpclient::filedatasend(qint64 numByte)
{
    bytesWritten_ += numByte;
    if(bytesToWrite_>0)
    {
        localfile_->seek(bytesWritten_);
        outBlock = localfile_->read(qMin(loadSize_,bytesToWrite_));

        bytesToWrite_ -= (int)clientsock->write(outBlock);
    }
    else
    {
        emit finishsend(addr.toString());
    }
}

QString tcpclient::getaddr() const
{
    return addr.toString();
}

void tcpclient::msgrev()
{
    QByteArray data = clientsock->readAll();
    if(strcmp(data.data(),ERRORREV) == 0)
        emit error(addr.toString());
}
