#ifndef CONSUMERDISHLISTWIDGET_H
#define CONSUMERDISHLISTWIDGET_H

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

namespace dishList
{
struct DishItem:public QWidget
{
    Q_OBJECT;
public:
    using ptr = QSharedPointer<DishItem>;
    QString dish_id;
    QPushButton* dish_name;
    QLabel* single_picture;
    QLabel* single_price;


    DishItem()
    {
        QGridLayout* layout = new QGridLayout;
        this->setLayout(layout);
        single_picture = new QLabel("图片");
        dish_name = new QPushButton("菜品n名称");
        single_price = new QLabel("单价:xx元");

        single_picture->setFixedWidth(100);
        dish_name->setFixedWidth(100);
        single_price->setFixedWidth(100);


        QSpacerItem* spacer = new QSpacerItem(100,50);

        layout->addWidget(single_picture,0,0);
        layout->addWidget(dish_name,0,1);
        layout->addWidget(single_price,0,2);

        layout->addItem(spacer,0,3);


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
struct MerchantItem:public QWidget
{
    Q_OBJECT
public:
    using ptr = QSharedPointer<MerchantItem>;
    QString merchant_id;
    QLabel* merchant_name;


    QList<DishItem::ptr> list;
    MerchantItem()
    {

        merchant_name = new QLabel("商家1名称");



        //默认加三个
        list.push_back(DishItem::ptr(new DishItem));
        list.push_back(DishItem::ptr(new DishItem));
        list.push_back(DishItem::ptr(new DishItem));

        QGridLayout* layout = new QGridLayout;
        this->setLayout(layout);
        layout->addWidget(merchant_name,0,0);

        int line  = 1;
        for(DishItem::ptr p:list)
        {
            //全都绑定到同一个槽函数
            connect(p.data(),&DishItem::dishInfoSignal,this,&MerchantItem::dishItemBtnSlot);
            layout->addWidget(p.data(),line,0,1,4);
            line++;
        }
    }

public slots:
    void dishItemBtnSlot(QString dish_id)
    {
        emit dishInfoSignal(dish_id);//向外传递信号
    }

signals:
    void dishInfoSignal(QString dish_id);

};
}
class ConsumerDishListWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ConsumerDishListWidget(QWidget *parent = nullptr);
    ConsumerNavWidget* leftNavW;
    QWidget* rightW;
    void initRightW();

    QHash<QString,dishList::MerchantItem::ptr> merchantTable;
public:


public slots:
    //接收右子控件的槽
    void dishInfoSlot(QString dish_id);

signals:
    //右子控件的信号转发
    void dishInfoSignal(QString dish_id);


};

#endif // CONSUMERCARTLISTWIDGET_H
