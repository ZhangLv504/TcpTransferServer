#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDateTime>
#include <QDebug>
#include <QTableWidget>

class TcpServer : public QTcpServer {
    Q_OBJECT
public:
    explicit TcpServer(QTableWidget *table, QObject *parent = nullptr);
    
protected:
    // 当有新客户端连接时触发
    void incomingConnection(qintptr socketDescriptor) override;
    
private slots:
    // 处理客户端发来的数据
    void onReadyRead();
    // 处理客户端断开连接
    void onDisconnected();
    
signals:
    // 客户端连接信号
    void clientConnected(const QString &addr, quint16 port, qintptr socketDescriptor);
    // 客户端断开连接
    void clientDisconnected(qintptr socketDescriptor);
    // 各种调试信息
    void debugInfos(const QString &info);
    // 各种错误信息
    void debugErrors(const QString &info);
    
private:
    // 处理完整的 JSON 消息体
    void processMessage(QTcpSocket *client, const QByteArray &message);
    // 给客户端发送响应 JSON
    void sendResponse(QTcpSocket *client, const QJsonObject &data);
    // 构造完整协议包（头 + JSON 体）
    QByteArray buildPacket(quint8 type, const QJsonObject &data);
    // 增加客户端到表格中
    void addClientToTable(QTcpSocket *client);
    // 从表格中移除客户端
    void removeClientFromTable(QTcpSocket *client);
    
private:
    // 每个客户端对应一个接收缓冲区，用于粘包/半包处理
    QMap<QTcpSocket*, QByteArray> m_buffers;
    // 外部传进来的 QTableWidget，用来显示客户端
    QTableWidget *m_table; 
};

#endif // TCPSERVER_H
