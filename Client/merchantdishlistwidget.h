#ifndef MERCHANTDISHLISTWIDGET_H
#define MERCHANTDISHLISTWIDGET_H

#include <QWidget>
#include <QIcon>
#include <QGridLayout>
#include "Nav.h"
#include <QLabel>
#include <QString>
#include <QPointer>
#include <QPushButton>
#include <QList>
namespace merchant
{
namespace dishList
{
struct DishItem:public QWidget
{
    Q_OBJECT
public:
    using ptr = QSharedPointer<DishItem>;
    QString dish_id = "dish_id";
    DishItem()
    {
        QWidget* mainW = new QWidget;
        QGridLayout*mLaout = new QGridLayout;
        this->setLayout(mLaout);
        mLaout->addWidget(mainW);

        mainW->setStyleSheet("QWidget { border: 2px solid black; }");

        QGridLayout* layout = new QGridLayout;
        mainW->setLayout(layout);

        QLabel* icon = new QLabel("图标");
        icon->setFixedSize(50,50);
        QPushButton* dishName = new QPushButton("这是一道菜");
        dishName->setFixedHeight(50);
        QLabel* dishPrice = new QLabel("价格：00");

        layout->addWidget(icon,0,0);
        layout->addWidget(dishName,0,1);
        layout->addWidget(dishPrice,0,2);

        connect(dishName,&QPushButton::clicked,this,&DishItem::dishInfoSlot);
    }
public slots:
    void dishInfoSlot(){emit dishInfoSignal(dish_id);}
signals:
    void dishInfoSignal(QString dish_id);
};
}
}

class MerchantDishListWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MerchantDishListWidget(QWidget *parent = nullptr);
    MerchantNavWidget* leftNavW;
    QWidget* rightW;

    QList<merchant::dishList::DishItem::ptr>list;
    void initRight();
public slots:    
    void merchantDishInfoSlot(QString dish_id);
signals:
    void merchantDishInfoSignal(QString dish_id);
};

#endif // MERCHANTDISHLISTWIDGET_H
