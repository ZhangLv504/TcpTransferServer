#pragma execution_character_set("utf-8")
#include "tcpserver.h"
#include "protocalstruct.h"
#include "debughelper.h"
#include <QCoreApplication>
#include <QFile>
#include <QtEndian>

// // 全局 map，保存所有正在写入的文件（路径 → 文件对象）
// QMap<QString, QFile*> g_openFiles;
// 全局 map，保存所有正在写入的文件（按客户端区分）
QMap<qintptr, QMap<QString, QFile*>> g_openFiles;

TcpServer::TcpServer(QTableWidget *table, QObject *parent)
    : QTcpServer(parent)
    , m_table(table)
{
    if (m_table) {
        // 初始化表格
        m_table->setColumnCount(3);
        m_table->setHorizontalHeaderLabels({"Socket", "IP", "Port"});
        m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    } else {
        emit debugErrors("客户端列表为空!");
    }
}

void TcpServer::incomingConnection(qintptr socketDescriptor) {
    // 当有新的 TCP 连接进来时，Qt 会调用该函数
    QTcpSocket *client = new QTcpSocket(this);
    if (!client->setSocketDescriptor(socketDescriptor)) {
        emit debugErrors(QString("设置 socket 描述符失败: %1").arg(socketDescriptor));
        client->deleteLater();
        return;
    }
    
    // 保存 socketDescriptor 到 property
    client->setProperty("sd", socketDescriptor);
    
    // 绑定 readyRead 信号，当客户端有数据可读时会触发
    connect(client, &QTcpSocket::readyRead, this, &TcpServer::onReadyRead);
    // 绑定 disconnected 信号，当客户端断开连接时触发
    connect(client, &QTcpSocket::disconnected, this, &TcpServer::onDisconnected);
    
    // 初始化该客户端的缓冲区
    m_buffers[client] = QByteArray();
    
    quint32 ipv4 = client->peerAddress().toIPv4Address();
    QHostAddress addr(ipv4);
    // emit clientConnected(addr.toString(), client->peerPort(), socketDescriptor);
    
    addClientToTable(client);
}

// 接收消息回调
void TcpServer::onReadyRead() {
    // 从 sender() 获取当前发送信号的客户端 socket
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    if (!client) return;
    
    // 获取该客户端对应的缓存 buffer
    QByteArray &buffer = m_buffers[client];
    // 读取 socket 上的所有数据，追加到 buffer
    buffer.append(client->readAll());
    
    // 处理缓存中的数据（可能一次性收到了多个包）
    while (buffer.size() >= sizeof(ProtocolHeader)) {
        // 取出协议头（不移除）
        ProtocolHeader header;
        memcpy(&header, buffer.constData(), sizeof(ProtocolHeader));
        
        // 检查协议头是否合法（魔数、版本等）
        if (!header.isValid()) {
            emit debugErrors("无效的协议头!");
            client->disconnectFromHost();
            return;
        }
        
        // 如果数据还不完整（body 还没收完），跳出等待下次 readyRead
        if (buffer.size() < sizeof(ProtocolHeader) + header.body_len)
            break;
        
        // 提取完整的消息体（JSON 部分）
        QByteArray message = buffer.mid(sizeof(ProtocolHeader), header.body_len);
        // 移除已处理的数据（头 + 体）
        buffer.remove(0, sizeof(ProtocolHeader) + header.body_len);
        // 交给消息处理函数
        processMessage(client, message);
    }
}

