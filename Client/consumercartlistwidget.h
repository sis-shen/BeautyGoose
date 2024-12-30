#ifndef CONSUMERCARTLISTWIDGET_H
#define CONSUMERCARTLISTWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QScrollBar>
#include <QPushButton>
#include <QLabel>
#include <QString>
#include <QSpacerItem>
#include <QList>
#include <QPointer>
#include <QScrollArea>
#include "Nav.h"

namespace cartList
{
struct DishItem:public QWidget
{
    Q_OBJECT;
public:
    using ptr = QSharedPointer<DishItem>;
    QString dish_id;
    QPushButton* dish_name;
    QLabel* single_price;
    QLabel* count;
    QLabel* pay;

    DishItem()
    {
        QGridLayout* layout = new QGridLayout;
        this->setLayout(layout);
        dish_name = new QPushButton("菜品n名称");
        single_price = new QLabel("单价:xx元");
        count= new QLabel("菜品数量:00");
        pay = new QLabel("菜品总价:000");
        dish_name->setFixedWidth(100);
        single_price->setFixedWidth(100);
        count->setFixedWidth(100);
        pay->setFixedWidth(100);

        QSpacerItem* spacer = new QSpacerItem(100,50);

        layout->addWidget(dish_name,0,0);
        layout->addWidget(single_price,0,1);
        layout->addWidget(count,0,2);
        layout->addItem(spacer,0,3);
        layout->addWidget(pay,0,4);

        connect(dish_name,&QPushButton::clicked,this,&DishItem::dishNameBtnSlot);
    }
public slots:
    void dishNameBtnSlot()
    {
        emit dishInfoSignal(dish_id);//向外传递信号
    }
signals:
    void dishInfoSignal(QString dish_id);
};
struct CartItem:public QWidget
{
    Q_OBJECT
public:
    using ptr = QSharedPointer<CartItem>;
    QString merchant_id;
    QLabel* merchant_name;
    QLabel* pay;

    QPushButton* OrderGenerateBtn;
    QPushButton* clearCartBtn;

    QList<DishItem::ptr> list;
    CartItem()
    {
        merchant_id = "商家uuid";
        merchant_name = new QLabel("商家1名称");
        pay = new QLabel("预付00元");
        OrderGenerateBtn = new QPushButton("生成订单");
        clearCartBtn = new QPushButton("清空购物车");

        connect(OrderGenerateBtn,&QPushButton::clicked,this,&CartItem::OrderGenerateSlot);
        connect(clearCartBtn,&QPushButton::clicked,this,&CartItem::CartClearSlot);

        //默认加三个
        list.push_back(DishItem::ptr(new DishItem));
        list.push_back(DishItem::ptr(new DishItem));
        list.push_back(DishItem::ptr(new DishItem));

        QGridLayout* layout = new QGridLayout;
        this->setLayout(layout);
        layout->addWidget(merchant_name,0,0);
        layout->addWidget(pay,0,1);
        layout->addWidget(OrderGenerateBtn,0,3);
        layout->addWidget(clearCartBtn,0,4);
        int line  = 1;
        for(DishItem::ptr p:list)
        {
            //全都绑定到同一个槽函数
            connect(p.data(),&DishItem::dishInfoSignal,this,&CartItem::dishItemBtnSlot);
            layout->addWidget(p.data(),line,0,1,4);
            line++;
        }
    }

public slots:
    void dishItemBtnSlot(QString dish_id)
    {
        emit dishInfoSignal(dish_id);//向外传递信号
    }
    void OrderGenerateSlot()
    {
        emit OrderGenerateSignal(merchant_id);
    }
    void CartClearSlot()
    {
        emit CartClearSignal(merchant_id);
    }
signals:
    void dishInfoSignal(QString dish_id);
    void OrderGenerateSignal(QString merchant_id);
    void CartClearSignal(QString merchant_id);
};
}
class ConsumerCartListWidget : public QWidget
{
    Q_OBJECT
public:

    explicit ConsumerCartListWidget(QWidget *parent = nullptr);
    ConsumerNavWidget* leftNavW;
    QWidget* rightW;
    void initRightW();

    QHash<QString,cartList::CartItem::ptr> cartTable;
public:


public slots:
    //接收右子控件的槽
    void dishInfoSlot(QString dish_id);
    void OrderGenerateSlot(QString merchant_id);
    void CartClearSlot(QString merchant_id);
signals:
    //右子控件的信号转发
    void dishInfoSignal(QString dish_id);
    void orderGenerateSignal(QString merchant_id);
    void cartClearSignal(QString merchant_id);
};

#endif // CONSUMERCARTLISTWIDGET_H
