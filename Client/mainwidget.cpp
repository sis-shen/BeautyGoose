#include "mainwidget.h"
#include "./ui_mainwidget.h"

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    this->setFixedSize(1600,900);

    RegisterWidget* rw = new RegisterWidget;
    rw->setParent(this);
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::clearAll()
{

}

void MainWidget::initRegisterConnection(RegisterWidget *sender)
{
    connect(sender,&RegisterWidget::toNameLoginSignal,this,&MainWidget::toNameLoginSlot);
    connect(sender,&RegisterWidget::toPhoneLoginSignal,this,&MainWidget::toPhoneLoginSlot);
    connect(sender,&RegisterWidget::authCodeSignal,this,&MainWidget::authCodeSlot);
    connect(sender,&::RegisterWidget::registerSiganl,this,&MainWidget::registerSlot);
}

void MainWidget::toRegisterSlot()
{

}

void MainWidget::toNameLoginSlot()
{

}

void MainWidget::toPhoneLoginSlot()
{

}


void MainWidget::authCodeSlot()
{

}

void MainWidget::registerSlot(RegisterWidget::Input input)
{
    if(true)
    {
        QMessageBox::information(this, "Info", "注册成功！");
        toNameLoginSlot();
    }
    else
    {
        //TODO
        //输出失败原因
        QMessageBox::information(this, "Info", "注册失败！");
    }
}

