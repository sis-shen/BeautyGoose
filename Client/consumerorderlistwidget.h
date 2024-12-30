#ifndef CONSUMERORDERLISTWIDGET_H
#define CONSUMERORDERLISTWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QPointer>
#include <QList>
#include <QString>
#include "Nav.h"
namespace consumer
{
namespace orderList
{
struct OrderItem:public QWidget
{
    Q_OBJECT
public:
    using ptr = QSharedPointer<OrderItem>;
    QString order_id = "order_id";
    OrderItem()
    {
        this->setFixedSize(1000,200);
        this->setStyleSheet("QWidget{border: 2px solid black;}");
        QGridLayout* mainLayout = new QGridLayout;
        this->setLayout(mainLayout);

        QWidget* mainW = new QWidget;
        mainLayout->addWidget(mainW);
        QGridLayout* layout = new QGridLayout;
        mainW->setLayout(layout);
        QWidget* leftW = new QWidget;
        QWidget* rightW = new QWidget;
        layout->addWidget(leftW,0,0);
        layout->addWidget(rightW,0,1);
        layout->setAlignment(leftW,Qt::AlignLeft);
        layout->setAlignment(rightW,Qt::AlignRight);

        //初始化左边
        QLabel* nickname = new QLabel("商家:苟布里包子店");
        QLabel* time = new QLabel("2024/12/31 12:00");
        QLabel* sum = new QLabel("菜品总数:99");
        QLabel* pay = new QLabel("订单价格:99元");

        QGridLayout* lLayout = new QGridLayout;
        lLayout->addWidget(nickname,0,0);
        lLayout->addWidget(time,1,0);
        lLayout->addWidget(sum,2,0);
        lLayout->addWidget(pay,3,0);

        leftW->setLayout(lLayout);

        //初始化右边
        rightW->setFixedWidth(250);
        QPushButton* statBtn = new QPushButton("状态：已完成");
        QPushButton* infoBtn = new QPushButton("查看");
        statBtn->setFixedSize(100,50);
        infoBtn->setFixedSize(100,50);
        QGridLayout* rLayout = new QGridLayout;
        rightW->setLayout(rLayout);
        rLayout->addWidget(statBtn,0,0);
        rLayout->addWidget(infoBtn,0,1);
        connect(infoBtn,&QPushButton::clicked,this,&OrderItem::toOrderInfoSlot);
    }
public slots:
    void toOrderInfoSlot()
    {
        emit toOrderInfoSignal(order_id);
    }
signals:
    void toOrderInfoSignal(QString order_id);
};
}
}
class ConsumerOrderListWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ConsumerOrderListWidget(QWidget *parent = nullptr);

    ConsumerNavWidget* leftNavW;
    QWidget* rightW;
    void initRight();
    QList<consumer::orderList::OrderItem::ptr> list;

public slots:
    void toOrderInfoSlot(QString order_id);
signals:
    void toOrderInfoSignal(QString order_id);
};


#endif // CONSUMERORDERLISTWIDGET_H
