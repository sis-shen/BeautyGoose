#ifndef MERCHANTORDERLISTWIDGET_H
#define MERCHANTORDERLISTWIDGET_H

#include <QWidget>
#include <QIcon>
#include <QGridLayout>
#include "Nav.h"
#include <QLabel>
#include <QString>
#include <QPointer>
#include <QPushButton>
#include <QList>
#include "CoreData.h"
#include "ColorConfig.h"
namespace merchant
{
namespace orderList
{
struct OrderItem:public QWidget
{
    Q_OBJECT
public:
    using ptr = QSharedPointer<OrderItem>;
    QString order_id = "order_id";
    OrderItem(const btyGoose::data::Order& order)
        :order_id(order.uuid)
    {
        auto color = ColorConfig::getInstance();
        QWidget* mainW = new QWidget;
        QGridLayout*mLaout = new QGridLayout;
        this->setLayout(mLaout);
        mLaout->addWidget(mainW);
        QString btnStyle = QString("QPushButton{border-radius: 15px;"
                                   " border: 2px solid %1;"
                                   "background-color: %1;}").arg(color->main_color);
        mainW->setStyleSheet("QWidget { border: 0px solid black;");
        mainW->setStyleSheet(btnStyle);

        QGridLayout* layout = new QGridLayout;
        mainW->setLayout(layout);
        layout->setContentsMargins(0,0,15,0);

        QLabel*  consumer_name = new QLabel(order.consumer_name);
        QLabel* time = new QLabel("时间:"+btyGoose::util::formatTime(order.time.toInt()));
        QLabel* cnt = new QLabel("菜品数量:"+QString::number(order.sum));
        QLabel* pay = new QLabel("总价:"+QString::number(order.pay));

        layout->addWidget(consumer_name,0,0);
        layout->addWidget(time,1,0);
        layout->addWidget(cnt,2,0);
        layout->addWidget(pay,3,0);

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
        else if(order.status == btyGoose::data::Order::Status::CANCELED)
            stat = "已取消";
        else
        {
            qDebug()<<"stat:"<<order.status;
            Q_ASSERT(false);
        }

        QPushButton* statBtn = new QPushButton("状态:"+stat);
        QPushButton* process = new QPushButton("处理");
        process->setStyleSheet("QPushButton { margin-right: 20px; }");

        statBtn->setFixedSize(100,50);
        process->setFixedSize(100,50);

        layout->addWidget(statBtn,0,1,4,1);
        layout->addWidget(process,0,2,4,1);
        connect(process,&QPushButton::clicked,this,&OrderItem::orderInfoSlot);
    }
public slots:
    void orderInfoSlot(){emit orderInfoSignal(order_id);}
signals:
    void orderInfoSignal(QString dish_id);
};
}
}
class MerchantOrderListWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MerchantOrderListWidget(const QHash<QString,btyGoose::data::Order>* order_table);
    MerchantNavWidget* leftNavW;
    QWidget* rightW;

    QList<merchant::orderList::OrderItem::ptr>list;
    void initRight();
public slots:
    void merchantOrderInfoSlot(QString order_id);
signals:
    void merchantOrderInfoSignal(QString order_id);
};

#endif // MERCHANTORDERLISTWIDGET_H
