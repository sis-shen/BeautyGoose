#include "consumercartlistwidget.h"
#include <QStyle>
#include <QBoxLayout>
using namespace cartList;
ConsumerCartListWidget::ConsumerCartListWidget(QHash<QString,btyGoose::Cart::ptr>*cart_table)
{
    QGridLayout* layout = new QGridLayout;
    this->setLayout(layout);
    leftNavW = new ConsumerNavWidget;
    rightW = new QWidget;
    layout->addWidget(leftNavW,0,0);
    layout->addWidget(rightW,0,1);

    for(auto it = cart_table->begin();it != cart_table->end();++it)
    {
        cartTable.insert(it.key(),CartItem::ptr(new CartItem(it.value())));
    }

    //DEBUG //TODO
    // cartTable["商家id"] = CartItem::ptr(new CartItem);
    // cartTable["商家2id"] = CartItem::ptr(new CartItem);

    initRightW();

}

void ConsumerCartListWidget::initRightW()
{
    rightW->setFixedWidth(1000);
    rightW->setStyleSheet("QWidget { border: 2px solid black; }");
    QGridLayout* glayout = new QGridLayout;
    rightW->setLayout(glayout);
    QScrollArea* sa = new QScrollArea;
    glayout->addWidget(sa);
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
    for(auto it = cartTable.begin();it!=cartTable.end();++it)
    {
        CartItem::ptr p = it.value();
        layout->addWidget(p.data());

        connect(p.data(),&CartItem::dishInfoSignal,this,&ConsumerCartListWidget::dishInfoSlot);
        connect(p.data(),&CartItem::OrderGenerateSignal,this,&ConsumerCartListWidget::OrderGenerateSlot);
        connect(p.data(),&CartItem::CartClearSignal,this,&ConsumerCartListWidget::CartClearSlot);
    }
}

void ConsumerCartListWidget::dishInfoSlot(QString dish_id)
{
    emit dishInfoSignal(dish_id);
}

void ConsumerCartListWidget::OrderGenerateSlot(QString merchant_id)
{
    emit orderGenerateSignal(merchant_id);
}

void ConsumerCartListWidget::CartClearSlot(QString merchant_id)
{
    emit cartClearSignal(merchant_id);
}

