#include "consumerdishlistwidget.h"
#include <QStyle>
#include <QBoxLayout>
using namespace dishList;
ConsumerDishListWidget::ConsumerDishListWidget(QHash<QString,QList<btyGoose::data::Dish>>* dish_list_table)
: QWidget{nullptr}
{
    QGridLayout* layout = new QGridLayout;
    this->setLayout(layout);
    leftNavW = new ConsumerNavWidget;
    rightW = new QWidget;
    layout->addWidget(leftNavW,0,0);
    layout->addWidget(rightW,0,1);
    //DEBUG //TODO
    // qDebug()<<"开始往缓冲区插入菜品列表";
    for(auto it = dish_list_table->begin();it!= dish_list_table->end();++it)
    {
        // qDebug()<<"第一个列表"<<it.value().size();
        merchantTable.insert(it.key(),MerchantItem::ptr(new MerchantItem(it.value())));
    }
    // qDebug()<<"插入完成";
    initRightW();

}

void ConsumerDishListWidget::initRightW()
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
    for(auto it = merchantTable.begin();it!=merchantTable.end();++it)
    {
       MerchantItem::ptr p = it.value();
        layout->addWidget(p.data());

        connect(p.data(),&MerchantItem::dishInfoSignal,this,&ConsumerDishListWidget::dishInfoSlot);

    }
}

void ConsumerDishListWidget::dishInfoSlot(QString dish_id)
{
    emit dishInfoSignal(dish_id);
}


