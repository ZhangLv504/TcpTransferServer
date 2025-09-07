#ifndef TCPWORKER_H
#define TCPWORKER_H

#include <QObject>
#include <QTcpSocket>
#include <QFile>
#include <QMap>
#include <QJsonObject>
#include "protocalstruct.h"

// 专门处理一个客户端的 Worker（运行在独立线程中）
class TcpWorker : public QObject
{
    Q_OBJECT
public:
    explicit TcpWorker(qintptr socketDescriptor, QObject *parent = nullptr);
    ~TcpWorker();
    
public slots:
    void start();   // 在线程开始后调用
    void onReadyRead();
    void onDisconnected();

signals:
    void debugInfos(const QString &msg);
    void debugErrors(const QString &msg);
    void finished(qintptr sd);
    void clientConnected(qintptr sd, const QString &ip, quint16 port);
    
private:
    void processMessage(const QByteArray &message);
    void sendResponse(const QJsonObject &data);
    QByteArray buildPacket(quint8 type, const QJsonObject &data);
    
private:
    QTcpSocket *m_client;
    qintptr m_sd;
    QByteArray m_buffer;
    
    // 文件 map (每个客户端独立维护)
    QMap<QString, QFile*> m_fileMap;
};

#endif // TCPWORKER_H
