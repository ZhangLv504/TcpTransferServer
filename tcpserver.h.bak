#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDateTime>
#include <QDebug>

class TcpServer : public QTcpServer {
    Q_OBJECT
public:
    explicit TcpServer(QObject *parent = nullptr);
    
protected:
    // 当有新客户端连接时触发
    void incomingConnection(qintptr socketDescriptor) override;
    
private slots:
    // 处理客户端发来的数据
    void onReadyRead();
    // 处理客户端断开连接
    void onDisconnected();
    
private:
    // 处理完整的 JSON 消息体
    void processMessage(QTcpSocket *client, const QByteArray &message);
    // 给客户端发送响应 JSON
    void sendResponse(QTcpSocket *client, const QJsonObject &data);
    // 构造完整协议包（头 + JSON 体）
    QByteArray buildPacket(quint8 type, const QJsonObject &data);
    
private:
    // 每个客户端对应一个接收缓冲区，用于粘包/半包处理
    QMap<QTcpSocket*, QByteArray> m_buffers;
};

#endif // TCPSERVER_H
