/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>
#include <ipaddressui.h>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QGridLayout *gridLayout;
    QTextEdit *infosTextE;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    IpAddressUI *ipaddrUI;
    QLabel *label_2;
    QLineEdit *portLE;
    QSpacerItem *horizontalSpacer;
    QPushButton *listenBtn;
    QTableWidget *clientsTableW;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName(QString::fromUtf8("Widget"));
        Widget->resize(787, 574);
        gridLayout = new QGridLayout(Widget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        infosTextE = new QTextEdit(Widget);
        infosTextE->setObjectName(QString::fromUtf8("infosTextE"));
        QFont font;
        font.setPointSize(12);
        infosTextE->setFont(font);

        gridLayout->addWidget(infosTextE, 1, 2, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(Widget);
        label->setObjectName(QString::fromUtf8("label"));
        QFont font1;
        font1.setPointSize(14);
        font1.setBold(true);
        label->setFont(font1);
        label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout->addWidget(label);

        ipaddrUI = new IpAddressUI(Widget);
        ipaddrUI->setObjectName(QString::fromUtf8("ipaddrUI"));
        ipaddrUI->setMinimumSize(QSize(0, 35));

        horizontalLayout->addWidget(ipaddrUI);

        label_2 = new QLabel(Widget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setFont(font1);
        label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout->addWidget(label_2);

        portLE = new QLineEdit(Widget);
        portLE->setObjectName(QString::fromUtf8("portLE"));
        portLE->setMinimumSize(QSize(0, 35));
        QFont font2;
        font2.setPointSize(15);
        portLE->setFont(font2);

        horizontalLayout->addWidget(portLE);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        listenBtn = new QPushButton(Widget);
        listenBtn->setObjectName(QString::fromUtf8("listenBtn"));
        listenBtn->setMinimumSize(QSize(0, 35));
        QFont font3;
        font3.setPointSize(10);
        listenBtn->setFont(font3);

        horizontalLayout->addWidget(listenBtn);


        gridLayout->addLayout(horizontalLayout, 0, 0, 1, 3);

        clientsTableW = new QTableWidget(Widget);
        clientsTableW->setObjectName(QString::fromUtf8("clientsTableW"));

        gridLayout->addWidget(clientsTableW, 1, 1, 1, 1);


        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QCoreApplication::translate("Widget", "Widget", nullptr));
        label->setText(QCoreApplication::translate("Widget", "IP:", nullptr));
        label_2->setText(QCoreApplication::translate("Widget", "Port\357\274\232", nullptr));
        portLE->setText(QCoreApplication::translate("Widget", "12345", nullptr));
        listenBtn->setText(QCoreApplication::translate("Widget", "\345\274\200\345\247\213\347\233\221\345\220\254", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
