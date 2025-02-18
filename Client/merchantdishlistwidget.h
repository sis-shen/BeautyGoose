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

#include "CoreData.h"
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
    DishItem(const btyGoose::data::Dish& dish)
        :dish_id(dish.uuid)
    {
        auto color = ColorConfig::getInstance();
        QWidget* mainW = new QWidget;
        QGridLayout*mLaout = new QGridLayout;
        this->setLayout(mLaout);
        mLaout->addWidget(mainW);

        // mainW->setStyleSheet("QWidget { border: 2px solid black; }");
        mainW->setStyleSheet(QString("QWidget{border-radius: 15px;"
                                     " border: 2px solid %1;"
                                     "background-color: %2;}").arg(color->main_color_light,color->main_color_light));

        QGridLayout* layout = new QGridLayout;
        mainW->setLayout(layout);
        QIcon icon = btyGoose::util::getIconFromLink(dish.icon_path);
        if(icon.isNull())
        {
            //如果图片下载失败，则使用默认图片
            icon = QIcon(QPixmap("://qsrc/defaultDish.png"));
        }
        QLabel* icon_label = new QLabel();
        icon_label->setFixedSize(50,50);
        icon_label->setPixmap(icon.pixmap(50,50));
        QPushButton* dishName = new QPushButton(dish.name);
        dishName->setStyleSheet(QString("QLabel{border-radius: 15px;"
                                            " border: 2px solid %1;"
                                            "background-color: %1;}").arg(color->main_color));
        dishName->setFixedHeight(50);
        QLabel* dishPrice = new QLabel("价格：" + QString::number(dish.base_price*dish.price_factor));
        dishPrice->setStyleSheet(QString("QLabel{border-radius: 15px;"
                                            " border: 2px solid %1;"
                                            "background-color: %1;}").arg(color->main_color));
        layout->addWidget(icon_label,0,0);
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
    explicit MerchantDishListWidget(const QHash<QString,btyGoose::data::Dish>*dish_table);
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
