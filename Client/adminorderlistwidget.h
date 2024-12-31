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

namespace admin
{
namespace orderList
{
struct OrderItem:public QWidget
{
    Q_OBJECT
public:
    using ptr = QSharedPointer<OrderItem>;
    OrderItem()
    {
        QWidget* mainW = new QWidget;
        QGridLayout*mLaout = new QGridLayout;
        this->setLayout(mLaout);
        mLaout->addWidget(mainW);

        mainW->setStyleSheet("QWidget { border: 2px solid black; }");

        QGridLayout* layout = new QGridLayout;
        mainW->setLayout(layout);
        layout->setContentsMargins(0,0,50,0);

        QLabel* order_id = new QLabel("订单id:000000000011111");
        QLabel* merchant_name = new QLabel("商家：苟布里包子铺");
        QLabel*  consumer_name = new QLabel("用户:你好我是李华");
        QLabel* time = new QLabel("时间:2024/12/31 12:00");
        QLabel* cnt = new QLabel("菜品数量:00");
        QLabel* pay = new QLabel("总价:00");;

        layout->addWidget(order_id,0,0);
        layout->addWidget(merchant_name,1,0);
        layout->addWidget(consumer_name,2,0);
        layout->addWidget(time,3,0);
        layout->addWidget(cnt,4,0);
        layout->addWidget(pay,5,0);

        QPushButton* stat = new QPushButton("状态:待处理");
        stat->setFixedSize(100,50);


        layout->addWidget(stat,0,1,6,1);

    }
};
}
}

class AdminOrderListWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AdminOrderListWidget(QWidget *parent = nullptr);

    QWidget* rightW;

    QList<admin::orderList::OrderItem::ptr>list;
    void initRight();

signals:
};

#endif // ADMINORDERLISTWIDGET_H
