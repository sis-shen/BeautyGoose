#include "mainwidget.h"
#include "./ui_mainwidget.h"

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    this->setFixedSize(1600,900);
    QGridLayout* layout = new QGridLayout;
    this->setLayout(layout);

    // toRegisterSlot();
    toNameLoginSlot();
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::clearAll()
{
    QLayout* layout = this->layout();
    while(layout->count())
    {
        QLayoutItem* obj=layout->takeAt(0);
        if(obj->widget())
        {
            delete obj->widget();
        }
    }
}

void MainWidget::toRegisterSlot()
{
    clearAll();
    RegisterWidget* rw = new RegisterWidget;
    this->layout()->addWidget(rw);
    connect(rw,&RegisterWidget::toNameLoginSignal,this,&MainWidget::toNameLoginSlot);
    connect(rw,&RegisterWidget::toPhoneLoginSignal,this,&MainWidget::toPhoneLoginSlot);
    connect(rw,&RegisterWidget::authCodeSignal,this,&MainWidget::authCodeSlot);
    connect(rw,&RegisterWidget::registerSiganl,this,&MainWidget::registerSlot);
}

void MainWidget::toNameLoginSlot()
{
    clearAll();
    LoginByNameWidget* ln = new LoginByNameWidget;
    this->layout()->addWidget(ln);
    connect(ln,&LoginByNameWidget::toRegisterSignal,this,&MainWidget::toRegisterSlot);
    connect(ln,&LoginByNameWidget::toPhoneLoginSignal,this,&MainWidget::toPhoneLoginSlot);
    connect(ln,&LoginByNameWidget::loginSignal,this,&MainWidget::loginNameSlot);
}

