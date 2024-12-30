#include "loginbynamewidget.h"
#include <QLabel>
#include <QGridLayout>
#include <QRadioButton>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>

LoginByNameWidget::LoginByNameWidget(QWidget *parent)
    : QWidget{parent}
{
    this->setFixedSize(1600,900);
    QGridLayout* layout = new QGridLayout;
    this->setLayout(layout);

    int m=150;
    layout->setContentsMargins(m,m,m,m);

    QLabel *label=new QLabel;
    label->setText("账户类型");
    layout->addWidget(label,0,0);

    consumerRadio=new QRadioButton;
    consumerRadio->setChecked(true);
    layout->addWidget(consumerRadio,0,1);

    label=new QLabel;
    label->setText("我是消费者");
    layout->addWidget(label,0,2);

    merchantRadio=new QRadioButton;
    layout->addWidget(merchantRadio,0,3);

    label=new QLabel;
    label->setText("我是商家");
    layout->addWidget(label,0,4);

    adminRadio=new QRadioButton;
    layout->addWidget(adminRadio,0,5);

    label=new QLabel;
    label->setText("我是管理员");
    layout->addWidget(label,0,6);

    label=new QLabel;
    label->setText("用户名");
    layout->addWidget(label,2,0);

    nameInput=new QLineEdit;
    nameInput->setFixedHeight(50);
    layout->addWidget(nameInput,2,1,1,6);

    label=new QLabel;
    label->setText("密码");
    layout->addWidget(label,3,0);

    passwordInput=new QLineEdit;
    passwordInput->setFixedHeight(50);
    passwordInput->setEchoMode(QLineEdit::Password);
    layout->addWidget(passwordInput,3,1,1,6);

    loginBtn=new QPushButton("登录");
    loginBtn->setFixedHeight(50);
    layout->addWidget(loginBtn,4,1,1,6);

    toPhoneBtn=new QPushButton("转到手机号登录");
    toPhoneBtn->setFixedHeight(50);
    layout->addWidget(toPhoneBtn,5,1,1,3);

    toRegisterBtn=new QPushButton("没有账号？点我注册");
    toRegisterBtn->setFixedHeight(50);
    layout->addWidget(toRegisterBtn,5,4,1,3);

    connect(loginBtn,&QPushButton::clicked,this,&LoginByNameWidget::loginSlot);

    //先暂时设置两个槽接受信号
    connect(toPhoneBtn,&QPushButton::clicked,this,&LoginByNameWidget::toPhoneBtnClickedSlot);
    connect(toRegisterBtn,&QPushButton::clicked,this,&LoginByNameWidget::toRegisterBtnClickedSlot);
}

bool LoginByNameWidget::check()
{
    //toplaintext不知道是什么，用text了
    QString name = nameInput->text();
    if(name.isEmpty())
    {
        QMessageBox::information(this, "Info", "用户名不能为空！");
        return false;
    }
    QString password = passwordInput->text();
    if(password.size()<8)
    {
        QMessageBox::information(this, "Info", "密码不能小于8位！");
        return false;
    }
    return true;
}

void LoginByNameWidget::loginSlot()
{
    if(!check())
    {
        return;
    }
    Input input;
    if(consumerRadio->isChecked())
    {
        input.type = "CONSUMER";
    }
    else if(merchantRadio->isChecked())
    {
        input.type = "MERCHANT";
    }
    else if(adminRadio->isChecked()){
        input.type ="ADMIN";
    }

    //toplaintext不知道是什么，用text了
    input.name = nameInput->text();
    input.password = passwordInput->text();

    qDebug()<<"loginSlot";
    emit loginSignal(input);
}
void LoginByNameWidget::toPhoneBtnClickedSlot()
{
    emit toPhoneLoginSignal();
}
void LoginByNameWidget::toRegisterBtnClickedSlot()
{
    emit toRegisterSignal();
}
