#include "loginbyphone.h"
#include "QGridLayout"
#include "ColorConfig.h"
#include <QIcon>
#include <QLabel>
#include <QMessageBox>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
LoginByPhoneWidget::LoginByPhoneWidget(QWidget *parent)
    : QWidget{parent}
{
    QGridLayout* layout=new QGridLayout;
    this->setFixedSize(400,600);
    this->setLayout(layout);
    const ColorConfig* color = ColorConfig::getInstance();
    int m = 10;
    layout->setContentsMargins(m,m,m,m);

    QWidget* iconW = new QWidget;
    iconW->setStyleSheet(QString("QWidget { border: 3px dashed %1; border-radius: 10px; }").arg(color->main_color));
    iconW->setFixedWidth(350);
    QPushButton* iconBtn = new QPushButton;
    int n = 275;
    iconBtn->setFixedSize(n,n);
    iconBtn->setIconSize(QSize(n,n));
    iconBtn->setIcon(QIcon(QPixmap("://qsrc/loginByPhone.png")));
    QGridLayout* iconLayout = new QGridLayout;
    iconLayout->setAlignment(Qt::AlignCenter);
    iconW->setLayout(iconLayout);
    iconLayout->addWidget(iconBtn);
    layout->addWidget(iconW,0,0,1,3);
    //第一行
    QLabel *label=new QLabel;
    label->setText("账户类型");
    label->setAlignment(Qt::AlignCenter);//文字居中
    label->setStyleSheet(QString("QLabel{"
                                 "background-color: %1;"
                                 "border-radius: 10px;"
                                 "border: 2px solid %2;"
                                 "font-size: 15px;"
                                 "font-weight: 500;"
                                 "font-family: 'Microsoft YaHei';}").arg(color->main_color,color->main_color));
    layout->addWidget(label,1,0);

    label = new QLabel("无需选择，自动判断~");
    label->setAlignment(Qt::AlignCenter);//文字居中
    label->setStyleSheet(QString("QLabel{"
                                 "background-color: %1;"
                                 "border-radius: 10px;"
                                 "border: 2px solid %2;"
                                 "font-size: 15px;"
                                 "font-weight: 500;"
                                 "font-family: 'Microsoft YaHei';}").arg(color->main_color,color->main_color));
    label->setFixedWidth(300);
    label->setAlignment(Qt::AlignCenter);//文字居中
    label->setFixedHeight(50);
    layout->addWidget(label,1,1);

    //第二三行
    label = new QLabel("手机号");
    label->setAlignment(Qt::AlignCenter);//文字居中
    label->setStyleSheet(QString("QLabel{"
                                 "background-color: %1;"
                                 "border-radius: 10px;"
                                 "border: 2px solid %2;"
                                 "font-size: 15px;"
                                 "font-weight: 500;"
                                 "font-family: 'Microsoft YaHei';}").arg(color->main_color,color->main_color));
    layout->addWidget(label,2,0);
    phoneInput = new QTextEdit;
    phoneInput->setFixedHeight(50);
    layout->addWidget(phoneInput,2,1,1,6);

    label = new QLabel("密码");
    label->setAlignment(Qt::AlignCenter);//文字居中
    label->setStyleSheet(QString("QLabel{"
                                 "background-color: %1;"
                                 "border-radius: 10px;"
                                 "border: 2px solid %2;"
                                 "font-size: 15px;"
                                 "font-weight: 500;"
                                 "font-family: 'Microsoft YaHei';}").arg(color->main_color,color->main_color));
    layout->addWidget(label,3,0);
    passwordInput = new QLineEdit;
    passwordInput->setFixedHeight(50);
    layout->addWidget(passwordInput,3,1,1,6);

    loginBtn = new QPushButton("登录");
    label->setStyleSheet(QString("QLabel{"
                                 "background-color: %1;"
                                 "border-radius: 10px;"
                                 "border: 2px solid %2;"
                                 "font-size: 15px;"
                                 "font-weight: 500;"
                                 "font-family: 'Microsoft YaHei';}").arg(color->main_color,color->main_color));
    loginBtn->setFixedHeight(50);
    layout->addWidget(loginBtn,4,1,1,6);

    toNameLoginBtn = new QPushButton("转到用户名登录");
    toNameLoginBtn->setStyleSheet(QString("QPushButton {"
                                      "background-color: %1;"  // 设置背景色
                                      "color: %2;"
                                      "border: 0;}").arg(color->background_color,color->grey));
    toNameLoginBtn->setFixedHeight(25);
    layout->addWidget(toNameLoginBtn,5,1,1,3);
    toRegisterBtn = new QPushButton("没有账号？点我注册");
    toRegisterBtn->setStyleSheet(QString("QPushButton {"
                                          "background-color: %1;"  // 设置背景色
                                          "color: %2;"
                                          "border: 0;}").arg(color->background_color,color->grey));
    toRegisterBtn->setFixedHeight(25);
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

