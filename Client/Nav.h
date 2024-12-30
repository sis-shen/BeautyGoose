#ifndef NAV_H
#define NAV_H
#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QPainter>
#include <QStyleOption>
#include <QStyle>
class ConsumerNavWidget:public QWidget
{
    Q_OBJECT
public:
    //导航栏切换界面的按钮
    QPushButton* viewDishList;
    QPushButton* viewOrderList;
    QPushButton* viewCartList;
    QPushButton* viewVIP;
    void paintEvent(QPaintEvent* event) override
    {
        (void)event;
        QStyleOption opt;
        opt.initFrom(this);
        QPainter p(this);
        style()->drawPrimitive(QStyle::PE_Widget,&opt,&p,this);
    }
    ConsumerNavWidget()
    {
        this->setStyleSheet("QWidget{background-color: rgb(46,46,46)}");
        this->setStyleSheet("QPushButton{background-color: rgb(246,246,246)}");
        this->setFixedWidth(150);
        this->setFixedHeight(900);

        viewDishList = new QPushButton("查看菜品列表");
        viewCartList = new QPushButton("查看购物车列表");
        viewOrderList = new QPushButton("查看订单列表");
        viewVIP = new QPushButton("VIP");

        QGridLayout* layout = new QGridLayout;
        this->setLayout(layout);
        layout->addWidget(viewDishList,0,0);
        layout->addWidget(viewCartList,1,0);
        layout->addWidget(viewOrderList,2,0);
        layout->addWidget(viewVIP,3,0);

        connect(viewDishList,&QPushButton::clicked,this,&ConsumerNavWidget::toDishListSlot);
        connect(viewCartList,&QPushButton::clicked,this,&ConsumerNavWidget::toCartListSlot);
        connect(viewOrderList,&QPushButton::clicked,this,&ConsumerNavWidget::toOrderListSlot);
        connect(viewVIP,&QPushButton::clicked,this,&ConsumerNavWidget::toVIPSlot);
    }
public slots:
    void toDishListSlot(){emit toDishListSignal();}
    void toCartListSlot(){emit toCartListSignal();}
    void toOrderListSlot(){emit toOrderListSignal();}
    void toVIPSlot(){emit toVIPSignal();}

signals:
    void toDishListSignal();
    void toCartListSignal();
    void toOrderListSignal();
    void toVIPSignal();
};

class MerchantNavWidget:public QWidget
{
    Q_OBJECT
public:
    //导航栏切换界面的按钮
    QPushButton* viewDishList;
    QPushButton* viewOrderList;
    QPushButton* dishRegister;

    void paintEvent(QPaintEvent* event) override
    {
        (void)event;
        QStyleOption opt;
        opt.initFrom(this);
        QPainter p(this);
        style()->drawPrimitive(QStyle::PE_Widget,&opt,&p,this);
    }

    MerchantNavWidget()
    {
        this->setStyleSheet("QWidget{background-color: rgb(46,46,46)}");
        this->setStyleSheet("QPushButton{background-color: rgb(246,246,246)}");
        this->setFixedWidth(150);
        this->setFixedHeight(900);

        viewDishList = new QPushButton("查看菜品列表");
        viewOrderList = new QPushButton("查看订单列表");
        dishRegister = new QPushButton("注册菜品");

        QGridLayout* layout = new QGridLayout;
        this->setLayout(layout);
        layout->addWidget(viewDishList,0,0);
        layout->addWidget(viewOrderList,1,0);
        layout->addWidget(dishRegister,2,0);

        connect(viewDishList,&QPushButton::clicked,this,&MerchantNavWidget::toDishListSlot);
        connect(viewOrderList,&QPushButton::clicked,this,&MerchantNavWidget::toOrderListSlot);
        connect(dishRegister,&QPushButton::clicked,this,&MerchantNavWidget::toDishRegisterWindowSlot);

    }
public slots:
    void toDishListSlot(){emit toDishListSignal();}
    void toOrderListSlot(){emit toOrderListSignal();}
    void toDishRegisterWindowSlot(){emit toDishRegisterWindowSignal();}

signals:
    void toDishListSignal();
    void toOrderListSignal();
    void toDishRegisterWindowSignal();
};


#endif // NAV_H
