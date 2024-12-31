#include "adminorderlistwidget.h"
#include "QScrollArea"
#include <QScrollBar>

using namespace admin::orderList;
AdminOrderListWidget::AdminOrderListWidget(QWidget *parent)
    : QWidget{parent}
{
    this->setFixedSize(1600,900);
    QGridLayout* layout = new QGridLayout;
    this->setLayout(layout);
    rightW = new QWidget;
    QPushButton* Btn = new QPushButton("管理员界面");
    layout->setSpacing(0);
    Btn->setFixedSize(200,100);
    layout->addWidget(Btn,0,0);
    layout->setAlignment(Btn,Qt::AlignTop);
    // layout->setAlignment(Btn,Qt::AlignLeft|Qt::AlignTop);
    layout->addWidget(rightW,0,1);
    // layout->setAlignment(rightW,Qt::AlignLeft);

    list.push_back(OrderItem::ptr(new OrderItem));
    list.push_back(OrderItem::ptr(new OrderItem));
    list.push_back(OrderItem::ptr(new OrderItem));
    list.push_back(OrderItem::ptr(new OrderItem));
    list.push_back(OrderItem::ptr(new OrderItem));
    list.push_back(OrderItem::ptr(new OrderItem));
    list.push_back(OrderItem::ptr(new OrderItem));
    list.push_back(OrderItem::ptr(new OrderItem));
    list.push_back(OrderItem::ptr(new OrderItem));
    list.push_back(OrderItem::ptr(new OrderItem));
    initRight();
}

void AdminOrderListWidget::initRight()
{
    rightW->setStyleSheet("QWidget { border: 2px solid black; }");
    rightW->setFixedWidth(1000);
    rightW->setFixedHeight(800);
    rightW->setStyleSheet("QWidget { border: 2px solid black; }");
    QGridLayout* glayout = new QGridLayout;
    rightW->setLayout(glayout);
    QScrollArea* sa = new QScrollArea;
    glayout->addWidget(sa);
    sa->setWidgetResizable(true);
    sa->verticalScrollBar()->setStyleSheet("QScrollBar:vertical{width:10px;background-color: rgb(46,46,46);}");
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

}


