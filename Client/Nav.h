#ifndef NAV_H
#define NAV_H
#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QPainter>
#include <QStyleOption>
#include <QStyle>
#include <QDebug>
#include <QStyleFactory>

#include "ColorConfig.h"
class ConsumerNavWidget:public QWidget
{
    Q_OBJECT
public:

    //导航栏切换界面的按钮
    QPushButton* viewUserInfo;
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
        this->setFixedWidth(150);
        this->setFixedHeight(800);
        auto color = ColorConfig::getInstance();
        QGridLayout* mainLayout = new QGridLayout;
        QWidget* mainW = new QWidget;
        this->setLayout(mainLayout);
        mainLayout->addWidget(mainW);
        mainW->setObjectName("Nav");
        // mainW->setStyleSheet(QString("QPushButton { background-color: %1;}").arg(color->background_color));
        mainW->setStyleSheet("QWidget#Nav { border: 2px solid black;}");
        QGridLayout* layout = new QGridLayout;
        mainW->setLayout(layout);
        layout->setAlignment(Qt::AlignTop);

        //个人信息
        QWidget* widget = new QWidget;
        QGridLayout* lay = new QGridLayout;
        lay->setAlignment(Qt::AlignHCenter);
        // widget->setFixedHeight(125);
        widget->setLayout(lay);
        viewUserInfo = new QPushButton("查看个人信息");
        QPushButton* userInfoIcon = new QPushButton;
        userInfoIcon->setIcon(QIcon(QPixmap("://qsrc/avatar.png")));
        userInfoIcon->setIconSize(QSize(100,100));
        viewUserInfo->setStyleSheet("QPushButton { text-align: center; }");
        userInfoIcon->setFixedSize(100,100);
        viewUserInfo->setFixedSize(100,25);
        lay->addWidget(userInfoIcon,0,0);
        lay->addWidget(viewUserInfo,1,0);
        lay->setSpacing(0);
        lay->setAlignment(Qt::AlignTop);
        layout->addWidget(widget);



        viewDishList = new QPushButton("查看菜品列表");
        widget = new QWidget;
        // widget->setFixedHeight(100);
        lay = new QGridLayout;
        widget->setLayout(lay);
        QPushButton* dishListIcon = new QPushButton;
        dishListIcon->setIcon(QIcon(QPixmap("://qsrc/dishList.png")));
        dishListIcon->setIconSize(QSize(75,75));
        viewDishList->setStyleSheet("QPushButton { text-align: center; }");
        dishListIcon->setFixedSize(75,75);
        viewDishList->setFixedSize(100,25);
        lay->addWidget(dishListIcon,0,0);
        lay->addWidget(viewDishList,1,0);
        lay->setSpacing(0);
        lay->setAlignment(Qt::AlignTop);
        layout->addWidget(widget);





        viewCartList = new QPushButton("查看购物车列表");
        widget = new QWidget;
        // widget->setFixedHeight(100);
        lay = new QGridLayout;
        widget->setLayout(lay);
        QPushButton* cartListIcon = new QPushButton;
        cartListIcon->setIcon(QIcon(QPixmap("://qsrc/cart.png")));
        cartListIcon->setIconSize(QSize(75,75));
        viewCartList->setStyleSheet("QPushButton { text-align: center; }");
        cartListIcon->setFixedSize(75,75);
        viewCartList->setFixedSize(100,25);
        lay->addWidget(cartListIcon,0,0);
        lay->addWidget(viewCartList,1,0);
        lay->setSpacing(0);
        lay->setAlignment(Qt::AlignTop);
        layout->addWidget(widget);




        viewOrderList = new QPushButton("查看订单列表");
        widget = new QWidget;
        // widget->setFixedHeight(100);
        lay = new QGridLayout;
        widget->setLayout(lay);
        QPushButton* orderListIcon = new QPushButton;
        orderListIcon->setIcon(QIcon(QPixmap("://qsrc/order.png")));
        orderListIcon->setIconSize(QSize(75,75));
        viewOrderList->setStyleSheet("QPushButton { text-align: center; }");
        orderListIcon->setFixedSize(75,75);
        viewOrderList->setFixedSize(100,25);
        lay->addWidget(orderListIcon,0,0);
        lay->addWidget(viewOrderList,1,0);
        lay->setSpacing(0);
        lay->setAlignment(Qt::AlignTop);
        layout->addWidget(widget);


