#include "merchantdisheditwidget.h"

MerchantDishEditWidget::MerchantDishEditWidget(const btyGoose::data::Dish* dish)
    : QWidget{nullptr},dish_id(dish->uuid)
{
    this->setFixedSize(1080,600);
    QGridLayout* layout = new QGridLayout;
    this->setLayout(layout);

    int m=10;
    layout->setContentsMargins(m,m,m+20,m);

    QLabel* label=new QLabel;
    label->setText("菜品图片链接");
    layout->addWidget(label,0,0);

    dishPictureLinkInput=new QLineEdit;
    dishPictureLinkInput->setFixedHeight(50);
    dishPictureLinkInput->setText(dish->icon_path);
    layout->addWidget(dishPictureLinkInput,0,1,1,6);

    label=new QLabel;
    label->setText("菜品名称");
    layout->addWidget(label,1,0);

    dishNameInput=new QLineEdit;
    dishNameInput->setFixedHeight(50);
    dishNameInput->setText(dish->name);
    layout->addWidget(dishNameInput,1,1,1,6);

    label=new QLabel;
    label->setText("菜品价格");
    layout->addWidget(label,2,0);

    dishPriceInput=new QLineEdit;
    dishPriceInput->setFixedHeight(50);
    dishPriceInput->setText(QString::number(dish->base_price));
    layout->addWidget(dishPriceInput,2,1,1,6);

    label=new QLabel;
    label->setText("菜品简介");

    layout->addWidget(label,3,0);

    dishIntroductionInput=new QPlainTextEdit;
    dishIntroductionInput->setFixedHeight(150);
    dishIntroductionInput->setPlainText(dish->description);
    layout->addWidget(dishIntroductionInput,3,1,1,6);

    dishDeleteBtn=new QPushButton("删除菜品");
    dishDeleteBtn->setFixedHeight(50);
    layout->addWidget(dishDeleteBtn,4,0,1,3);

    dishSaveBtn=new QPushButton("保存更改");
    dishSaveBtn->setFixedHeight(50);
    layout->addWidget(dishSaveBtn,4,4,1,3);


    connect(dishSaveBtn,&QPushButton::clicked,this,&MerchantDishEditWidget::saveSlot);
    connect(dishDeleteBtn,&QPushButton::clicked,this,&MerchantDishEditWidget::delSlot);
}

void MerchantDishEditWidget::saveSlot()
{
    Input input;
    // qDebug()<<"dishRegisterSlot text start";
    input.dish_id = dish_id;
    input.link=dishPictureLinkInput->text();
    input.name=dishNameInput->text();
    input.price=dishPriceInput->text().toDouble();
    input.introduction=dishIntroductionInput->toPlainText();
    // qDebug()<<"dishRegisterSlot text done";
    emit merchantDishSaveSignal(input);
}

void MerchantDishEditWidget::delSlot()
{
    emit merchantDishDelSignal(dish_id);
}
