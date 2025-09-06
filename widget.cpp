#pragma execution_character_set("utf-8")
#include "widget.h"
#include "./ui_widget.h"

#include "protocalstruct.h"
#include "debughelper.h"
#include <QDebug>
#include <QHostAddress>
#include <QNetworkInterface>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    
    // 初始化服务器
    m_server = new TcpServer(ui->clientsTableW, this);
    
    // 默认 IP 和端口
    ui->ipaddrUI->setIP("127.0.0.1");
    ui->portLE->setText("12345");
    
    // 处理监听
    connect(ui->listenBtn, &QPushButton::clicked, this, &Widget::handleListenBtnSlot);
    
    // 处理 IP 和 Port 输入变化
    connect(ui->ipaddrUI, &IpAddressUI::ipTextChanged, this, &Widget::updateConnectBtnState);
    connect(ui->portLE, &QLineEdit::textChanged, this, &Widget::updateConnectBtnState);
    
    // 处理调试信息
    connect(m_server, &TcpServer::debugInfos, this, [this](const QString &info){
        ui->infosTextE->append(info);
    });
    // 处理错误信息
    connect(m_server, &TcpServer::debugErrors, this, [this](const QString &err){
        ui->infosTextE->append(err);
    });
    
    // // 处理客户端连接状态
    // connect(m_server, &TcpServer::clientConnected, this,
    //         [this](const QString &addr, quint16 port, qintptr socketDesc) {
    //             int row = ui->clientsTableW->rowCount();
    //             ui->clientsTableW->insertRow(row);
                
    //             ui->clientsTableW->setItem(row, 0, new QTableWidgetItem(QString::number(socketDesc)));
    //             ui->clientsTableW->setItem(row, 1, new QTableWidgetItem(addr));
    //             ui->clientsTableW->setItem(row, 2, new QTableWidgetItem(QString::number(port)));
                
    //             ui->infosTextE->append(LogWithTime(
    //                 QString("客户端连接：%1:%2 (sd=%3)").arg(addr).arg(port).arg(socketDesc)));
    //         });
    // // 处理客户端断开连接状态
    // connect(m_server, &TcpServer::clientDisconnected, this,
    //         [this](qintptr socketDesc) {
    //             for (int row = 0; row < ui->clientsTableW->rowCount(); ++row) {
    //                 if (ui->clientsTableW->item(row, 0)->text().toInt() == socketDesc) {
    //                     ui->clientsTableW->removeRow(row);
    //                     break;
    //                 }
    //             }
    //             ui->infosTextE->append(LogWithTime(
    //                 QString("客户端断开：sd=%1").arg(socketDesc)));
    //         });
    
    ui->infosTextE->append(LogWithTime("程序启动完成"));
}

Widget::~Widget()
{
    delete ui;
}

// 处理监听
void Widget::handleListenBtnSlot()
{
    QString ip = ui->ipaddrUI->getIP();
    quint16 port = ui->portLE->text().toUShort();
    
    if (!m_server->isListening()) {
        if (!m_server->listen(QHostAddress(ip), port)) {
            ui->infosTextE->append(LogWithTimeError(
                QString("服务器启动失败：%1").arg(m_server->errorString())));
            return;
        }
        ui->infosTextE->append(LogWithTime(
            QString("服务器启动成功，监听 %1:%2").arg(ip).arg(port)));
        ui->listenBtn->setText("停止监听");
    } else {
        m_server->close();
        ui->infosTextE->append(LogWithTime("服务器已停止监听"));
        ui->listenBtn->setText("开始监听");
    }
}

// 获取本机IP地址
QString Widget::getPrimaryIP()
{
    const QList<QHostAddress>& allIPs = QNetworkInterface::allAddresses();
    for (const QHostAddress& ip : allIPs)
    {
        if (ip.protocol() == QAbstractSocket::IPv4Protocol
            && !ip.isLoopback())
        {
            return ip.toString();
        }
    }
    return QString();
}

// 更新连接按钮状态
void Widget::updateConnectBtnState()
{
    QString ip = ui->ipaddrUI->getIP().trimmed();
    QString port = ui->portLE->text().trimmed();
    
    // 判断是否都有值
    bool enable = !ip.isEmpty() && !port.isEmpty();
    ui->listenBtn->setEnabled(enable);
}
