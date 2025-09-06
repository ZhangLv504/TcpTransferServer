#include "widget.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    
    // TcpServer server;
    // QString ip = getPrimaryIP();
    // if (!server.listen(QHostAddress(ip), 12345)) {
    //     qWarning() << "Server failed to start";
    //     return -1;
    // }
    
    // TcpServer server;
    // if (!server.listen(QHostAddress("127.0.0.1"), 12345)) {
    //     qWarning() << "Server failed to start";
    //     return -1;
    // }
    
    // qDebug() << "Server started on port 12345";
    
    Widget w;
    w.show();
    
    return a.exec();
}
