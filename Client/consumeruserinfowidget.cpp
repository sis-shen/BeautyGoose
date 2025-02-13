#include "consumeruserinfowidget.h"
#include "ColorConfig.h"
ConsumerUserInfoWidget::ConsumerUserInfoWidget(const btyGoose::data::Account*account)
    :_account(account)
{
    QGridLayout* layout = new QGridLayout;
    this->setLayout(layout);
    leftNavW = new ConsumerNavWidget;
    rightW = new QWidget;
    layout->addWidget(leftNavW,0,0);
    layout->addWidget(rightW,0,1);
    //DEBUG //TODO

    initRightW();
}

void ConsumerUserInfoWidget::initRightW()
{
    auto color = ColorConfig::getInstance();
    nameLB=new QLabel(_account->name);
    nicknameLB =new QLineEdit(_account->nickname);
    phoneLB = new QLabel(_account->phone);

    int h = 50,w = 400;

    nameLB->setFixedSize(w,h);
    nicknameLB->setFixedSize(w,h);
    phoneLB->setFixedSize(w,h);

    rightW->setFixedWidth(1000);
    rightW->setStyleSheet("QWidget { border: 2px solid black; }");
    QGridLayout* layout = new QGridLayout;
    // layout->setSpacing(0);
    rightW->setLayout(layout);
    layout->setAlignment(Qt::AlignTop);
    icon = new QLabel;
    QIcon i = QIcon(QPixmap(_account->icon)).pixmap(200,200);
    if(i.isNull())
    {
        i = QIcon(QPixmap("://qsrc/avatar.png"));
    }
    icon->setPixmap(i.pixmap(200,200));
    icon->setFixedSize(200,200);
    layout->addWidget(icon,0,0,1,4);

    QLabel* label = new QLabel("用户名");
    label->setStyleSheet(QString("QLabel{/*border-radius: 15px;*/"
                                " border: 2px solid %1;"
                                "background-color: %1;}").arg(color->main_color));
    label->setFixedWidth(100);
    layout->addWidget(label,1,0);
    layout->addWidget(nameLB,1,1);

    label = new QLabel("昵称");
    label->setStyleSheet(QString("QLabel{/*border-radius: 15px;*/"
                                 " border: 2px solid %1;"
                                 "background-color: %1;}").arg(color->main_color));
    label->setFixedWidth(100);
    layout->addWidget(label,2,0);
    layout->addWidget(nicknameLB,2,1);
    QPushButton* Btn = new QPushButton("修改");

    Btn->setFixedHeight(50);
    layout->addWidget(Btn,2,2);
    connect(Btn,&QPushButton::clicked,this,&ConsumerUserInfoWidget::changeNicknameSlot);


    label = new QLabel("手机号");
    label->setStyleSheet(QString("QLabel{/*border-radius: 15px;*/"
                                 " border: 2px solid %1;"
                                 "background-color: %1;}").arg(color->main_color));
    label->setFixedWidth(100);
    layout->addWidget(label,3,0);
    layout->addWidget(phoneLB,3,1);

}

void ConsumerUserInfoWidget::changeNicknameSlot()
{
    QString nickname = nicknameLB->text();
    if(nickname.isEmpty())return;//不接受空的昵称
    emit changeNicknameSignal(nickname);
}
