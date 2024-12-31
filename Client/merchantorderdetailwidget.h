#ifndef MERCHANTORDERDETAILWIDGET_H
#define MERCHANTORDERDETAILWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QPointer>
#include <QList>
#include <QString>
namespace merchant
{
namespace orderDetail
{
struct OrderDishItem:public QWidget
{
    Q_OBJECT
public:
    using ptr = QSharedPointer<OrderDishItem>;
    OrderDishItem()
    {
        QGridLayout* this_layout = new QGridLayout;
        this->setLayout(this_layout);
        QWidget* mainW = new QWidget;
        this_layout->addWidget(mainW);
        mainW->setFixedSize(800,100);
        mainW->setStyleSheet("QWidget{border: 2px solid black;}");
        QGridLayout* layout = new QGridLayout;
        mainW->setLayout(layout);

        QLabel* name = new QLabel("菜品名称");
        QLabel* price = new QLabel("单价:00");
        price->setFixedWidth(200);
        QLabel* cnt = new QLabel("数量：00");
        cnt->setFixedWidth(200);
        QLabel* pay = new QLabel("总价格:00");
        pay->setFixedWidth(200);

        layout->addWidget(name,0,0);
        layout->addWidget(price,1,0);
        layout->addWidget(cnt,1,1);
        layout->addWidget(pay,1,2);
    }
};
}
}

class MerchantOrderDetailWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MerchantOrderDetailWidget(QWidget *parent = nullptr);
    QWidget* mainW;
    void initMainW();
    QString order_id;

    QList<merchant::orderDetail::OrderDishItem::ptr> list;
public slots:
    void acceptOrderSlot();//接单
    void rejectOrderSlot();//拒单
signals:
    void acceptOrderSignal(QString order_id);//接单
    void rejectOrderSignal(QString order_id);//拒单
};

#endif // MERCHANTORDERDETAILWIDGET_H
