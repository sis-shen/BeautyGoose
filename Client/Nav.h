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

        QGridLayout* mainLayout = new QGridLayout;
        QWidget* mainW = new QWidget;
        this->setLayout(mainLayout);
        mainLayout->addWidget(mainW);
        mainW->setStyleSheet("QWidget { border: 2px solid black;}");
        viewUserInfo = new QPushButton("查看个人信息");
        viewUserInfo->setFixedSize(100,100);
        viewDishList = new QPushButton("查看菜品列表");
        viewCartList = new QPushButton("查看购物车列表");
        viewOrderList = new QPushButton("查看订单列表");
        viewVIP = new QPushButton("VIP");

        QGridLayout* layout = new QGridLayout;
        mainW->setLayout(layout);
        layout->addWidget(viewUserInfo,0,0);
        layout->addWidget(viewDishList,1,0);
        layout->addWidget(viewCartList,2,0);
        layout->addWidget(viewOrderList,3,0);
        layout->addWidget(viewVIP,4,0);

        connect(viewUserInfo,&QPushButton::clicked,this,&ConsumerNavWidget::toUserInfoSlot);
        connect(viewDishList,&QPushButton::clicked,this,&ConsumerNavWidget::toDishListSlot);
        connect(viewCartList,&QPushButton::clicked,this,&ConsumerNavWidget::toCartListSlot);
        connect(viewOrderList,&QPushButton::clicked,this,&ConsumerNavWidget::toOrderListSlot);
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
