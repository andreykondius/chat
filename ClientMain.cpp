#include "tcpClient/TcpClient.h"
#include <QApplication>

int main(int argc, char* argv[])
{    
    QApplication a(argc, argv);
    QString login( argv[1] );
    QString pass( argv[2] );
    TcpClient  sock(login, pass);
    return a.exec();
}
