#include "consumerorderdetailwidget.h"
#include <QScrollArea>
#include <QBoxLayout>
#include <QScrollBar>
using namespace consumer::orderDetail;
ConsumerOrderDetailWidget::ConsumerOrderDetailWidget(QWidget *parent)
    : QWidget{parent},order_id("order_id")
{
    QGridLayout* layout = new QGridLayout;
    this->setLayout(layout);
    leftNavW = new ConsumerNavWidget;
    rightW = new QWidget;
    layout->addWidget(leftNavW,0,0);
    layout->addWidget(rightW,0,1);
    //DEBUG //TODO
    list.push_back(OrderDishItem::ptr(new OrderDishItem));
    list.push_back(OrderDishItem::ptr(new OrderDishItem));
    list.push_back(OrderDishItem::ptr(new OrderDishItem));
    list.push_back(OrderDishItem::ptr(new OrderDishItem));
    list.push_back(OrderDishItem::ptr(new OrderDishItem));

    initRight();
}

void ConsumerOrderDetailWidget::initRight()
{
    rightW->setFixedHeight(900);
    rightW->setFixedWidth(1000);
    QGridLayout* rlayout = new QGridLayout;
    rightW->setLayout(rlayout);
    QWidget* top = new QWidget;
    QWidget* mid = new QWidget;
    QWidget* bottom = new QWidget;

    rlayout->addWidget(top,0,0);
    rlayout->addWidget(mid,1,0);
    rlayout->addWidget(bottom,2,0);

    //初始化top
    top->setFixedHeight(100);
    QGridLayout* topLayout = new QGridLayout;
    top->setLayout(topLayout);
    QLabel* order_id = new QLabel("订单号: o111222333444");
    QLabel* pay = new QLabel("支付:00元");
    topLayout->setAlignment(order_id,Qt::AlignLeft);
    topLayout->setAlignment(pay,Qt::AlignLeft);
    topLayout->addWidget(order_id,0,0);
    topLayout->addWidget(pay,1,0);

    //初始化mid
    mid->setStyleSheet("QWidget{border: 2px solid black;}");
    mid->setFixedHeight(400);
    mid->setFixedWidth(1000);
    QGridLayout* mid_layout= new QGridLayout;
    mid->setLayout(mid_layout);

    QScrollArea* sa = new QScrollArea;
    mid_layout->addWidget(sa);
    sa->setWidgetResizable(true);
    sa->verticalScrollBar()->setStyleSheet("QScrollBar:vertical{width:2px;background-color: rgb(46,46,46);}");
    sa->horizontalScrollBar()->setStyleSheet("QScrollBar:horizontal{height:0;}");

    //3.实例化container
    QWidget* container =new QWidget;
    container->setFixedWidth(1000);//设置固定宽度
    sa->setWidget(container);//告诉滚动区和哪个控件配合

    //4.指定布局管理器
    QVBoxLayout* layout = new QVBoxLayout;
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
    layout->setAlignment(Qt::AlignTop);
    container->setLayout(layout);

    //插入每个商家的购物车控件
    for(auto p:list)
    {
        layout->addWidget(p.data());
    }


    //初始化bottom
    QGridLayout* b_layout = new QGridLayout;
    bottom->setLayout(b_layout);

    QPushButton* statusBtn = new QPushButton("状态：待支付");
    QPushButton* payBtn = new QPushButton("支付订单");
    QPushButton* listBtn = new QPushButton("返回");
    QPushButton* cancelBtn = new QPushButton("取消订单");
    statusBtn->setFixedSize(100,50);
    payBtn->setFixedSize(100,50);
    listBtn->setFixedSize(100,50);
    cancelBtn->setFixedSize(100,50);

    b_layout->addWidget(statusBtn,0,0);
    b_layout->addWidget(payBtn,0,1);
    b_layout->addWidget(listBtn,0,2);
    b_layout->addWidget(cancelBtn,0,3);

    connect(payBtn,&QPushButton::clicked,this,&ConsumerOrderDetailWidget::payOrderSlot);
    connect(listBtn,&QPushButton::clicked,this,&ConsumerOrderDetailWidget::toOrderListSLot);
    connect(cancelBtn,&QPushButton::clicked,this,&ConsumerOrderDetailWidget::cancelSlot);
}

void ConsumerOrderDetailWidget::payOrderSlot()
{
    emit payOrderSignal(order_id);
}

void ConsumerOrderDetailWidget::toOrderListSLot()
{
    emit toOrderListSignal();
}

void ConsumerOrderDetailWidget::cancelSlot()
{
    emit cancelSignal(order_id);
}
