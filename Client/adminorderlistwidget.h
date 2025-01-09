#ifndef ADMINORDERLISTWIDGET_H
#define ADMINORDERLISTWIDGET_H

#include <QWidget>
#include <QIcon>
#include <QGridLayout>
#include <QLabel>
#include <QString>
#include <QPointer>
#include <QPushButton>
#include <QList>
#include "CoreData.h"
namespace admin
{
namespace orderList
{
struct OrderItem:public QWidget
{
    Q_OBJECT
public:
    using ptr = QSharedPointer<OrderItem>;
    OrderItem(const btyGoose::data::Order&order)
    {
        QWidget* mainW = new QWidget;
        QGridLayout*mLaout = new QGridLayout;
        this->setLayout(mLaout);
        mLaout->addWidget(mainW);

        mainW->setStyleSheet("QWidget { border: 2px solid black; }");

        QGridLayout* layout = new QGridLayout;
        mainW->setLayout(layout);
        layout->setContentsMargins(0,0,50,0);

        QLabel* order_id = new QLabel("订单id:"+order.uuid);
        QLabel* merchant_name = new QLabel("商家："+order.merchant_name);
        QLabel*  consumer_name = new QLabel("用户:"+order.consumer_name);
        QLabel* time = new QLabel("时间:"+btyGoose::util::formatTime(order.time.toInt()));
        QLabel* cnt = new QLabel("菜品数量:"+QString::number(order.sum));
        QLabel* pay = new QLabel("总价:" + QString::number(order.pay)+"元");;

        layout->addWidget(order_id,0,0);
        layout->addWidget(merchant_name,1,0);
        layout->addWidget(consumer_name,2,0);
        layout->addWidget(time,3,0);
        layout->addWidget(cnt,4,0);
        layout->addWidget(pay,5,0);
        QString stat;
        if(order.status == btyGoose::data::Order::Status::UNPAYED)
            stat = "待支付";
        else if(order.status == btyGoose::data::Order::Status::WAITING)
            stat = "等待商家";
        else if(order.status == btyGoose::data::Order::Status::ERR)
            stat = "错误";
        else if(order.status == btyGoose::data::Order::Status::FATAL)
            stat = "失败";
        else if(order.status == btyGoose::data::Order::Status::OVER_TIME)
            stat = "超时";
        else if(order.status == btyGoose::data::Order::Status::SUCCESS)
            stat = "成功完成";
        else if(order.status == btyGoose::data::Order::Status::REJECTED)
            stat = "被拒单";
        else
        {
            qDebug()<<"stat:"<<order.status;
            Q_ASSERT(false);
        }

        QPushButton* statBtn = new QPushButton("状态:"+stat);
        statBtn->setFixedSize(100,50);


        layout->addWidget(statBtn,0,1,6,1);

    }
};
}
}

class AdminOrderListWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AdminOrderListWidget(const QList<btyGoose::data::Order>* order_list);

    QWidget* rightW;

    QList<admin::orderList::OrderItem::ptr>list;
    void initRight();

signals:
};

#endif // ADMINORDERLISTWIDGET_H
