#pragma once

#include <QTcpSocket>
#include <QTcpServer>
#include <QObject>
#include <QByteArray>
#include <QString>
#include <memory>

class TcpClient : public QObject
{
    Q_OBJECT
public:
    TcpClient(const QString &login, const QString &pass, QObject* parent = nullptr);
    virtual ~TcpClient();

    bool isConnected() const;
    void sendMessage(QByteArray message);

private:
    const uint uTimer = 3000;
    const int port = 6534;
    bool fReadyRead = false;
    bool fConnected = false;
    bool fAuthorized = false;
    uint uidSession = 0;
    uint client_id = 0;
    uint timer_id = 3000;
    QString login;
    QString password;
    int id = 0;
    std::unique_ptr<QTcpSocket> tcp_sock;

    void calcData(const QString &data);
    void onReadData();
    void onWriteData(QByteArray data);
    void  testMessage();

    // QObject interface
protected:
    void timerEvent(QTimerEvent *ev);
};
