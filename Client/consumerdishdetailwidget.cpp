#include "consumerdishdetailwidget.h"

ConsumerDishDetailWidget::ConsumerDishDetailWidget(QWidget *parent)
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

void ConsumerDishDetailWidget::setCnt(int n)
{
    QString content = QString("购物车内数量:") + QString::number(n);
    cnt->setText(content);
}

void ConsumerDishDetailWidget::initUp()
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

void ConsumerDishDetailWidget::initDown()
{
    QGridLayout* layout = new QGridLayout;
    downW->setLayout(layout);
    QPushButton* addBtn = new QPushButton("添加到购物车");
    QPushButton* popBtn = new QPushButton("从购物车移除");
    cnt = new QLabel;
    setCnt(99);

    addBtn->setFixedHeight(50);
    popBtn->setFixedHeight(50);
    cnt->setFixedHeight(50);

    layout->addWidget(addBtn,0,0);
    layout->addWidget(popBtn,1,0);
    layout->addWidget(cnt,2,0);

    layout->setAlignment(addBtn,Qt::AlignRight);
    layout->setAlignment(popBtn,Qt::AlignRight);
    layout->setAlignment(cnt,Qt::AlignRight);

    connect(addBtn,&QPushButton::clicked,this,&ConsumerDishDetailWidget::addSlot);
    connect(popBtn,&QPushButton::clicked,this,&ConsumerDishDetailWidget::popSlot);
}

void ConsumerDishDetailWidget::addSlot()
{
    emit cartDishAddSignal(merchant_id,dish_id);
}

void ConsumerDishDetailWidget::popSlot()
{
    emit cartDishPopSignal(merchant_id,dish_id);
}
