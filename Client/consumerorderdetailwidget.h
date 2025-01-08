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
    OrderDishItem(const btyGoose::CartDishItem&dish_item)
    {
        QGridLayout* this_layout = new QGridLayout;
        this->setLayout(this_layout);
        QWidget* mainW = new QWidget;
        this_layout->addWidget(mainW);
        mainW->setFixedSize(800,100);
        mainW->setStyleSheet("QWidget{border: 2px solid black;}");
        QGridLayout* layout = new QGridLayout;
        mainW->setLayout(layout);

        QLabel* name = new QLabel(dish_item.dish_name);
        QLabel* price = new QLabel("单价:" + QString::number(dish_item.dish_price));
        price->setFixedWidth(200);
        QLabel* cnt = new QLabel("数量：" + QString::number(dish_item.cnt));
        cnt->setFixedWidth(200);
        QLabel* pay = new QLabel("总价格:"+QString::number(dish_item.pay));
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
    explicit ConsumerOrderDetailWidget(const btyGoose::ConsumerOrderItem* order_item);

    ConsumerNavWidget* leftNavW;
    QWidget* rightW;
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
