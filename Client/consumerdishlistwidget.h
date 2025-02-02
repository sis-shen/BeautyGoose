#ifndef CONSUMERDISHLISTWIDGET_H
#define CONSUMERDISHLISTWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QScrollBar>
#include <QPushButton>
#include <QLabel>
#include <QString>
#include <QSpacerItem>
#include <QList>
#include <QPointer>
#include <QScrollArea>
#include <QHash>
#include <QEvent>
#include "Nav.h"

#include "CoreData.h"
#include "DataCenterCoreData.h"
#include "ColorConfig.h"

namespace dishList
{
struct DishItem:public QWidget
{
    Q_OBJECT;
public:
    using ptr = QSharedPointer<DishItem>;
    QString dish_id;
    QPushButton* dish_name;
    QPushButton* single_picture;
    QLabel* single_price;


    DishItem(const btyGoose::data::Dish& dish)
        :dish_id(dish.uuid)
    {
        auto color = ColorConfig::getInstance();
        QGridLayout* layout = new QGridLayout;
        QWidget* mainW = new QWidget;
        QGridLayout* mainL = new QGridLayout;
        mainL->addWidget(mainW);
        mainL->setContentsMargins(0,5,0,0);
        mainW->setLayout(layout);
        this->setLayout(mainL);
        mainW->setStyleSheet(QString("QWidget{border-radius: 15px;"
                                     " border: 2px solid %1;"
                                     "background-color: %2;}").arg(color->main_color_light,color->main_color_light));
        single_picture = new QPushButton();
        dish_name = new QPushButton(dish.name);
        QString price = "单价:";
        price += QString::number(dish.price_factor*dish.base_price);
        price += "元";
        single_price = new QLabel(price);

        QWidget* leftW = new QWidget;
        QWidget* rightW = new QWidget;

        single_picture->setFixedSize(100,100);
        QIcon icon = btyGoose::util::getIconFromLink(dish.icon_path);
        if(icon.isNull())
        {
            //如果图片下载失败，则使用默认图片
            icon = QIcon(QPixmap("://qsrc/defaultDish.png"));
        }
        single_picture->setIcon(icon);
        single_picture->setIconSize(QSize(100,100));
        dish_name->setFixedWidth(100);
        single_price->setFixedWidth(100);


        QGridLayout* leftLayout = new QGridLayout;
        leftLayout->setAlignment(Qt::AlignLeft);
        leftW->setLayout(leftLayout);
        leftLayout->addWidget(single_picture,0,0,2,1);
        leftLayout->addWidget(dish_name,0,1);
        QSpacerItem* space = new QSpacerItem(100,50);
        leftLayout->addItem(space,1,1);



        QGridLayout* rightLayout = new QGridLayout;
        rightLayout->setAlignment(Qt::AlignRight);
        rightLayout->addWidget(single_price);
        single_price->setFixedHeight(50);
        single_price->setStyleSheet(QString("QLabel{border-radius: 15px;"
                                            " border: 2px solid %1;"
                                            "background-color: %1;}").arg(color->main_color));
        rightW->setLayout(rightLayout);
        layout->addWidget(leftW,0,0);
        layout->addWidget(rightW,0,1);


        connect(dish_name,&QPushButton::clicked,this,&DishItem::dishNameBtnSlot);
        connect(single_picture,&QPushButton::clicked,this,&DishItem::dishNameBtnSlot);

    }
protected:
    void mousePressEvent(QMouseEvent *event) override {
        qDebug()<<"菜品item鼠标点击事件";
        emit dishInfoSignal(dish_id);//向外传递信号
        QWidget::mousePressEvent(event);
    }

public slots:
    void dishNameBtnSlot()
    {
        emit dishInfoSignal(dish_id);//向外传递信号
    }
signals:
    void dishInfoSignal(QString dish_id);
};
struct MerchantItem:public QWidget
{
    Q_OBJECT
public:
    using ptr = QSharedPointer<MerchantItem>;
    QString merchant_id;
    QLabel* merchant_name;


    QList<DishItem::ptr> list;
    MerchantItem(const QList<btyGoose::data::Dish>& lst)
    {
        auto color = ColorConfig::getInstance();


        merchant_id = lst[0].merchant_id;
        //默认加三个
        // qDebug()<<"开始往缓冲区插入菜品";
        for(auto dish:lst)
        {
            list.push_back(DishItem::ptr(new DishItem(dish)));
        }
        // qDebug()<<"插入完成";
        QWidget* mainW = new QWidget;
        QGridLayout* mainLayout = new QGridLayout;
        mainLayout->addWidget(mainW);
        QGridLayout* layout = new QGridLayout;
        this->setLayout(mainLayout);
        mainLayout->addWidget(mainW);
        mainW->setLayout(layout);

        merchant_name = new QLabel(lst[0].merchant_name);
        QWidget* nameW = new QWidget;
        nameW->setStyleSheet(QString("QWidget{background-color: %1;border-radius: 5px;}").arg(color->main_color));
        // nameW->setFixedWidth(this->width());
        QGridLayout* nameLayout = new QGridLayout;
        nameW->setLayout(nameLayout);
        nameLayout->addWidget(merchant_name);
        merchant_name->setParent(nameW);
        merchant_name->setStyleSheet(QString("QLabel{border-radius: 5px;"
                                             " border: 2px solid %1;}").arg(color->background_color2));
        layout->addWidget(nameW,0,0,1,4);//添加商家名称
        layout->setSpacing(0);

        mainW->setStyleSheet(QString("QWidget{background-color: %1;"
                                     "border: 2px solid %2}").arg(color->background_color2,color->background_color));

        int line  = 1;
        for(DishItem::ptr p:list)
        {
            //全都绑定到同一个槽函数
            connect(p.data(),&DishItem::dishInfoSignal,this,&MerchantItem::dishItemBtnSlot);
            layout->addWidget(p.data(),line,0,1,4);
            line++;
        }
    }

public slots:
    void dishItemBtnSlot(QString dish_id)
    {
        emit dishInfoSignal(dish_id);//向外传递信号
    }

signals:
    void dishInfoSignal(QString dish_id);

};
}
class ConsumerDishListWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ConsumerDishListWidget(QHash<QString,QList<btyGoose::data::Dish>>* dish_list_table);
    ConsumerNavWidget* leftNavW;
    QWidget* rightW;
    void initRightW();

    QHash<QString,dishList::MerchantItem::ptr> merchantTable;
public:


public slots:
    //接收右子控件的槽
    void dishInfoSlot(QString dish_id);

signals:
    //右子控件的信号转发
    void dishInfoSignal(QString dish_id);


};

#endif // CONSUMERCARTLISTWIDGET_H
