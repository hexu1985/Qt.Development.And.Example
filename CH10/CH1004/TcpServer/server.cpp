#include "server.h"
#include <QDebug>

Server::Server(QObject *parent,int port)
    :QTcpServer(parent)
{
    connect(this,SIGNAL(newConnection()),this,SLOT(slotNewConnection()));
    listen(QHostAddress::Any,port);
    qDebug() << "init Server ok!\n";
}

void Server::slotNewConnection()
{
    qDebug() << "onNewConnection!\n";
    QTcpSocket *tcpSocket = this->nextPendingConnection();
    TcpClientSocket *tcpClientSocket = new TcpClientSocket(tcpSocket, this);
    connect(tcpClientSocket,SIGNAL(updateClients(QString,int)),this,SLOT(updateClients(QString,int)));
    connect(tcpClientSocket,SIGNAL(disconnected(QTcpSocket *)),this,SLOT(slotDisconnected(QTcpSocket *)));

    tcpClientSocketList.append(tcpClientSocket);
}

void Server::updateClients(QString msg,int length)
{
    qDebug() << "updateClients!\n";
    emit updateServer(msg,length);
    for(int i=0;i<tcpClientSocketList.count();i++)
    {
        QTcpSocket *item = tcpClientSocketList.at(i)->getTcpSocket();
        if(item->write(msg.toLatin1(),length)!=length)
        {
            qDebug() << "write error!\n";
            continue;
        }
    }
}

void Server::slotDisconnected(QTcpSocket *tcpSocket)
{
    for(int i=0;i<tcpClientSocketList.count();i++)
    {
        TcpClientSocket *item = tcpClientSocketList.at(i);
        if(item->getTcpSocket()==tcpSocket)
        {
            tcpSocket->deleteLater();
            tcpClientSocketList.removeAt(i);
            return;
        }
    }
    return;
}
