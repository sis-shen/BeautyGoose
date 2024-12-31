#include "merchantorderdetailwidget.h"
#include <QScrollArea>
#include <QScrollBar>
#include <QPushButton>
using namespace merchant::orderDetail;
MerchantOrderDetailWidget::MerchantOrderDetailWidget(QWidget *parent)
    : QWidget{parent}
{
    QGridLayout* mainLayout = new QGridLayout;
    this->setLayout(mainLayout);
    mainW = new QWidget;
    mainLayout->addWidget(mainW);

    initMainW();


}

void MerchantOrderDetailWidget::initMainW()
{
    mainW->setFixedHeight(600);
    mainW->setFixedWidth(1000);
    QGridLayout* rlayout = new QGridLayout;
    mainW->setLayout(rlayout);
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

    QPushButton* accBtn = new QPushButton("接单");
    QPushButton* rejBtn = new QPushButton("拒单");
    accBtn->setFixedSize(100,50);
    rejBtn->setFixedSize(100,50);

    b_layout->addWidget(accBtn,0,0);
    b_layout->addWidget(rejBtn,0,1);

    connect(accBtn,&QPushButton::clicked,this,&MerchantOrderDetailWidget::acceptOrderSlot);
    connect(rejBtn,&QPushButton::clicked,this,&MerchantOrderDetailWidget::rejectOrderSlot);
}

void MerchantOrderDetailWidget::acceptOrderSlot()
{
    emit acceptOrderSignal(order_id);
}

void MerchantOrderDetailWidget::rejectOrderSlot()
{
    emit rejectOrderSignal(order_id);
}
