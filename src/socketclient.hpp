#ifndef SOCKETCLIENT_HPP
#define SOCKETCLIENT_HPP

#include <QObject>
#include <QDebug>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QLabel>

class SocketClient : public QObject
{
    Q_OBJECT
public:
    explicit SocketClient(QObject *parent = nullptr);
    void Connect(QString host, quint16 port, QLabel *widget = nullptr);
    void SendData(QByteArray data);
    void Disconnect();
    QByteArray GetData();

signals:

public slots:

    void connected();
    void disconnected();
    void bytesWritten(qint64 bytes);
    void readyRead();
    void slotError(QAbstractSocket::SocketError err);

private:
    QTcpSocket *m_socket;
    QLabel *m_label = nullptr;
    QByteArray m_recived_data;

};

#endif // SOCKETCLIENT_HPP
