#include "loginbyphone.h"
#include "QGridLayout"
#include <QLabel>
#include <QMessageBox>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
LoginByPhoneWidget::LoginByPhoneWidget(QWidget *parent)
    : QWidget{parent}
{
    QGridLayout* layout=new QGridLayout;
    this->setFixedSize(1600,900);
    this->setLayout(layout);

    int m = 150;
    layout->setContentsMargins(m,m,m,m);
    //第一行
    QLabel* label = new QLabel;
    label->setText("账户类型");
    layout->addWidget(label,0,0);

    consumerRadio = new QRadioButton;
    consumerRadio->setChecked(true);
    layout->addWidget(consumerRadio,0,1);

    label = new QLabel;
    label->setText("我是消费者");
    layout->addWidget(label,0,2);

    merchantRadio = new QRadioButton;
    layout->addWidget(merchantRadio,0,3);

    label = new QLabel;
    label->setText("我是商家");
    layout->addWidget(label,0,4);

    //第二三行
    label = new QLabel("手机号");
    layout->addWidget(label,2,0);
    phoneInput = new QTextEdit;
    phoneInput->setFixedHeight(50);
    layout->addWidget(phoneInput,2,1,1,6);

    label = new QLabel("密码");
    layout->addWidget(label,3,0);
    passwordInput = new QLineEdit;
    passwordInput->setFixedHeight(50);
    layout->addWidget(passwordInput,3,1,1,6);

    loginBtn = new QPushButton("登录");
    loginBtn->setFixedHeight(50);
    layout->addWidget(loginBtn,4,1,1,6);

    toNameLoginBtn = new QPushButton("转到用户名登录");
    toNameLoginBtn->setFixedHeight(50);
    layout->addWidget(toNameLoginBtn,5,1,1,3);
    toRegisterBtn = new QPushButton("没有账号？点我注册");
    toRegisterBtn->setFixedHeight(50);
    layout->addWidget(toRegisterBtn,5,4,1,3);

    connect(loginBtn,&QPushButton::clicked,this,&LoginByPhoneWidget::loginSlot);
    connect(toNameLoginBtn,&QPushButton::clicked,this,&LoginByPhoneWidget::toNameBtnClickedSlot);
    connect(toRegisterBtn,&QPushButton::clicked,this,&LoginByPhoneWidget::toRegisterBtnClickedSlot);
}
bool LoginByPhoneWidget::check()
{
    QRegularExpression regex("^[1][3-9][0-9]{9}$");
    QString phone = phoneInput->toPlainText();
    if (regex.match(phone).hasMatch() == false)
    {
        QMessageBox::information(this, "Info", "非法手机号！");
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

void LoginByPhoneWidget::loginSlot()
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
    else if(adminRadio->isChecked())
    {
        input.type ="ADMIN";
    }

    //toplaintext不知道是什么，用text了
    input.password = passwordInput->text();
    input.phone = phoneInput->toPlainText();

    emit loginSignal(input);
}

void LoginByPhoneWidget::toRegisterBtnClickedSlot()
{
    emit toRegisterSignal();
}

void LoginByPhoneWidget::toNameBtnClickedSlot()
{
    emit toNameLoginSignal();
}

