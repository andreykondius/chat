#include "TcpClient.h"

#include <iostream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

TcpClient::TcpClient(const QString &login, const QString &pass, QObject *parent)
    : QObject(parent), login(login), password(pass)
{    
    tcp_sock = std::make_unique<QTcpSocket>();
    tcp_sock->connectToHost("127.0.0.1", port);
    tcp_sock->waitForConnected( uTimer );
    if (tcp_sock->state() == QAbstractSocket::ConnectedState)
    {
        fConnected = true;

        QString text ("{ \n\
                       \"id\":1,\n\
                       \"command\":\"HELLO\" \n\
                       }");
        QByteArray data( text.toStdString().c_str() );
        onWriteData(data);
    }

    connect(tcp_sock.get(), &QTcpSocket::connected, this, [this]()
    {
        fConnected = true;
    });

    connect(tcp_sock.get(), &QTcpSocket::disconnected, this, [this]()
    {
        fConnected = false;
    });

    connect(tcp_sock.get(), &QTcpSocket::readyRead, this, &TcpClient::onReadData, Qt::DirectConnection);

}

TcpClient::~TcpClient()
{

}

bool TcpClient::isConnected() const
{
    return fConnected;
}

void TcpClient::calcData(const QString &inp)
{
    QJsonParseError parseError;
    QJsonDocument d = QJsonDocument::fromJson(inp.toUtf8(), &parseError);
    if(parseError.error != QJsonParseError::NoError)
    {
        std::cout << "Error parsing of data" << std::endl;
        return;
    }
    QByteArray data;
    QJsonObject ob = d.object();
    QJsonValue value = ob.value("command");
    if (value.toString() == "HELLO")
    {
        data = "{ \n\
                \"id\":2, \n\
                \"command\":\"login\",\n\
                \"login\":\"";
        data.append(this->login);
        data.append("\",\n\
                    \"password\":\"");
        data.append(password);
        data.append("\"\n\
                     }");
        onWriteData(data);
    }
    else    if (value.toString() == "login")
    {
        QJsonValue status = ob.value("status");
        if (status.toString() == "ok")
        {
            fAuthorized = true;
            std::cout << "Authorize was successfull." << std::endl;
            QJsonValue session = ob.value("session");
            uidSession = atoi(session.toString().toStdString().c_str());
        }

        testMessage();
    }
    else    if (value.toString() == "message_reply")
    {
        QJsonValue status = ob.value("status");
        if (status.toString() == "ok")
        {
            QJsonValue cl = ob.value("client_id");
            client_id = atoi(cl.toString().toStdString().c_str());
        }
        data = "{ \n\
                    \"id\":2,\n\
                    \"command\":\"message_ok\",\n\
                    \"session\":\"";
        data.append(std::to_string(uidSession).c_str());
        data.append("\"\n\
                }");
        onWriteData(data);
    }
    else    if (value.toString() == "message")
    {
        data = "{ \n\
                \"id\":2,\n\
                \"command\":\"message_reply\",\n\
                \"status\":\"failed\",\n\
                \"message\":\"Authorization failed\"\n\
                }";
        if (fAuthorized)
        {
            data = "{ \n\
                \"id\":2,\n\
                \"command\":\"message_reply\",\n\
                \"status\":\"ok\",\n\
                \"client_id\":\"";
            data.append(std::to_string(client_id).c_str());
            data.append("\"\n\
                        }");
        }
        onWriteData(data);
        timer_id = startTimer(3000);
    }
    else    if (value.toString() == "ping_reply")
    {
        QByteArray logout("{\n\
                            \"id\":2,\n\
                            \"command\":\"logout\",\n\
                            \"session\":\"");
        logout.append(std::to_string(uidSession).c_str());
        logout.append("\"\n\
                        }");
        onWriteData(logout);
    }
}

void TcpClient::onReadData()
{    
    QTcpSocket* clientSocket = (QTcpSocket*)sender();
    QString data = QString::fromUtf8(clientSocket->readAll());
    std::cout << "Read the data: " << data.toStdString() << std::endl;
    calcData(data);
}

void TcpClient::onWriteData(QByteArray data)
{
    std::cout << "Write the data: " << data.toStdString() << std::endl;
    tcp_sock->write( data );
    tcp_sock->waitForBytesWritten( uTimer );
    tcp_sock->flush();
}

void  TcpClient::testMessage()
{
    sendMessage("The test nessage");
}

void TcpClient::timerEvent(QTimerEvent *ev)
{
    QByteArray ping("{\n\
                    \"id\":2,\n\
                    \"command\":\"ping\",\n\
                    \"session\":\"");
    ping.append(std::to_string(uidSession).c_str());
    ping.append("\"\n\
                }");
    onWriteData(ping);
    killTimer(timer_id);
}

void TcpClient::sendMessage(QByteArray message)
{
    QByteArray data("{\n\
               \"id\":2,\n\
               \"command\":\"message\",\n\
               \"body\":\"");
    data.append(message);
    data.append("\",\n\
               \"session\":\"");
    data.append(std::to_string(uidSession).c_str());
    data.append("\"\n\
                    }");
    onWriteData(data);
}