        viewVIP = new QPushButton("VIP");
        layout->addWidget(viewVIP);


        // layout->addWidget(viewUserInfo,0,0);
        // layout->addWidget(viewDishList,1,0);
        // layout->addWidget(viewCartList,2,0);
        // layout->addWidget(viewOrderList,3,0);
        // layout->addWidget(viewVIP,4,0);

        connect(viewUserInfo,&QPushButton::clicked,this,&ConsumerNavWidget::toUserInfoSlot);
        connect(userInfoIcon,&QPushButton::clicked,this,&ConsumerNavWidget::toUserInfoSlot);
        connect(viewDishList,&QPushButton::clicked,this,&ConsumerNavWidget::toDishListSlot);
        connect(dishListIcon,&QPushButton::clicked,this,&ConsumerNavWidget::toDishListSlot);
        connect(viewCartList,&QPushButton::clicked,this,&ConsumerNavWidget::toCartListSlot);
        connect(cartListIcon,&QPushButton::clicked,this,&ConsumerNavWidget::toCartListSlot);
        connect(viewOrderList,&QPushButton::clicked,this,&ConsumerNavWidget::toOrderListSlot);
        connect(orderListIcon,&QPushButton::clicked,this,&ConsumerNavWidget::toOrderListSlot);
        connect(viewVIP,&QPushButton::clicked,this,&ConsumerNavWidget::toVIPSlot);
    }
public slots:
    void toUserInfoSlot(){emit toUesrInfoSignal();}
    void toDishListSlot(){emit toDishListSignal();}
    void toCartListSlot(){emit toCartListSignal();}
    void toOrderListSlot(){emit toOrderListSignal();}
    void toVIPSlot(){emit toVIPSignal();}

signals:
    void toUesrInfoSignal();
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
    QPushButton* viewUserInfo;
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
        this->setFixedWidth(150);
        this->setFixedHeight(800);
        QGridLayout* mainLayout = new QGridLayout;
        QWidget* mainW = new QWidget;
        this->setLayout(mainLayout);
        mainLayout->addWidget(mainW);
        mainW->setStyleSheet("QWidget { border: 2px solid black;}");
        viewUserInfo = new QPushButton("查看个人信息");
        viewUserInfo->setFixedSize(100,100);
        viewDishList = new QPushButton("查看菜品列表");
        viewOrderList = new QPushButton("查看订单列表");
        dishRegister = new QPushButton("注册菜品");

        QGridLayout* layout = new QGridLayout;
        mainW->setLayout(layout);
        layout->addWidget(viewUserInfo,0,0);
        layout->addWidget(viewDishList,1,0);
        layout->addWidget(viewOrderList,2,0);
        layout->addWidget(dishRegister,3,0);

        connect(viewUserInfo,&QPushButton::clicked,this,&MerchantNavWidget::toUserInfoSlot);
        connect(viewDishList,&QPushButton::clicked,this,&MerchantNavWidget::toDishListSlot);
        connect(viewOrderList,&QPushButton::clicked,this,&MerchantNavWidget::toOrderListSlot);
        connect(dishRegister,&QPushButton::clicked,this,&MerchantNavWidget::toDishRegisterWindowSlot);

    }
public slots:
    void toUserInfoSlot(){emit toUserInfoSignal();}
    void toDishListSlot(){emit toDishListSignal();}
    void toOrderListSlot(){emit toOrderListSignal();}
    void toDishRegisterWindowSlot(){emit toDishRegisterWindowSignal();}

signals:
    void toUserInfoSignal();
    void toDishListSignal();
    void toOrderListSignal();
    void toDishRegisterWindowSignal();
};


#endif // NAV_H
