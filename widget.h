#ifndef WIDGET_H
#define WIDGET_H

#include "tcpserver.h"
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    
public slots:
    // 处理监听
    void handleListenBtnSlot();
    
private:
    // 获取本机IP地址
    QString getPrimaryIP();
    // 更新连接按钮状态
    void updateConnectBtnState();

private:
    Ui::Widget *ui;
    
    TcpServer *m_server; // 服务端对象
};
#endif // WIDGET_H
