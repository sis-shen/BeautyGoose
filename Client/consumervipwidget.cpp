#include "consumervipwidget.h"

ConsumerVIPWidget::ConsumerVIPWidget(btyGoose::data::Account::Level level)
    : QWidget{nullptr}
{
    QGridLayout* layout = new QGridLayout;
    this->setLayout(layout);
    leftNavW = new ConsumerNavWidget;
    rightW = new QWidget;
    layout->addWidget(leftNavW,0,0);
    layout->addWidget(rightW,0,1);
    initRight(level);
}

void ConsumerVIPWidget::initRight(btyGoose::data::Account::Level level)
{
    rightW->setFixedSize(1000,800);
    rightW->setStyleSheet("QWidget { border: 2px solid black; }");
    QGridLayout* layout = new QGridLayout;
    rightW->setLayout(layout);
    QString lstr;
    if(level == btyGoose::data::Account::Level::MEMBER)
    {
        lstr = "普通会员";
    }
    else if(level == btyGoose::data::Account::Level::VIP)
    {
        lstr = "VIP会员";
    }
    else if(level == btyGoose::data::Account::Level::SUVIP)
    {
        lstr = "SUVIP会员";
    }
    QLabel* label = new QLabel("当前用户等级:"+lstr);
    label->setFixedHeight(50);
    QPushButton* VIPBtn = new QPushButton("升级为VIP");
    QPushButton* SUVIPBtn = new QPushButton("升级为SUVIP");

    VIPBtn->setFixedSize(150,150);
    SUVIPBtn->setFixedSize(150,150);


    layout->addWidget(label,0,0,1,2);
    layout->addWidget(VIPBtn,1,0);
    layout->addWidget(SUVIPBtn,1,1);

    connect(VIPBtn,&QPushButton::clicked,this,&ConsumerVIPWidget::VIPSlot);
    connect(SUVIPBtn,&QPushButton::clicked,this,&ConsumerVIPWidget::SUVIPSlot);
}

void ConsumerVIPWidget::VIPSlot()
{
    emit accountUpgradeSignal("VIP");
}

void ConsumerVIPWidget::SUVIPSlot()
{
    emit accountUpgradeSignal("SUVIP");
}
