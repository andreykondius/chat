#include "tcpServer/TcpServer.h"
#include <QApplication>

int main(int argc, char* argv[])
{    
    QApplication a(argc, argv);
    QString login( argv[1] );
    QString pass( argv[2] );
    TcpServer serv(login, pass);
    serv.Start();
    return a.exec();
}
