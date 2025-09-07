#pragma execution_character_set("utf-8")
#include "tcpserver.h"
#include "tcpworker.h"  
#include <QHostAddress>
#include <QDateTime>

TcpServer::TcpServer(QTableWidget *table, QObject *parent)
    : QTcpServer{parent}
    , m_table(table)
{
    if (m_table) {
        m_table->setColumnCount(3);
        m_table->setHorizontalHeaderLabels({"Socket", "IP", "Port"});
        m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    } else {
        emit debugErrors("客户端列表为空!");
    }
}

void TcpServer::incomingConnection(qintptr socketDescriptor)
{
    QThread *thread = new QThread;
    TcpWorker *worker = new TcpWorker(socketDescriptor);
    
    worker->moveToThread(thread);
    
    connect(thread, &QThread::started, worker, &TcpWorker::start);
    connect(worker, &TcpWorker::finished, this, [=](qintptr sd){
        emit debugInfos(QString("线程退出: sd=%1").arg(sd));
        removeClientFromTable(sd);
        thread->quit();
    }, Qt::QueuedConnection);
    
    connect(thread, &QThread::finished, worker, &QObject::deleteLater);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    connect(worker, &TcpWorker::debugInfos, this, &TcpServer::debugInfos);
    connect(worker, &TcpWorker::debugErrors, this, &TcpServer::debugErrors);
    connect(worker, &TcpWorker::clientConnected,
            this, &TcpServer::addClientToTable, Qt::QueuedConnection);
    
    // // 当 worker 启动成功时，添加到表格
    // connect(worker, &TcpWorker::debugInfos, this, [=](const QString &msg){
    //     if (msg.startsWith("客户端接入:")) {
    //         QHostAddress addr;
    //         quint16 port = 0;
    //         // 解析 socket 信息（你可以在 TcpWorker::start 中 emit 更清晰的参数）
    //         // 这里为了简单，直接再获取一次
    //         QTcpSocket tmp;
    //         tmp.setSocketDescriptor(socketDescriptor);
    //         addClientToTable(socketDescriptor, tmp.peerAddress().toString(), tmp.peerPort());
    //     }
    // });
    
    debugInfos(QString("TcpServer当前线程ID为：%1")
                    .arg(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));
    
    thread->start();
}

void TcpServer::addClientToTable(qintptr sd, const QString &ip, quint16 port)
{
    if (!m_table) return;
    
    int row = m_table->rowCount();
    m_table->insertRow(row);
    
    m_table->setItem(row, 0, new QTableWidgetItem(QString::number(sd)));
    m_table->setItem(row, 1, new QTableWidgetItem(ip));
    m_table->setItem(row, 2, new QTableWidgetItem(QString::number(port)));
    
    emit debugInfos(QString("客户端连接：%1:%2 (sd=%3)").arg(ip).arg(port).arg(sd));
}

void TcpServer::removeClientFromTable(qintptr sd)
{
    if (!m_table) return;
    
    for (int row = 0; row < m_table->rowCount(); ++row) {
        if (m_table->item(row, 0)->text().toLongLong() == sd) {
            m_table->removeRow(row);
            break;
        }
    }
    emit debugInfos(QString("客户端断开：sd=%1").arg(sd));
}
