#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QTableWidget>
#include <QMap>
#include <QThread>

class TcpWorker;   // 前置声明
class TcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit TcpServer(QTableWidget *table, QObject *parent = nullptr);
    
signals:
    void debugInfos(const QString &msg);
    void debugErrors(const QString &msg);
    void clientDisconnected(qintptr sd);
    
protected:
    void incomingConnection(qintptr socketDescriptor) override;
    
private:
    void addClientToTable(qintptr sd, const QString &ip, quint16 port);
    void removeClientFromTable(qintptr sd);
    
private:
    QTableWidget *m_table;
};

#endif // TCPSERVER_H
