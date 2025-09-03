#include "tcpserver.h"
#include "protocalstruct.h"
#include <QCoreApplication>
#include <QDebug>

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    
    TcpServer server;
    if (!server.listen(QHostAddress::Any, 12345)) {
        qWarning() << "Server failed to start";
        return -1;
    }
    
    qDebug() << "Server started on port 12345";
    
    return a.exec();
}
