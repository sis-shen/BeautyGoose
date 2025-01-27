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
#include <QHash>
#include "Nav.h"

#include "CoreData.h"
#include "DataCenterCoreData.h"
#include "ColorConfig.h"

namespace dishList
{
struct DishItem:public QWidget
{
    Q_OBJECT;
public:
    using ptr = QSharedPointer<DishItem>;
    QString dish_id;
    QPushButton* dish_name;
    QPushButton* single_picture;
    QLabel* single_price;


    DishItem(const btyGoose::data::Dish& dish)
        :dish_id(dish.uuid)
    {
        auto color = ColorConfig::getInstance();
        QGridLayout* layout = new QGridLayout;
        QWidget* mainW = new QWidget;
        QGridLayout* mainL = new QGridLayout;
        mainL->addWidget(mainW);
        mainW->setLayout(layout);
        this->setLayout(mainL);

        mainW->setStyleSheet(QString("QWidget{border-radius: 15px;"
                                     " border: 2px solid %1;}").arg(color->background_color));
        single_picture = new QPushButton();
        dish_name = new QPushButton(dish.name);
        QString price = "单价:";
        price += QString::number(dish.price_factor*dish.base_price);
        price += "元";
        single_price = new QLabel(price);

        single_picture->setFixedSize(100,100);
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
    MerchantItem(const QList<btyGoose::data::Dish>& lst)
    {
        auto color = ColorConfig::getInstance();
        merchant_name = new QLabel(lst[0].merchant_name);
        merchant_name->setStyleSheet(QString("QLabel{border-radius: 5px;"
                                             " border: 2px solid %1;}").arg(color->background_color2));
        merchant_id = lst[0].merchant_id;
        //默认加三个
        // qDebug()<<"开始往缓冲区插入菜品";
        for(auto dish:lst)
        {
            list.push_back(DishItem::ptr(new DishItem(dish)));
        }
        // qDebug()<<"插入完成";
        QWidget* mainW = new QWidget;
        QGridLayout* mainLayout = new QGridLayout;
        mainLayout->addWidget(mainW);
        QGridLayout* layout = new QGridLayout;
        this->setLayout(mainLayout);
        mainLayout->addWidget(mainW);
        mainW->setLayout(layout);
        layout->addWidget(merchant_name,0,0);

        mainW->setStyleSheet(QString("QWidget{background-color: %1;"
                                     "border: 2px solid %2}").arg(color->background_color2,color->background_color));

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
    explicit ConsumerDishListWidget(QHash<QString,QList<btyGoose::data::Dish>>* dish_list_table);
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
