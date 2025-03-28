#include "merchantorderlistwidget.h"
#include <QScrollArea>
#include <QScrollBar>
using namespace merchant::orderList;
MerchantOrderListWidget::MerchantOrderListWidget(const QHash<QString,btyGoose::data::Order>* order_table)
    : QWidget{nullptr}
{
    QGridLayout* layout = new QGridLayout;
    this->setLayout(layout);
    leftNavW = new MerchantNavWidget;
    rightW = new QWidget;
    layout->addWidget(leftNavW,0,0);
    layout->addWidget(rightW,0,1);
    for(auto it = order_table->begin();it!=order_table->end();++it)
    {
        list.push_back(OrderItem::ptr(new OrderItem(it.value())));
    }
    // list.push_back(OrderItem::ptr(new OrderItem));

    initRight();
}

void MerchantOrderListWidget::initRight()
{
    rightW->setFixedWidth(1000);
    rightW->setFixedHeight(800);
    rightW->setObjectName("rightW");
    rightW->setStyleSheet("QWidget#rightW { border: 2px solid black; }");
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

        connect(p.data(),&OrderItem::orderInfoSignal,this,&MerchantOrderListWidget::merchantOrderInfoSlot);
    }

}

void MerchantOrderListWidget::merchantOrderInfoSlot(QString order_id)
{
    emit merchantOrderInfoSignal(order_id);
}
