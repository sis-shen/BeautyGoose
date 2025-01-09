#ifndef CONSUMERORDERDETAILWIDGET_H
#define CONSUMERORDERDETAILWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QPointer>
#include <QList>
#include <QString>
#include "Nav.h"
#include "DataCenterCoreData.h"
namespace consumer
{
namespace orderDetail
{
struct OrderDishItem:public QWidget
{
    Q_OBJECT
public:
    using ptr = QSharedPointer<OrderDishItem>;
    OrderDishItem(const btyGoose::data::OrderDish& dish)
    {
        QGridLayout* this_layout = new QGridLayout;
        this->setLayout(this_layout);
        QWidget* mainW = new QWidget;
        this_layout->addWidget(mainW);
        mainW->setFixedSize(800,100);
        mainW->setStyleSheet("QWidget{border: 2px solid black;}");
        QGridLayout* layout = new QGridLayout;
        mainW->setLayout(layout);

        QLabel* name = new QLabel(dish.name);
        QLabel* price = new QLabel("单价:" + QString::number(dish.dish_price)+"元");
        price->setFixedWidth(200);
        QLabel* cnt = new QLabel("数量：" + QString::number(dish.count));
        cnt->setFixedWidth(200);
        QLabel* pay = new QLabel("总价格:"+QString::number(dish.dish_price*dish.count)+"元");
        pay->setFixedWidth(200);

        layout->addWidget(name,0,0);
        layout->addWidget(price,1,0);
        layout->addWidget(cnt,1,1);
        layout->addWidget(pay,1,2);
    }
};
}
}
class ConsumerOrderDetailWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ConsumerOrderDetailWidget(const btyGoose::data::Order& order,const QList<btyGoose::data::OrderDish>*dish_list);

    ConsumerNavWidget* leftNavW;
    QWidget* rightW;
    btyGoose::data::Order order;
    void initRight();
    QList<consumer::orderDetail::OrderDishItem::ptr> list;

    QString order_id;
public slots:
    void payOrderSlot();
    void toOrderListSLot();
    void cancelSlot();
signals:
    void payOrderSignal(QString order_id);
    void toOrderListSignal();
    void cancelSignal(QString order_id);
};

#endif // CONSUMERORDERDETAILWIDGET_H
