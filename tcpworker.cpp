#pragma execution_character_set("utf-8")
#include "tcpworker.h"
#include <QJsonDocument>
#include <QCoreApplication>
#include <QDateTime>
#include <QtEndian>
#include <QHostAddress>
#include <QThread>

TcpWorker::TcpWorker(qintptr socketDescriptor, QObject *parent)
    : QObject(parent)
    , m_client(nullptr)
    , m_sd(socketDescriptor)
{}

TcpWorker::~TcpWorker()
{
    for (auto file : m_fileMap.values()) {
        if (file) {
            file->close();
            delete file;
        }
    }
    m_fileMap.clear();
}

void TcpWorker::start()
{
    m_client = new QTcpSocket();
    if (!m_client->setSocketDescriptor(m_sd)) {
        emit debugErrors(QString("设置 socket 描述符失败: %1").arg(m_sd));
        emit finished(m_sd);
        return;
    }
    
    connect(m_client, &QTcpSocket::readyRead, this, &TcpWorker::onReadyRead);
    connect(m_client, &QTcpSocket::disconnected, this, &TcpWorker::onDisconnected);
    
    emit clientConnected(m_sd, m_client->peerAddress().toString(), m_client->peerPort());
    
    emit debugInfos(QString("客户端接入: sd=%1, ip=%2, port=%3")
                        .arg(m_sd)
                        .arg(m_client->peerAddress().toString())
                        .arg(m_client->peerPort()));
    
    debugInfos(QString("TcpWorker当前线程ID为：%1")
                   .arg(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));
    
}

void TcpWorker::onReadyRead()
{
    m_buffer.append(m_client->readAll());
    
    while (m_buffer.size() >= static_cast<int>(sizeof(ProtocolHeader))) {
        ProtocolHeader header;
        memcpy(&header, m_buffer.constData(), sizeof(ProtocolHeader));
        
        if (!header.isValid()) {
            emit debugErrors("无效协议头");
            m_client->disconnectFromHost();
            return;
        }
        
        if (m_buffer.size() < static_cast<int>(sizeof(ProtocolHeader) + header.body_len))
            break;
        
        QByteArray message = m_buffer.mid(sizeof(ProtocolHeader), header.body_len);
        m_buffer.remove(0, sizeof(ProtocolHeader) + header.body_len);
        
        processMessage(message);
    }
}

void TcpWorker::onDisconnected()
{
    emit debugInfos(QString("客户端断开: sd=%1").arg(m_sd));
    emit finished(m_sd);
    QThread* thread = this->thread();  // 获取所属线程
    thread->quit();
    m_client->deleteLater();
}

void TcpWorker::processMessage(const QByteArray &message)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(message, &error);
    if (error.error != QJsonParseError::NoError) {
        emit debugErrors("JSON 解析失败: " + error.errorString());
        return;
    }
    
    QJsonObject json = doc.object();
    QString cmd = json["cmd"].toString();
    int status = json["status"].toInt(0);
    
    if (cmd == "file_transfer") {
        if(status == 0)
        {
            QJsonObject data = json["data"].toObject();
            QString fileName = data["file_name"].toString();
            int chunkIndex = data["chunk_index"].toInt();
            QByteArray chunkData = QByteArray::fromBase64(data["chunk_data"].toString().toUtf8());
            
            QString savePath = QString("%1/recv_%2_%3")
                                   .arg(QCoreApplication::applicationDirPath() + "/files")
                                   .arg(m_sd)
                                   .arg(fileName);
            
            QFile *file = m_fileMap.value(savePath, nullptr);
            if (!file) {
                file = new QFile(savePath);
                if (!file->open(QIODevice::WriteOnly | QIODevice::Append)) {
                    emit debugErrors("打开文件失败: " + savePath);
                    return;
                }
                m_fileMap[savePath] = file;
            }
            
            file->write(chunkData);
            file->flush();
            
            QJsonObject response;
            response["seq"] = json["seq"].toInt();
            response["timestamp"] = QDateTime::currentSecsSinceEpoch();
            response["cmd"] = "file_transfer_ack";
            response["data"] = QJsonObject{
                {"chunk_index", chunkIndex},
                {"status", 0}
            };
            response["status"] = 0;
            response["message"] = "success";
            
            sendResponse(response);
            
            emit debugInfos(QString("客户端 %1 收到文件: %2, 块=%3, 大小=%4")
                                .arg(m_sd).arg(fileName).arg(chunkIndex).arg(chunkData.size()));
        } else {
            emit debugErrors("未知status: " + QString::number(status));
        }
    } else {
        emit debugErrors("未知命令: " + cmd);
    }   
}

void TcpWorker::sendResponse(const QJsonObject &data)
{
    QByteArray packet = buildPacket(1, data);
    m_client->write(packet);
    m_client->flush();
}

QByteArray TcpWorker::buildPacket(quint8 type, const QJsonObject &data)
{
    QJsonDocument doc(data);
    QByteArray body = doc.toJson(QJsonDocument::Compact);
    
    ProtocolHeader header;
    header.type = type;
    header.body_len = static_cast<quint32>(body.size());
    
    QByteArray packet;
    packet.append(reinterpret_cast<const char*>(&header), sizeof(header));
    packet.append(body);
    return packet;
}
