#pragma once

#include <QTcpSocket>
#include <QTcpServer>
#include <QString>
#include <QObject>
#include <memory>

struct StructSock
{
    uint id=0;
    bool auth=false;
    QTcpSocket* sock;
};

using MapClients = QMap<int, StructSock>;

class TcpServer : public QObject
{
    Q_OBJECT
public:
    TcpServer(const QString &login, const QString &pass, QObject* parent = nullptr);
    virtual ~TcpServer();
    void Start();
    void sendMessage(const uint descr, QByteArray message);

private:
    const uint uTimer = 3000;
    const uint port = 6534;
    uint uidSession = 0;
    uint countClients = 0;
    QString login;
    QString password;
    QMetaObject::Connection connTcpServ;
    MapClients clients;
    std::unique_ptr<QTcpServer> tcp_serv;

    void calcData(const int descr, const QString &data);
    void onReadData();
    void onWriteData(const uint descr, const QString data);
    void onWriteData(QTcpSocket *sock, const QString data);
    void socketDisconnect(const uint descr);

private slots:
    void onNewuser();
};
