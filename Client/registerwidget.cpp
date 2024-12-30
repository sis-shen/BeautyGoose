#include "registerwidget.h"
#include "QGridLayout"
#include <QLabel>
#include <QMessageBox>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
RegisterWidget::RegisterWidget(QWidget *parent)
    : QWidget{parent}
{
    this->setFixedSize(1600,900);
    QGridLayout* layout = new QGridLayout;

    int m = 150;
    layout->setContentsMargins(m,m,m,m);
    //第一行
    QLabel* label = new QLabel;
    label->setText("账户类型");
    layout->addWidget(label,0,0);
    consumerRadio = new QRadioButton;
    consumerRadio->setChecked(true);
    layout->addWidget(consumerRadio,0,1);

    label = new QLabel ;
    label->setText("我是消费者");
    layout->addWidget(label,0,2);
    merchantRadio = new QRadioButton;
    layout->addWidget(merchantRadio,0,3);
    label = new QLabel;
    label->setText("我是商家");
    layout->addWidget(label,0,4);

    //第二三四五六七八行
    label = new QLabel("用户名");
    layout->addWidget(label,1,0);
    nameInput = new QTextEdit;
    nameInput->setFixedHeight(50);
    layout->addWidget(nameInput,1,1,1,4);
    label = new QLabel("昵称");
    layout->addWidget(label,2,0);
    nicknameInput = new QTextEdit;
        nicknameInput->setFixedHeight(50);
    layout->addWidget(nicknameInput,2,1,1,4);
    label = new QLabel("密码");
    layout->addWidget(label,3,0);
    passwordInput = new QLineEdit;
        passwordInput->setFixedHeight(50);
    passwordInput->setEchoMode(QLineEdit::Password);
    layout->addWidget(passwordInput,3,1,1,4);
    label = new QLabel("重复密码");
    layout->addWidget(label,4,0);
    passwordRepeatInput = new QLineEdit;
        passwordRepeatInput->setFixedHeight(50);
        passwordRepeatInput->setEchoMode(QLineEdit::Password);
    layout->addWidget(passwordRepeatInput,4,1,1,4);
    label = new QLabel("手机号");
    layout->addWidget(label,5,0);
    phoneInput = new QTextEdit;
        phoneInput->setFixedHeight(50);
    layout->addWidget(phoneInput,5,1,1,4);
    label = new QLabel("短信验证码");
    layout->addWidget(label,6,0);
    auth_codeInput = new QTextEdit;
        auth_codeInput->setFixedHeight(50);
    auth_codeInput->setFixedWidth(100);
    layout->addWidget(auth_codeInput,6,1);

    getCodeBtn = new QPushButton("获取验证码");
    getCodeBtn->setFixedHeight(50);
    layout->addWidget(getCodeBtn,6,2);

    registerBtn = new QPushButton("注册");
    registerBtn->setFixedHeight(50);
    layout->addWidget(registerBtn,7,1,1,4);

    toNameLoginBtn = new QPushButton("转到用户名登录");
    layout->addWidget(toNameLoginBtn,8,2,1,2);
    toPhoneLoginBtn = new QPushButton("转到手机号登录");
    layout->addWidget(toPhoneLoginBtn,8,4,1,2);


    this->setLayout(layout);

    connect(registerBtn,&QPushButton::clicked,this,&RegisterWidget::registerSlot);
    connect(getCodeBtn,&QPushButton::clicked,this,&RegisterWidget::getCodeSlot);
    connect(toNameLoginBtn,&QPushButton::clicked,this,&RegisterWidget::toNameLoginSignal);
    connect(toPhoneLoginBtn,&QPushButton::clicked,this,&RegisterWidget::toPhoneLoginSignal);
}

bool RegisterWidget::check()
{
    QString name = nameInput->toPlainText();
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

    QString passwordR = passwordRepeatInput->text();
    if(password != passwordR)
    {
        QMessageBox::information(this, "Info", "两次i密码不一样");
        return false;
    }
    QRegularExpression regex("^[1][3-9][0-9]{9}$");
    QString phone = phoneInput->toPlainText();
    if (regex.match(phone).hasMatch() == false)
    {
        QMessageBox::information(this, "Info", "非法手机号！");
        return false;
    }
    return true;
}

void RegisterWidget::registerSlot()
{
    if(!check())
    {
        return;
    }
    QString code = auth_codeInput->toPlainText();
    if(code.size() != 6)
    {
        QMessageBox::information(this, "Info", "请输入六位验证码！");
        return;
    }

    Input input;
    if(consumerRadio->isChecked())
    {
        input.type = "CONSUMER";
    }
    else
    {
        input.type = "MERCHANT";
    }
    input.name = nameInput->toPlainText();
    QString nickname = nameInput->toPlainText();
    if(nickname.isEmpty())
    {
        input.nickname = input.name;
    }
    else
    {
        input.nickname = nickname;
    }
    input.password = passwordInput->text();
    input.phone = phoneInput->toPlainText();
    input.auth_code = auth_codeInput->toPlainText();

    emit registerSiganl(input);
}

void RegisterWidget::getCodeSlot()
{
    if(check())
    {
        emit authCodeSignal();
    }
}

void RegisterWidget::toNameSlot()
{
    emit toNameLoginSignal();
}

void RegisterWidget::toPhoneSlot()
{
    emit toPhoneLoginSignal();
}