void TcpServer::processMessage(QTcpSocket *client, const QByteArray &message) {
    // 将收到的 JSON 数据解析为 QJsonObject
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(message, &error);
    if (error.error != QJsonParseError::NoError) {
        emit debugErrors("JSON 解析失败: " + error.errorString());
        return;
    }
    
    QJsonObject json = doc.object();
    // 读取命令字段
    QString cmd = json["cmd"].toString();
    
    // 如果命令是文件传输
    if (cmd == "file_transfer") {
        QJsonObject data = json["data"].toObject();
        QString fileName = data["file_name"].toString();
        int chunkIndex = data["chunk_index"].toInt();
        QByteArray chunkData = QByteArray::fromBase64(data["chunk_data"].toString().toUtf8());
        
        emit debugInfos(QString("[%1] 接收文件数据：%2, 块索引：%3, 块大小：%4")
                            .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"))
                            .arg(fileName).arg(chunkIndex).arg(chunkData.size()));
        
        // 构造文件保存路径（在当前应用目录下，以 recv_ 前缀保存）
        QString savePath = QString("%1/recv_%2_%3")
                               .arg(QCoreApplication::applicationDirPath() + "/files")
                               .arg(client->socketDescriptor())
                               .arg(fileName);
        qintptr sd = client->socketDescriptor();
        QMap<QString, QFile*> &fileMap = g_openFiles[sd];
        
        emit debugInfos(savePath);

        
        // 查找是否已经打开过该文件
        QFile *file = fileMap.value(savePath, nullptr);
        if (!file) {
            // 如果还没打开，则新建并打开
            file = new QFile(savePath);
            if (!file->open(QIODevice::WriteOnly | QIODevice::Append)) {
                emit debugErrors("打开文件失败: " + savePath);
                return;
            }
            // g_openFiles[savePath] = file;
            fileMap[savePath] = file;
        }
        
        // 将该分片写入文件
        file->write(chunkData);
        // 立即刷新到磁盘，避免缓存丢失
        file->flush();
        
        // 构造响应包，告诉客户端该块接收成功
        QJsonObject response;
        // 回传客户端的请求序号
        response["seq"] = json["seq"].toInt();
        response["timestamp"] = QDateTime::currentSecsSinceEpoch();
        response["cmd"] = "file_transfer_ack";
        response["data"] = QJsonObject(
            {
                { "chunk_index", chunkIndex },
                { "status", 0 }
            }
        );
        response["status"] = 0;
        response["message"] = "success";
        
        sendResponse(client, response);
    }
}


void TcpServer::sendResponse(QTcpSocket *client, const QJsonObject &data) {
    // 将 JSON 数据打包成 协议头 + JSON体
    QByteArray packet = buildPacket(1, data);
    
    // 发送给客户端
    client->write(packet);
    client->flush(); // 强制刷新输出缓冲区，确保立即发送
    
    emit debugInfos(QString("[%1] 已发送响应包给客户端 %2")
                        .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"))
                        .arg(client->socketDescriptor()));
}

// 构建包
QByteArray TcpServer::buildPacket(quint8 type, const QJsonObject &data) {
    // 1) 将 JSON 序列化成字节数组（紧凑格式）
    QJsonDocument doc(data);
    QByteArray body = doc.toJson(QJsonDocument::Compact);
    
    // 2) 填写协议头（注意：需要考虑大小端问题，后续可改进为 qToBigEndian）
    ProtocolHeader header;
    header.type = type;
    header.body_len = static_cast<quint32>(body.size());
    
    // 3) 拼接成完整的数据包（头 + 体）
    QByteArray packet;
    packet.append(reinterpret_cast<const char*>(&header), sizeof(header));
    packet.append(body);
    return packet;
}

// 增加客户端到表格中
void TcpServer::addClientToTable(QTcpSocket *client)
{
    int row = m_table->rowCount();
    m_table->insertRow(row);
    
    m_table->setItem(row, 0, new QTableWidgetItem(QString::number(client->socketDescriptor())));
    m_table->setItem(row, 1, new QTableWidgetItem(client->peerAddress().toString()));
    m_table->setItem(row, 2, new QTableWidgetItem(QString::number(client->peerPort())));
    
    emit debugInfos(QString("客户端连接：%1:%2 (sd=%3)")
                        .arg(client->peerAddress().toString())
                        .arg(client->peerPort())
                        .arg(client->socketDescriptor()));
}

// 从表格中移除客户端
void TcpServer::removeClientFromTable(QTcpSocket *client)
{
    qintptr sd = client->property("sd").toLongLong();
    for (int row = 0; row < m_table->rowCount(); ++row) {
        if (m_table->item(row, 0)->text().toInt() == sd) {
            m_table->removeRow(row);
            break;
        }
    }
    emit debugInfos(QString("客户端断开：sd=%1").arg(sd));
}

void TcpServer::onDisconnected() {
    // 获取断开连接的客户端
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    if (!client) return;
    
    // qintptr sd = client->socketDescriptor();
    qintptr sd = client->property("sd").toLongLong();  // 从 property 获取
    emit clientDisconnected(sd);
    // 移除其缓存 buffer
    m_buffers.remove(client);
    
    removeClientFromTable(client);

    if (g_openFiles.contains(sd)) {
        auto &fileMap = g_openFiles[sd];
        for (auto file : fileMap.values()) {
            if (file) {
                file->close();
                delete file;
            }
        }
        // g_openFiles.clear();
        g_openFiles.remove(sd);
        
        emit debugInfos(QString("[%1] 客户端 %2 的文件已清理")
                            .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"))
                            .arg(sd));
    }
    
    // 释放 socket 对象
    client->deleteLater();
}
