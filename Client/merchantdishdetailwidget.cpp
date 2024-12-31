#include "merchantdishdetailwidget.h"

MerchantDishDetailWidget::MerchantDishDetailWidget(QWidget *parent)
    : QWidget{parent}
{
    this->setFixedSize(1080,600);
    QWidget* MW = new QWidget;
    QGridLayout* mwl = new QGridLayout;
    this->setLayout(mwl);
    mwl->addWidget(MW);
    QGridLayout* layout = new QGridLayout;
    MW->setLayout(layout);
    MW->setStyleSheet("QWidget { border: 2px solid black; }");
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
    //TODO 此处应改为用QICON
    iconTMP = new QLabel("图片");
    iconTMP->setFixedSize(200,200);

    QLabel* price = new QLabel("价格：00元");
    price->setFixedSize(100,50);

    lLayout->addWidget(iconTMP,0,0);
    lLayout->addWidget(price,1,0);

    //右上角初始化
    QGridLayout* rLayout = new QGridLayout;
    urW->setLayout(rLayout);
    QLabel* name = new QLabel("群英荟萃之萝卜开会");
    name->setFixedHeight(40);
    QLabel* profile = new QLabel("这是一个多行文本\n这是一个多行文本\n这是一个多行文本\n这是一个多行文本\n");
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
    QGridLayout* layout = new QGridLayout;
    downW->setLayout(layout);
    QPushButton* editBtn = new QPushButton("编辑菜品");

    editBtn->setFixedSize(100,50);

    layout->addWidget(editBtn,0,0);

    layout->setAlignment(editBtn,Qt::AlignRight);

    connect(editBtn,&QPushButton::clicked,this,&MerchantDishDetailWidget::editSlot);
}


void MerchantDishDetailWidget::editSlot()
{
    emit dishEditSignal(dish_id);
}
