#include "merchantdishdetailwidget.h"

MerchantDishDetailWidget::MerchantDishDetailWidget(const btyGoose::data::Dish* dish)
    : QWidget{nullptr},dish_id(dish->uuid),dish(dish)
{
    this->setFixedSize(1080,600);
    QWidget* MW = new QWidget;
    QGridLayout* mwl = new QGridLayout;
    this->setLayout(mwl);
    mwl->addWidget(MW);
    QGridLayout* layout = new QGridLayout;
    MW->setLayout(layout);
    MW->setObjectName("MW");
    MW->setStyleSheet("QWidget#MW { border: 2px solid black; }");
    upW = new QWidget;
    downW = new QWidget;
    ulW = new QWidget;
    urW = new QWidget;

    upW->setFixedSize(1080,400);
    layout->addWidget(upW,0,0);
    layout->addWidget(downW,1,0);

    initUp();
    initDown();

}

void MerchantDishDetailWidget::initUp()
{
    QGridLayout* ulayout = new QGridLayout;
    upW->setLayout(ulayout);
    ulayout->addWidget(ulW,0,0);
    ulayout->addWidget(urW,0,1);
    //左上角初始化
    ulW->setFixedWidth(200);
    QGridLayout* lLayout = new QGridLayout;
    ulW->setLayout(lLayout);

    QIcon icon = btyGoose::util::getIconFromLink(dish->icon_path);
    if(icon.isNull())
    {
        //如果图片下载失败，则使用默认图片
        icon = QIcon(QPixmap("://qsrc/defaultDish.png"));
    }
    icon_label = new QLabel();
    icon_label->setFixedSize(200,200);
    icon_label->setPixmap(icon.pixmap(200,200));
    auto color = ColorConfig::getInstance();
    QLabel* price = new QLabel("价格："+QString::number(dish->price_factor* dish->base_price)+"元");
    price->setStyleSheet(QString("QLabel{/*border-radius: 15px;*/"
                                 " border: 2px solid %1;"
                                 "background-color: %1;}").arg(color->main_color));
    price->setFixedSize(100,50);

    lLayout->addWidget(icon_label,0,0);
    lLayout->addWidget(price,1,0);

    //右上角初始化
    QGridLayout* rLayout = new QGridLayout;
    urW->setLayout(rLayout);
    QLabel* name = new QLabel(dish->name);
    name->setStyleSheet(QString("QLabel{/*border-radius: 15px;*/"
                                " border: 2px solid %1;"
                                "background-color: %1;}").arg(color->main_color));
    name->setFixedHeight(40);
    QLabel* profile = new QLabel(dish->description);
    profile->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    profile->setWordWrap(true);
    profile->setFixedWidth(600);

    rLayout->setAlignment(name,Qt::AlignLeft);
    rLayout->setAlignment(profile,Qt::AlignLeft);

    rLayout->addWidget(name,0,0);
    rLayout->addWidget(profile,1,0);
}

void MerchantDishDetailWidget::initDown()
{
    auto color = ColorConfig::getInstance();
    QGridLayout* layout = new QGridLayout;
    downW->setLayout(layout);
    QPushButton* editBtn = new QPushButton("编辑菜品");
    QString btnStyle = QString("QPushButton{border-radius: 15px;"
                               " border: 2px solid %1;"
                               "background-color: %1;}").arg(color->main_color);
    editBtn->setStyleSheet(btnStyle);

    editBtn->setFixedSize(100,50);

    layout->addWidget(editBtn,0,0);

    layout->setAlignment(editBtn,Qt::AlignRight);
    layout->setContentsMargins(0,0,50,0);

    connect(editBtn,&QPushButton::clicked,this,&MerchantDishDetailWidget::editSlot);
}


void MerchantDishDetailWidget::editSlot()
{
    emit dishEditSignal(dish_id);
}
