#include "TcpServer.h"

#include <iostream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

TcpServer::TcpServer(const QString &login, const QString &pass, QObject *parent)
    : QObject(parent), login(login), password(pass)
{
    std::cout <<  "The set login/password: " << login.toStdString() << "/" << password.toStdString() << std::endl;
    tcp_serv = std::make_unique<QTcpServer>();
}

TcpServer::~TcpServer()
{

}

void TcpServer::Start()
{
    connTcpServ = connect(tcp_serv.get(), &QTcpServer::newConnection, this, &TcpServer::onNewuser);

    if (!tcp_serv->listen(QHostAddress::Any, port))
    {
        std::cout <<  "Unable to start the server: %1." << tcp_serv->errorString().toStdString().c_str() << std::endl;
        return;
    }
    std::cout <<  tcp_serv->isListening() << "TCPSocket listen on port: " << port << std::endl;
}

void TcpServer::onNewuser()
{
    QTcpSocket *clientSocket = new QTcpSocket();
    clientSocket = tcp_serv->nextPendingConnection();
    const uint descr = clientSocket->socketDescriptor();
    clients[descr] = {0, false, clientSocket};
    std::cout << "Added the new connection" << std::endl;
    connect(clientSocket, &QTcpSocket::readyRead, this, &TcpServer::onReadData);
}

void TcpServer::onReadData()
{
    QTcpSocket* clientSocket = (QTcpSocket*)sender();
    QString data = QString::fromUtf8(clientSocket->readAll());
    std::cout << "Read the data: " << data.toStdString() << std::endl;
    const uint descr = clientSocket->socketDescriptor();
    calcData(descr, data);
}

void TcpServer::onWriteData(const uint descr, const QString data)
{
    std::cout << "Write the data: \"" << data.toStdString() << "\"" << std::endl;
    clients[descr].sock->write( data.toStdString().c_str() );
    clients[descr].sock->waitForBytesWritten( uTimer );
}

void TcpServer::onWriteData(QTcpSocket *sock, const QString data)
{
    std::cout << "Write the data: \"" << data.toStdString() << "\"" << std::endl;
    sock->write( data.toStdString().c_str() );
    sock->waitForBytesWritten( uTimer );
}

void TcpServer::calcData(const int descr, const QString &inp)
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
                \"id\":1, \n\
                \"command\":\"HELLO\",\n\
                \"auth_method\":\"plain-text\" \n\
                }";
    }
    else    if (value.toString() == "login")
    {
        QJsonValue login = ob.value("login");
        QJsonValue password = ob.value("password");

        data = "{\n\
               \"id\":2,\n\
               \"command\":\"login\",\n\
                    \"status\":\"failed\",\n\
                    \"message\":\"Authorization failed.\"\n\
                    }";

        if (login.toString() == this->login && password.toString() == this->password)
        {
            data = ("{\n\
                         \"id\":2,\n\
                         \"command\":\"login\",\n\
                        \"status\":\"ok\",\n\
                        \"session\":\"");
            ++uidSession;
            data.append(std::to_string(uidSession).c_str());
            data.append("\"\n\
                        }");
            clients[descr].id = uidSession;
            clients[descr].auth = true;
        }
    }
    else    if (value.toString() == "message")
    {
        data = "{\n\
                \"id\":2,\n\
                \"command\":\"message_reply\",\n\
                \"status\":\"failed\",\n\
                \"message\":\"Authorization failed. Message not delivered.\"\n\
                }";
        if (clients[descr].auth)
        {
            data = "{\n\
                    \"id\":2,\n\
                    \"command\":\"message_reply\",\n\
                    \"status\":\"ok\",\n\
                    \"client_id\":\"";
            data.append(std::to_string(descr).c_str());
            data.append("\"\n\
                        }");
        }
    }
    else    if (value.toString() == "message_ok")
    {
        sendMessage(descr, "The test message !!!");
    }
    else    if (value.toString() == "ping")
    {
        data = "{\n\
                \"id\":2,\n\
                \"command\":\"ping_reply\",\n\
                \"status\":\"failed\",\n\
                \"message\":\"Authorization failed.\"\n\
                }";
        if (clients[descr].auth)
        {
            data = "{\n\
                    \"id\":2,\n\
                    \"command\":\"ping_reply\",\n\
                    \"status\":\"ok\"\n\
                    } ";
        }
    }
    else    if (value.toString() == "logout")
    {
        data = "{\n\
                \"id\":2,\n\
                \"command\":\"logout_reply\",\n\
                \"status\":\"ok\"\n\
                }";
    }
    else
    {
        return;
    }
    onWriteData(descr, data);

    if (value.toString() == "logout")
    {
        socketDisconnect(descr);
    }
}

void TcpServer::socketDisconnect(const uint descr)
{
    if (clients.contains(descr))
    {
        clients[descr].sock->close();
        clients[descr].sock->deleteLater();
        auto it = clients.find(descr);
        clients.erase(it);
        std::cout << "Socket with index " << descr << " was closed" << std::endl;
    }
}

void TcpServer::sendMessage(const uint descr, QByteArray message)
{
    QByteArray data("{\n\
               \"id\":2,\n\
               \"command\":\"message\",\n\
               \"body\":\"");
    data.append(message);
    data.append("\",\n\
                \"sender_login\":\"");
    data.append(login);
    data.append("\",\n\
               \"session\":\"");
    data.append(std::to_string(uidSession).c_str());
    data.append("\"\n\
                    }");
    onWriteData(descr, data);
}
