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

#include "CoreData.h"
#include "DataCenterCoreData.h"
#include "ColorConfig.h"

namespace cartList
{
struct DishItem:public QWidget
{
    Q_OBJECT;
public:
    using ptr = QSharedPointer<DishItem>;
    QString dish_id;
    QLabel* dish_icon;
    QPushButton* dish_name;
    QLabel* single_price;
    QLabel* count;
    QLabel* pay;

    DishItem(btyGoose::CartDishItem::ptr dish)
        :dish_id(dish->dish_id)
    {
        QFont font("微软雅黑",12);

        QGridLayout* layout = new QGridLayout;
        this->setLayout(layout);
        this->setStyleSheet("QWidget{border:0}");//取消边框显示
        dish_name = new QPushButton(dish->dish_name);
        dish_icon = new QLabel;
        single_price = new QLabel("单价:"+QString::number(dish->dish_price)+"元");
        count= new QLabel("菜品数量:" + QString::number(dish->cnt));
        pay = new QLabel("菜品总价:" + QString::number(dish->pay));
        dish_name->setFixedWidth(100);
        single_price->setFixedWidth(100);
        count->setFixedWidth(100);
        pay->setFixedWidth(100);

        QGridLayout* left_layout = new QGridLayout;
        QGridLayout* right_layout = new QGridLayout;

        layout->addLayout(left_layout,0,0);
        layout->addLayout(right_layout,0,1);

        left_layout->addWidget(dish_icon,0,0,2,1);
        left_layout->addWidget(dish_name,0,1);
        left_layout->addItem(new QSpacerItem(100,50),1,1);
        left_layout->setAlignment(Qt::AlignLeft);
        QIcon icon = btyGoose::util::getIconFromLink(dish->icon_path);
        //设置图标
        if(icon.isNull())
        {
            //如果图片下载失败，则使用默认图片
            icon = QIcon(QPixmap("://qsrc/defaultDish.png"));
        }
        QSize icon_size(100,100);
        dish_icon->setFixedSize(icon_size);
        dish_icon->setPixmap(icon.pixmap(icon_size));

        auto color = ColorConfig::getInstance();

        QSize label_size(100,50);
        right_layout->addWidget(single_price,0,0);
        right_layout->addWidget(count,0,1);
        right_layout->addWidget(pay,0,2);
        right_layout->setAlignment(Qt::AlignRight);
        QString label_color = color->main_color_light;
        single_price->setFont(font);
        count->setFont(font);
        pay->setFont(font);
        single_price->setFixedSize(label_size);
        count->setFixedSize(label_size);
        pay->setFixedSize(label_size);
        single_price->setStyleSheet(QString("QWidget{background-color:%1;"
                                            "border:0;"
                                            "border-radius:10px;"
                                            "font}").arg(label_color));
        count->setStyleSheet(QString("QWidget{background-color:%1;"
                                            "border:0;"
                                            "border-radius:10px}").arg(label_color));
        pay->setStyleSheet(QString("QWidget{background-color:%1;"
                                            "border:0;"
                                            "border-radius:10px}").arg(label_color));
        // layout->addWidget(dish_name,0,0);
        // layout->addWidget(single_price,0,1);
        // layout->addWidget(count,0,2);
        // layout->addItem(spacer,0,3);
        // layout->addWidget(pay,0,4);

        connect(dish_name,&QPushButton::clicked,this,&DishItem::dishNameBtnSlot);
    }

protected:
    void mousePressEvent(QMouseEvent *event) override {
        qDebug()<<"购物车菜品item鼠标点击事件";
        emit dishInfoSignal(dish_id);//向外传递信号
        QWidget::mousePressEvent(event);
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
    CartItem(btyGoose::Cart::ptr cart)
    {
        merchant_id = cart->merchant_id;
        merchant_name = new QLabel(cart->merchant_name);
        pay = new QLabel("预付:"+QString::number(cart->pay)+"元");
        OrderGenerateBtn = new QPushButton("生成订单");
        clearCartBtn = new QPushButton("清空购物车");

        connect(OrderGenerateBtn,&QPushButton::clicked,this,&CartItem::OrderGenerateSlot);
        connect(clearCartBtn,&QPushButton::clicked,this,&CartItem::CartClearSlot);

        for(auto it = cart->dish_table->begin();it!=cart->dish_table->end();++it)
        {
            list.push_back(DishItem::ptr(new DishItem(it.value())));
        }
        //默认加三个
        // list.push_back(DishItem::ptr(new DishItem));
        // list.push_back(DishItem::ptr(new DishItem));
        // list.push_back(DishItem::ptr(new DishItem));

        QGridLayout* layout = new QGridLayout;
        this->setLayout(layout);
        QWidget* nav = new QWidget;
        layout->addWidget(nav,0,0);
        QGridLayout* nav_layout = new QGridLayout;
        nav->setLayout(nav_layout);
        QWidget* nav_left = new QWidget;
        QWidget* nav_right = new QWidget;
        nav_layout->addWidget(nav_left,0,0);
        nav_layout->addWidget(nav_right,0,1);
        QGridLayout* nav_left_layout = new QGridLayout;
        QGridLayout* nav_right_layout = new QGridLayout;
        nav_left->setLayout(nav_left_layout);
        nav_right->setLayout(nav_right_layout);

        //设置布局
        nav_left_layout->addWidget(merchant_name,0,0);
        nav_left_layout->addWidget(pay,0,1);
        nav_left_layout->setAlignment(Qt::AlignLeft);
        nav_right_layout->addWidget(OrderGenerateBtn,0,0);
        nav_right_layout->addWidget(clearCartBtn,0,1);
        nav_right_layout->setAlignment(Qt::AlignRight);

        //设置样式
        auto color = ColorConfig::getInstance();
        nav->setStyleSheet(QString("QWidget{background-color: %1;border:0;"
                                   "font-family:微软雅黑; "
                                   "font-size:12pt}").arg(color->main_color));
        OrderGenerateBtn->setStyleSheet(QString("QPushButton{background-color:%1}").arg(color->main_color_light));
        clearCartBtn->setStyleSheet(QString("QPushButton{background-color:%1}").arg(color->main_color_light));
        // layout->addWidget(pay,0,1);
        // layout->addWidget(OrderGenerateBtn,0,3);
        // layout->addWidget(clearCartBtn,0,4);
        int line  = 1;
        for(DishItem::ptr p:list)
        {
            //全都绑定到同一个槽函数
            connect(p.data(),&DishItem::dishInfoSignal,this,&CartItem::dishItemBtnSlot);
            layout->addWidget(p.data(),line,0);
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

    explicit ConsumerCartListWidget(QHash<QString,btyGoose::Cart::ptr>* cart_table);
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
