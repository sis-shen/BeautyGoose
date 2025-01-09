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
        QWidget* mainW = new QWidget;
        QGridLayout*mLaout = new QGridLayout;
        this->setLayout(mLaout);
        mLaout->addWidget(mainW);

        mainW->setStyleSheet("QWidget { border: 2px solid black; }");

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

        QPushButton* stat = new QPushButton("状态:待处理");
        QPushButton* process = new QPushButton("处理");
        process->setStyleSheet("QPushButton { margin-right: 20px; }");

        stat->setFixedSize(100,50);
        process->setFixedSize(100,50);

        layout->addWidget(stat,0,1,4,1);
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
