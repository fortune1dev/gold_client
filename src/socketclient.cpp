#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "socketclient.hpp"

SocketClient::SocketClient(QObject *parent) : QObject(parent)
{

}

void SocketClient::Connect(QString host, quint16 port, QLabel *widget)
{
    m_socket = new QTcpSocket(this);
    connect(m_socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(m_socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(m_socket, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten(qint64)));
    connect(m_socket, SIGNAL(errorOccurred(QAbstractSocket::SocketError)), this, SLOT(slotError(QAbstractSocket::SocketError)));

    qDebug() << "Connecting to " + host + QString::number(port);

    m_socket->connectToHost(host, port);
    if (widget){
        m_label = widget;
        m_label->setText(tr("Connecting to the miner..."));
    }

}

void SocketClient::SendData(QByteArray data)
{
    m_socket->write(data);
    m_socket->waitForBytesWritten(1000);
}

void SocketClient::Disconnect()
{
    m_socket->disconnectFromHost();
}

QByteArray SocketClient::GetData()
{
    return m_recived_data;
}

void SocketClient::connected()
{
    qDebug() << "Connected!";
    if (m_label)
        m_label->setText(tr("Successfully connected to the miner."));
}

void SocketClient::disconnected()
{
    qDebug() << "Disconnected!";
//    if (m_label){
//        m_label->setText(tr("Disconnected from the miner."));
//    }
}

void SocketClient::bytesWritten(qint64 bytes)
{
    qDebug() << "We wrote: " << bytes;
}

void SocketClient::readyRead()
{
    m_recived_data =  m_socket->readAll();
}

void SocketClient::slotError(QAbstractSocket::SocketError err)
{
    QString strError =
    tr("Error: ") + (err == QAbstractSocket::HostNotFoundError ?
                 tr("The host was not found.") :
                 err == QAbstractSocket::RemoteHostClosedError ?
                 tr("The remote host is closed.") :
                 err == QAbstractSocket::ConnectionRefusedError ?
                 tr("The connection was refused.") :
                 QString(m_socket->errorString())
                );
    qDebug() << strError;
    if (m_label){
        m_label->setText(strError);
    }
}
