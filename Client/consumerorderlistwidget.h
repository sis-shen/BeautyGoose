#ifndef CONSUMERORDERLISTWIDGET_H
#define CONSUMERORDERLISTWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QPointer>
#include <QList>
#include <QString>
#include "Nav.h"
#include "CoreData.h"
#include "DataCenterCoreData.h"
#include "ColorConfig.h"
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
    OrderItem(const btyGoose::data::Order& order)
        :order_id(order.uuid)
    {
        auto color = ColorConfig::getInstance();
        this->setFixedSize(1000,200);
        QGridLayout* mainLayout = new QGridLayout;
        this->setLayout(mainLayout);

        QWidget* mainW = new QWidget;
        mainW->setObjectName("mainW");
        mainW->setStyleSheet("QWidget{border:0px;}");
        mainW->setStyleSheet(QString("QWidget#mainW{border: 2px solid %4;background-color:%1;}"
                                    "QWidget:hover{background-color: %2}"
                                    "QWidget:checked{background-color: %3}").arg(color->background_color,color->hover_color,color->selected_color,color->main_color_light));
        mainLayout->addWidget(mainW);
        QGridLayout* layout = new QGridLayout;
        mainW->setLayout(layout);
        QWidget* leftW = new QWidget;
        // leftW->setStyleSheet(QString("QWidget{background-color:%1}").arg(color->main_color));
        QWidget* rightW = new QWidget;
        rightW->setStyleSheet("QWidget{border:0px}");
        layout->addWidget(leftW,0,0);
        layout->addWidget(rightW,0,1);
        layout->setAlignment(leftW,Qt::AlignLeft);
        layout->setAlignment(rightW,Qt::AlignRight);

        //初始化左边
        QLabel* nickname = new QLabel(order.merchant_name);
        qint64 timestamp = (order.time).toLongLong();
        QLabel* time = new QLabel(btyGoose::util::formatTime(timestamp));
        QLabel* sum = new QLabel("菜品总数:" + QString::number(order.sum));
        QLabel* pay = new QLabel("订单价格:"+QString::number(order.pay)+"元");

        QGridLayout* lLayout = new QGridLayout;
        lLayout->addWidget(nickname,0,0);
        lLayout->addWidget(time,1,0);
        lLayout->addWidget(sum,2,0);
        lLayout->addWidget(pay,3,0);

        leftW->setLayout(lLayout);

        //初始化右边
        rightW->setFixedWidth(250);
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
        QString btnStyle = QString("QPushButton{border-radius: 15px;"
                                   " border: 2px solid %1;"
                                   "background-color: %1;}").arg(color->main_color);
        QPushButton* statBtn = new QPushButton(stat);
        QPushButton* infoBtn = new QPushButton("查看");

        statBtn->setStyleSheet(btnStyle);
        infoBtn->setStyleSheet(btnStyle);
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
    explicit ConsumerOrderListWidget(const QHash<QString,btyGoose::data::Order>* order_table);

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
