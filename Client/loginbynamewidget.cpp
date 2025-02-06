#include "loginbynamewidget.h"
#include "ColorConfig.h"
#include <QLabel>
#include <QGridLayout>
#include <QRadioButton>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>
#include <QIcon>
#include <QPixmap>

LoginByNameWidget::LoginByNameWidget(QString name,QString password)
    : QWidget{nullptr}
{
    this->setFixedSize(400,600);
    QGridLayout* layout = new QGridLayout;
    this->setLayout(layout);

    const ColorConfig* color = ColorConfig::getInstance();

    int m=10;
    layout->setContentsMargins(m,m,m,m);

    QWidget* iconW = new QWidget;
    iconW->setStyleSheet(QString("QWidget { border: 3px dashed %1; border-radius: 10px; }").arg(color->main_color));
    iconW->setFixedWidth(350);
    QPushButton* iconBtn = new QPushButton;
    int n = 275;
    iconBtn->setFixedSize(n,n);
    iconBtn->setIconSize(QSize(n,n));
    iconBtn->setIcon(QIcon(QPixmap(":/qsrc/loginByName.png")));
    QGridLayout* iconLayout = new QGridLayout;
    iconLayout->setAlignment(Qt::AlignCenter);
    iconW->setLayout(iconLayout);
    iconLayout->addWidget(iconBtn);
    layout->addWidget(iconW,0,0,1,3);
    // layout->setAlignment(iconBtn,Qt::AlignHCenter);

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

    // consumerRadio=new QRadioButton;
    // consumerRadio->setChecked(true);
    // layout->addWidget(consumerRadio,0,1);

    // label=new QLabel;
    // label->setText("我是消费者");
    // layout->addWidget(label,0,2);

    // merchantRadio=new QRadioButton;
    // layout->addWidget(merchantRadio,0,3);

    // label=new QLabel;
    // label->setText("我是商家");
    // layout->addWidget(label,0,4);

    // adminRadio=new QRadioButton;
    // layout->addWidget(adminRadio,0,5);

    // label=new QLabel;
    // label->setText("我是管理员");
    // layout->addWidget(label,0,6);

    label=new QLabel;
    label->setText("用户名");
    label->setStyleSheet(QString("QLabel{"
                                 "background-color: %1;"
                                 "border-radius: 10px;"
                                 "border: 2px solid %2;"
                                 "font-size: 15px;"
                                 "font-weight: 500;"
                                 "font-family: 'Microsoft YaHei';}").arg(color->main_color,color->main_color));
    label->setAlignment(Qt::AlignCenter);//文字居中
    layout->addWidget(label,2,0);

    nameInput=new QLineEdit(name);
    nameInput->setFixedHeight(50);
    layout->addWidget(nameInput,2,1,1,6);

    label=new QLabel;
    label->setText("密码");
    label->setStyleSheet(QString("QLabel{"
                                 "background-color: %1;"
                                 "border-radius: 10px;"
                                 "border: 2px solid %2;"
                                 "font-size: 15px;"
                                 "font-weight: 500;"
                                 "font-family: 'Microsoft YaHei';}").arg(color->main_color,color->main_color));
    label->setAlignment(Qt::AlignCenter);//文字居中
    layout->addWidget(label,3,0);

    passwordInput=new QLineEdit(password);
    passwordInput->setFixedHeight(50);
    passwordInput->setEchoMode(QLineEdit::Password);
    layout->addWidget(passwordInput,3,1,1,6);

    loginBtn=new QPushButton("登录");
    label->setStyleSheet(QString("QLabel{"
                                 "background-color: %1;"
                                 "border-radius: 10px;"
                                 "border: 2px solid %2;"
                                 "font-size: 15px;"
                                 "font-weight: 500;"
                                 "font-family: 'Microsoft YaHei';}").arg(color->main_color,color->main_color));
    loginBtn->setFixedHeight(50);
    layout->addWidget(loginBtn,4,1,1,6);

    toPhoneBtn=new QPushButton("转到手机号登录");
    toPhoneBtn->setFixedHeight(25);
    toPhoneBtn->setStyleSheet(QString("QPushButton {"
                                           "background-color: %1;"  // 设置背景色
                                           "color: %2;"
                                           "border: 0;}").arg(color->background_color,color->grey));
    layout->addWidget(toPhoneBtn,5,1,1,3);

    toRegisterBtn=new QPushButton("没有账号？点我注册");
    toRegisterBtn->setFixedHeight(25);
    toRegisterBtn->setStyleSheet(QString("QPushButton {"
                                           "background-color: %1;"  // 设置背景色
                                           "color: %2;"
                                           "border: 0;}").arg(color->background_color,color->grey));
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
