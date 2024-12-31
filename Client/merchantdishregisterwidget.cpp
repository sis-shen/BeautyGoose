#include "merchantdishegisterwidget.h"

#include <QLabel>
#include <QGridLayout>
#include <QRadioButton>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>

MerchantDishRegisterWidget::MerchantDishRegisterWidget(QWidget *parent)
    : QWidget{parent}
{
    this->setFixedSize(1600,900);
    QGridLayout* layout = new QGridLayout;
    this->setLayout(layout);

    int m=150;
    layout->setContentsMargins(m,m,m,m);

    QLabel* label=new QLabel;
    label->setText("菜品图片链接");
    layout->addWidget(label,0,0);

    dishPictureLinkInput=new QLineEdit;
    dishPictureLinkInput->setFixedHeight(50);
    layout->addWidget(dishPictureLinkInput,0,1,1,6);

    label=new QLabel;
    label->setText("菜品名称");
    layout->addWidget(label,1,0);

    dishNameInput=new QLineEdit;
    dishNameInput->setFixedHeight(50);
    layout->addWidget(dishNameInput,1,1,1,6);

    label=new QLabel;
    label->setText("菜品价格");
    layout->addWidget(label,2,0);

    dishPriceInput=new QLineEdit;
    dishPriceInput->setFixedHeight(50);
    layout->addWidget(dishPriceInput,2,1,1,6);

    label=new QLabel;
    label->setText("菜品简介");

    layout->addWidget(label,3,0);

    dishIntroductionInput=new QPlainTextEdit;
    dishIntroductionInput->setFixedHeight(150);
    layout->addWidget(dishIntroductionInput,3,1,1,6);


    dishRegisterBtn=new QPushButton("注册菜品");
    dishRegisterBtn->setFixedHeight(50);
    layout->addWidget(dishRegisterBtn,4,4,1,3);

    connect(dishRegisterBtn,&QPushButton::clicked,this,&MerchantDishRegisterWidget::dishRegisterSlot);
}

void MerchantDishRegisterWidget::dishRegisterSlot()
{
    Input input;
    // qDebug()<<"dishRegisterSlot text start";
    input.link=dishPictureLinkInput->text();
    input.name=dishNameInput->text();
    input.price=dishPriceInput->text().toDouble();
    input.introduction=dishIntroductionInput->toPlainText();
    // qDebug()<<"dishRegisterSlot text done";
    emit dishRegisterSignal(input);
}