void MainWidget::toPhoneLoginSlot()
{
    clearAll();
    LoginByPhoneWidget* lb = new LoginByPhoneWidget;
    this->layout()->addWidget(lb);

    connect(lb,&LoginByPhoneWidget::loginSignal,this,&MainWidget::loginPhoneSlot);
    connect(lb,&LoginByPhoneWidget::toNameLoginSignal,this,&MainWidget::toNameLoginSlot);
    connect(lb,&LoginByPhoneWidget::toRegisterSignal,this,&MainWidget::toRegisterSlot);
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

void MainWidget::loginNameSlot(LoginByNameWidget::Input input)
{
    //TODO
    if(input.type == "CONSUMER")toConsumerDishListSlot();
    else toMerchantDishListSlot();
}

void MainWidget::loginPhoneSlot(LoginByPhoneWidget::Input input)
{
    //TODO
    toConsumerDishListSlot();
}

void MainWidget::toConsumerDishListSlot()
{
    clearAll();
    ConsumerDishListWidget* cdl = new ConsumerDishListWidget;
    this->layout()->addWidget((cdl));
    //连导航栏
    connect(cdl->leftNavW,&ConsumerNavWidget::toCartListSignal,this,&MainWidget::toCartListSlot);
    connect(cdl->leftNavW,&ConsumerNavWidget::toDishListSignal,this,&MainWidget::toConsumerDishListSlot);
    connect(cdl->leftNavW,&ConsumerNavWidget::toOrderListSignal,this,&MainWidget::toConsumerOrderListSlot);
    connect(cdl->leftNavW,&ConsumerNavWidget::toVIPSignal,this,&MainWidget::toVIPSlot);

    //连其它信号
    connect(cdl,&ConsumerDishListWidget::dishInfoSignal,this,&MainWidget::consumerDishInfoSlot);
}

void MainWidget::cddCloseSlot()
{
    qDebug()<<"close subwindow";
    sender()->deleteLater();
    cdd_win = nullptr;
}

void MainWidget::consumerDishInfoSlot(QString dish_id)
{
    if(cdd_win != nullptr)
    {
        //已经有窗口打开了，所以不开新的窗口
        return;
    }
    //若不存在，则创建窗口并显示
    cdd_win =new ConsumerDishDetailWindow;
    //连接窗口信号
    connect(cdd_win,&ConsumerDishDetailWindow::finished,this,&MainWidget::cddCloseSlot);
    //连接控件信号
    connect(cdd_win->cdd,
            &ConsumerDishDetailWidget::cartDishAddSignal,
            this,
            &MainWidget::cartDsihAddSlot);
    connect(cdd_win->cdd,
            &ConsumerDishDetailWidget::cartDishAddSignal,
            this,
            &MainWidget::cartDsihPopSlot);

    cdd_win->exec();
}

void MainWidget::cartDsihAddSlot(QString merchant_id, QString dish_id)
{

}

void MainWidget::cartDsihPopSlot(QString merchant_id, QString dish_id)
{

}


void MainWidget::toCartListSlot()
{
    clearAll();
    ConsumerCartListWidget* ccl = new ConsumerCartListWidget;
    this->layout()->addWidget(ccl);

    //连导航栏
    connect(ccl->leftNavW,&ConsumerNavWidget::toCartListSignal,this,&MainWidget::toCartListSlot);
    connect(ccl->leftNavW,&ConsumerNavWidget::toDishListSignal,this,&MainWidget::toConsumerDishListSlot);
    connect(ccl->leftNavW,&ConsumerNavWidget::toOrderListSignal,this,&MainWidget::toConsumerOrderListSlot);
    connect(ccl->leftNavW,&ConsumerNavWidget::toVIPSignal,this,&MainWidget::toVIPSlot);

    //其它信号
    connect(ccl,&ConsumerCartListWidget::dishInfoSignal,this,&MainWidget::consumerDishInfoSlot);
    connect(ccl,&ConsumerCartListWidget::orderGenerateSignal,this,&MainWidget::orderGenerateSlot);
    connect(ccl,&ConsumerCartListWidget::cartClearSignal,this,&MainWidget::cartClearSlot);
}

void MainWidget::cartClearSlot(QString merchant_id)
{

}

void MainWidget::orderGenerateSlot(QString merchant_id)
{

}

void MainWidget::toConsumerOrderListSlot()
{
    clearAll();
    ConsumerOrderListWidget* col = new ConsumerOrderListWidget;
    this->layout()->addWidget(col);
    //连导航栏
    connect(col->leftNavW,&ConsumerNavWidget::toCartListSignal,this,&MainWidget::toCartListSlot);
    connect(col->leftNavW,&ConsumerNavWidget::toDishListSignal,this,&MainWidget::toConsumerDishListSlot);
    connect(col->leftNavW,&ConsumerNavWidget::toOrderListSignal,this,&MainWidget::toConsumerOrderListSlot);
    connect(col->leftNavW,&ConsumerNavWidget::toVIPSignal,this,&MainWidget::toVIPSlot);

    //连其它信号
    connect(col,&ConsumerOrderListWidget::toOrderInfoSignal,this,&MainWidget::consumerOrderInfoSlot);
}

void MainWidget::consumerOrderInfoSlot(QString order_id)
{
    //TODO
    //没做传入id的接口
    clearAll();
    ConsumerOrderDetailWidget* cod = new ConsumerOrderDetailWidget;
    this->layout()->addWidget(cod);

    //连导航栏
    connect(cod->leftNavW,&ConsumerNavWidget::toCartListSignal,this,&MainWidget::toCartListSlot);
    connect(cod->leftNavW,&ConsumerNavWidget::toDishListSignal,this,&MainWidget::toConsumerDishListSlot);
    connect(cod->leftNavW,&ConsumerNavWidget::toOrderListSignal,this,&MainWidget::toConsumerOrderListSlot);
    connect(cod->leftNavW,&ConsumerNavWidget::toVIPSignal,this,&MainWidget::toVIPSlot);

    //连其它信号
    connect(cod,&ConsumerOrderDetailWidget::toOrderListSignal,this,&MainWidget::toConsumerOrderListSlot);
    connect(cod,&ConsumerOrderDetailWidget::payOrderSignal,this,&MainWidget::payOrderSlot);
    connect(cod,&ConsumerOrderDetailWidget::cancelSignal,this,&MainWidget::orderCancelSlot);
}


void MainWidget::toVIPSlot()
{

}

void MainWidget::payOrderSlot(QString order_id)
{

}

void MainWidget::orderCancelSlot(QString order_id)
{

}

void MainWidget::toMerchantDishListSlot()
{
    clearAll();
    MerchantDishListWidget* mdl = new MerchantDishListWidget;
    this->layout()->addWidget(mdl);

    //连导航栏
    connect(mdl->leftNavW,&MerchantNavWidget::toDishListSignal,this,&MainWidget::toMerchantDishListSlot);
    connect(mdl->leftNavW,&MerchantNavWidget::toOrderListSignal,this,&MainWidget::toMerchantOrderListSlot);
    connect(mdl->leftNavW,&MerchantNavWidget::toDishRegisterWindowSignal,this,&MainWidget::toDishRegisterWindowSlot);

    //连其它信号
    connect(mdl,&MerchantDishListWidget::merchantDishInfoSignal,this,&MainWidget::toMerchantDishInfoWindowSlot);
}

void MainWidget::toMerchantDishInfoWindowSlot(QString dish_id)
{
    if(mdd_win != nullptr)
    {
        return;//不允许同时打开多个窗口
    }
    mdd_win = new MerchantDishDetailWindow;
    //连接内部信号
    connect(mdd_win->mdd,&MerchantDishDetailWidget::dishEditSignal,this,&MainWidget::merchantDishEditSlot);
    //连接窗口关闭信号
    connect(mdd_win,&MerchantDishRegisterWindow::finished,this,&MainWidget::mddCloseSlot);

    mdd_win->exec();
}


void MainWidget::toMerchantOrderListSlot()
{
    clearAll();
    MerchantOrderListWidget* mol = new MerchantOrderListWidget;
    this->layout()->addWidget(mol);

    //连导航栏
    connect(mol->leftNavW,&MerchantNavWidget::toDishListSignal,this,&MainWidget::toMerchantDishListSlot);
    connect(mol->leftNavW,&MerchantNavWidget::toOrderListSignal,this,&MainWidget::toMerchantOrderListSlot);
    connect(mol->leftNavW,&MerchantNavWidget::toDishRegisterWindowSignal,this,&MainWidget::toDishRegisterWindowSlot);
    //连接其它信号
    connect(mol,&MerchantOrderListWidget::merchantOrderInfoSignal,this,&MainWidget::toMerchantOrderDetailSlot);
}

void MainWidget::toMerchantOrderDetailSlot()
{
    if(mod_win != nullptr)
    {
        return;//不允许同时打开多个窗口
    }
    mod_win = new MerchantOrderDetailWindow;
    //连接内部信号
    connect(mod_win->mod,&MerchantOrderDetailWidget::acceptOrderSignal,this,&MainWidget::merchantDishAcceptSlot);
    connect(mod_win->mod,&MerchantOrderDetailWidget::rejectOrderSignal,this,&MainWidget::merchantDishRejecttSlot);
    //连接窗口关闭信号
    connect(mod_win,&MerchantDishRegisterWindow::finished,this,&MainWidget::modCloseSlot);

    mod_win->exec();
}

void MainWidget::toDishRegisterWindowSlot()
{
    if(mdr_win != nullptr)
    {
        return;//不允许同时打开多个窗口
    }
    mdr_win = new MerchantDishRegisterWindow;
    //连接内部信号
    connect(mdr_win->mrd,&MerchantDishRegisterWidget::dishRegisterSignal,this,&MainWidget::merchantDishRegisterSlot);
    //连接窗口关闭信号
    connect(mdr_win,&MerchantDishRegisterWindow::finished,this,&MainWidget::mdrCloseSlot);

    mdr_win->exec();
}

void MainWidget::mdrCloseSlot()
{
    qDebug()<<"close subwindow";
    sender()->deleteLater();
    mdr_win = nullptr;
}

void MainWidget::merchantDishRegisterSlot(MerchantDishRegisterWidget::Input input)
{
    qDebug()<<"merchantDishRegisterSlot";
}

void MainWidget::modCloseSlot()
{
    qDebug()<<"close mod window";
    sender()->deleteLater();
    mod_win = nullptr;
}

void MainWidget::merchantDishAcceptSlot(QString order_id)
{
    mod_win->close();
}

void MainWidget::merchantDishRejecttSlot(QString order_id)
{
    mod_win->close();
}

void MainWidget::mddCloseSlot()
{
    qDebug()<<"close mod window";
    sender()->deleteLater();
    mdd_win = nullptr;
}

void MainWidget::merchantDishEditSlot()
{

}





