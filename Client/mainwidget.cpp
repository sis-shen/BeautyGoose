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
    //初始化信号连接
    initAccountResponseConnection();
    initConsumerResponeConnection();
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
    auto datacenter = DataCenter::getInstance();
    int type = 0;
    if(input.type == "CONSUMER")type = (int)data::Account::Type::CONSUMER;
    else if(input.type == "MERCHANT")type = (int)data::Account::Type::MERCHANT;
    else if(input.type == "ADMIN")type = (int)data::Account::Type::ADMIN;
    datacenter->accountRegisterAsync(input.name,input.password,input.phone,input.nickname,input.auth_code,type);
}

void MainWidget::loginNameSlot(LoginByNameWidget::Input input)
{
    DataCenter* datacenter = DataCenter::getInstance();
    datacenter->loginByNameAsync(input.name,input.password);
}

void MainWidget::loginPhoneSlot(LoginByPhoneWidget::Input input)
{
    //TODO
    toConsumerDishListSlot();
}

void MainWidget::toConsumerUserInfoSlot()
{
    DataCenter* datacenter = DataCenter::getInstance();
    clearAll();
    ConsumerUserInfoWidget* aui = new ConsumerUserInfoWidget(datacenter->account);
    this->layout()->addWidget(aui);
    //导航栏
    connect(aui->leftNavW,&ConsumerNavWidget::toUesrInfoSignal,this,&MainWidget::toConsumerUserInfoSlot);
    connect(aui->leftNavW,&ConsumerNavWidget::toCartListSignal,this,&MainWidget::toCartListSlot);
    connect(aui->leftNavW,&ConsumerNavWidget::toDishListSignal,this,&MainWidget::toConsumerDishListSlot);
    connect(aui->leftNavW,&ConsumerNavWidget::toOrderListSignal,this,&MainWidget::toConsumerOrderListSlot);
    connect(aui->leftNavW,&ConsumerNavWidget::toVIPSignal,this,&MainWidget::toVIPSlot);
    //其它
    connect(aui,&ConsumerUserInfoWidget::changeNicknameSignal,this,&MainWidget::changeNicknameSlot);
}

void MainWidget::toMerchantUserInfoSlot()
{
    qDebug()<<"toMerchantUserInfoSlot";
    DataCenter* datacenter = DataCenter::getInstance();
    clearAll();
    MerchantUserInfoWidget* aui = new MerchantUserInfoWidget(datacenter->account);
    this->layout()->addWidget(aui);
    qDebug()<<"start Connection";
    //连导航栏
    connect(aui->leftNavW,&MerchantNavWidget::toUserInfoSignal,this,&MainWidget::toMerchantUserInfoSlot);
    connect(aui->leftNavW,&MerchantNavWidget::toDishListSignal,this,&MainWidget::toMerchantDishListSlot);
    connect(aui->leftNavW,&MerchantNavWidget::toOrderListSignal,this,&MainWidget::toMerchantOrderListSlot);
    connect(aui->leftNavW,&MerchantNavWidget::toDishRegisterWindowSignal,this,&MainWidget::toDishRegisterWindowSlot);
}

void MainWidget::changeNicknameSlot(QString nickname)
{
    DataCenter* datacenter = DataCenter::getInstance();
    datacenter->accountChangeNicknameAsync(nickname);
}

void MainWidget::initAccountResponseConnection()
{
///////////////////////////////////
/// HTTP响应
///////////////////////////////////
    auto datacenter = DataCenter::getInstance();
    connect(datacenter,&DataCenter::getRegisterDone,this,[=](bool ok,const QString&reason){
        if(ok)
        {
            QMessageBox::information(this, "Info", "注册成功！");
            qDebug()<<"注册成功";
            toNameLoginSlot();  //转到登录页面
        }
        else{
            QMessageBox::information(this, "Info", "注册失败！原因:" + reason);
            qDebug()<<"注册失败";
            // toRegisterSlot();   //刷新界面
        }
    });

    connect(datacenter,&DataCenter::getLoginByNameDone,this,[=](bool ok,const QString&reason){
        qDebug()<<"登录成功";
        if(ok)
        {
            if(datacenter->account->type == data::Account::Type::CONSUMER)
            {
                toConsumerDishListSlot();
            }
            else if(datacenter->account->type == data::Account::Type::MERCHANT)
            {
                toMerchantDishListSlot();
            }
            else if(datacenter->account->type == data::Account::Type::ADMIN)
            {
                toAdminOrderListSlot();
            }
            else
            {
                qDebug()<<"账户类型错误";
                Q_ASSERT(false);
            }
        }
        else
        {
            QMessageBox::information(this, "Info", "登录失败!原因:"+reason);
            toNameLoginSlot();  //刷新界面
        }
    });

    connect(datacenter,&DataCenter::getLoginByPhoneDone,this,[=](bool ok,const QString&reason){
        if(ok)
        {
            if(datacenter->account->type == data::Account::Type::CONSUMER)
            {
                toConsumerDishListSlot();
            }
            else if(datacenter->account->type == data::Account::Type::MERCHANT)
            {
                toMerchantDishListSlot();
            }
            else if(datacenter->account->type == data::Account::Type::ADMIN)
            {
                toAdminOrderListSlot();
            }
            else
            {
                qDebug()<<"账户类型错误";
                Q_ASSERT(false);
            }
        }
        else
        {
            QMessageBox::information(this, "Info", "登录失败!原因:"+reason);
            toPhoneLoginSlot();  //刷新界面
        }
    });

    connect(datacenter,&DataCenter::accountChangeNicknameAsyncDone,this,[&](){
        //刷新页面
        if(datacenter->account->type == data::Account::CONSUMER)
            {
            toConsumerUserInfoSlot();
        }
        else
            {
            toMerchantUserInfoSlot();
        }
    });
}



void MainWidget::toConsumerDishListSlot()
{
    DataCenter* datacenter = DataCenter::getInstance();
    datacenter->consumerGetDishListAsync();//它最后会发送完成信号getDishListDone
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

    DataCenter* datacenter = DataCenter::getInstance();
    datacenter->consumerGetDishInfoAsync(dish_id);
}

void MainWidget::cartDsihAddSlot(QString merchant_id, QString dish_id)
{
    DataCenter* datacenter = DataCenter::getInstance();
    auto table = datacenter->cart_list->cart_table;
    //找不到到商家
    if(table->find(merchant_id) == table->end())
    {
        qDebug()<<"新增商家"<<merchant_id;
        table->insert(merchant_id,Cart::ptr(new Cart));

        Cart::ptr cart = table->value(merchant_id);
        cart->dish_table = new QHash<QString,CartDishItem::ptr>;
        cart->merchant_id =merchant_id;
        cart->merchant_name =datacenter->dish->merchant_name;
        cart->pay = 0;
        cart->cnt =0;
    }
    //找到商家对应的菜品表
    auto dish_table = (*(table->find(merchant_id)))->dish_table;
    Cart::ptr cart = *(table->find(merchant_id));
    if(dish_table->find(dish_id) != dish_table->end())
    {
        auto dish_item = *(dish_table->find(dish_id));
        cart->cnt++;
        cart->pay+=dish_item->dish_price;
        // qDebug()<<dish_item;
        // qDebug()<<"0"<<dish_item->dish_name<<":"<<dish_item->cnt;
        dish_item->cnt++;
        // qDebug()<<"1"<<dish_item->dish_name<<":"<<dish_item->cnt;
        dish_item->pay += dish_item->dish_price;
        if(cdd_win != nullptr)
        {
            cdd_win->cdd->setCnt(dish_item->cnt);//更新数量
        }
    }
    else
    {
        //新表
        CartDishItem::ptr dish_item = CartDishItem::ptr(new CartDishItem);
        dish_table->insert(dish_id,dish_item);
        dish_item->dish_id = dish_id;
        dish_item->dish_name = datacenter->dish->name;
        dish_item->dish_price = datacenter->dish->base_price * datacenter->dish->price_factor;
        dish_item->cnt = 1;
        dish_item->pay = dish_item->dish_price;
        dish_table->insert(dish_id,dish_item);

        cart->cnt++;
        cart->pay +=dish_item->dish_price;
        if(cdd_win != nullptr)
        {
            qDebug()<<"new"<<dish_item->dish_name<<":"<<dish_item->cnt;
            cdd_win->cdd->setCnt(dish_item->cnt);//更新数量
        }
    }
}

void MainWidget::cartDsihPopSlot(QString merchant_id, QString dish_id)
{
    DataCenter* datacenter = DataCenter::getInstance();
    auto table = datacenter->cart_list->cart_table;
    if(table->find(merchant_id) == table->end())
    {
        //找不到就直接退出
        return;
    }

    auto dish_table = (table->find(merchant_id)).value()->dish_table;
    if(dish_table->find(dish_id) != dish_table->end())
    {
        auto dish_item = dish_table->value(dish_id);
        if(dish_item->cnt < 1)
        {
            //数量不足以继续减少
            return;
        }
        dish_item->cnt--;
        dish_item->pay -= dish_item->dish_price;
        Cart::ptr cart = table->value(merchant_id);
        cart->cnt--;
        cart->pay-=dish_item->dish_price;
        if(cdd_win != nullptr)
        {
            cdd_win->cdd->setCnt(dish_item->cnt);//更新数量
        }
    }
    else
    {
        //菜品不存在也退出
        return;
    }
}


void MainWidget::toCartListSlot()
{
    clearAll();
    DataCenter* datacenter = DataCenter::getInstance();
    ConsumerCartListWidget* ccl = new ConsumerCartListWidget(datacenter->cart_list->cart_table);
    this->layout()->addWidget(ccl);

    //连导航栏
    connect(ccl->leftNavW,&ConsumerNavWidget::toUesrInfoSignal,this,&MainWidget::toConsumerUserInfoSlot);
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
    DataCenter* datacenter = DataCenter::getInstance();
    auto table = datacenter->cart_list->cart_table;
    Q_ASSERT(table);
    if(table->find(merchant_id) == table->end() )return;//判空
    table->erase(table->find(merchant_id));
    toCartListSlot();//刷新界面
}

void MainWidget::orderGenerateSlot(QString merchant_id)
{
    DataCenter* datacenter = DataCenter::getInstance();
    datacenter->consumerOrderGenerateAsync(merchant_id);
    toConsumerOrderListSlot();
}

void MainWidget::toConsumerOrderListSlot()
{
    clearAll();
    DataCenter* datacenter = DataCenter::getInstance();
    ConsumerOrderListWidget* col = new ConsumerOrderListWidget(datacenter->order_table);
    this->layout()->addWidget(col);
    //连导航栏
    connect(col->leftNavW,&ConsumerNavWidget::toUesrInfoSignal,this,&MainWidget::toConsumerUserInfoSlot);
    connect(col->leftNavW,&ConsumerNavWidget::toCartListSignal,this,&MainWidget::toCartListSlot);
    connect(col->leftNavW,&ConsumerNavWidget::toDishListSignal,this,&MainWidget::toConsumerDishListSlot);
    connect(col->leftNavW,&ConsumerNavWidget::toOrderListSignal,this,&MainWidget::toConsumerOrderListSlot);
    connect(col->leftNavW,&ConsumerNavWidget::toVIPSignal,this,&MainWidget::toVIPSlot);

    //连其它信号
    connect(col,&ConsumerOrderListWidget::toOrderInfoSignal,this,&MainWidget::consumerOrderInfoSlot);
}

void MainWidget::consumerOrderInfoSlot(QString order_id)
{
    DataCenter* datacenter = DataCenter::getInstance();
    datacenter->consumerGetOrderInfoAsync(order_id);
}


void MainWidget::toVIPSlot()
{
    clearAll();
    ConsumerVIPWidget* vip = new ConsumerVIPWidget;
    this->layout()->addWidget(vip);
    //导航栏
    //连导航栏
    connect(vip->leftNavW,&ConsumerNavWidget::toUesrInfoSignal,this,&MainWidget::toConsumerUserInfoSlot);
    connect(vip->leftNavW,&ConsumerNavWidget::toCartListSignal,this,&MainWidget::toCartListSlot);
    connect(vip->leftNavW,&ConsumerNavWidget::toDishListSignal,this,&MainWidget::toConsumerDishListSlot);
    connect(vip->leftNavW,&ConsumerNavWidget::toOrderListSignal,this,&MainWidget::toConsumerOrderListSlot);
    connect(vip->leftNavW,&ConsumerNavWidget::toVIPSignal,this,&MainWidget::toVIPSlot);
    //控件信号
    connect(vip,&ConsumerVIPWidget::accountUpgradeSignal,this,&MainWidget::accountUpgradeSlot);
}

void MainWidget::accountUpgradeSlot(QString level)
{

}

void MainWidget::payOrderSlot(QString order_id)
{
    if( tpw_win!= nullptr)
    {
        return;//不允许同时打开多个窗口
    }
    tpw_win = new ThirdPayWindow;
    tpw_win->order_id=order_id;
    //连接内部信号
    connect(tpw_win,&ThirdPayWindow::payConfirmlySignal,this,&MainWidget::payConfirmlySlot);
    //连接窗口关闭信号
    connect(tpw_win,&ThirdPayWindow::finished,this,&MainWidget::tpwCloseSlot);

    tpw_win->exec();
}

void MainWidget::payConfirmlySlot(QString order_id)
{

}

void MainWidget::tpwCloseSlot()
{
    qDebug()<<"close mod window";
    sender()->deleteLater();
    tpw_win = nullptr;
}

void MainWidget::orderCancelSlot(QString order_id)
{

}

void MainWidget::initConsumerResponeConnection()
{
    DataCenter* datacenter = DataCenter::getInstance();

    //获取菜品列表
    connect(datacenter,&DataCenter::consumerGetDishListDone,this,[=](){

        //构建ConsumerDishList界面
        // qDebug()<<"开始构建ConsumerDishList界面";
        // qDebug()<<(datacenter->dish_list_table->size());
        // qDebug()<<"访问成功";
        ConsumerDishListWidget* cdl = new ConsumerDishListWidget(datacenter->dish_list_table);
        // qDebug()<<"构建成功";
        clearAll();
        this->layout()->addWidget((cdl));
        //连导航栏
        connect(cdl->leftNavW,&ConsumerNavWidget::toUesrInfoSignal,this,&MainWidget::toConsumerUserInfoSlot);
        connect(cdl->leftNavW,&ConsumerNavWidget::toCartListSignal,this,&MainWidget::toCartListSlot);
        connect(cdl->leftNavW,&ConsumerNavWidget::toDishListSignal,this,&MainWidget::toConsumerDishListSlot);
        connect(cdl->leftNavW,&ConsumerNavWidget::toOrderListSignal,this,&MainWidget::toConsumerOrderListSlot);
        connect(cdl->leftNavW,&ConsumerNavWidget::toVIPSignal,this,&MainWidget::toVIPSlot);

        //连其它信号
        connect(cdl,&ConsumerDishListWidget::dishInfoSignal,this,&MainWidget::consumerDishInfoSlot);
    });

    //菜品详情
    connect(datacenter,&DataCenter::consumergetDishInfoDone,this,[=](){
        //若不存在，则创建窗口并显示
        qDebug()<<"开始构建详情窗口";
        auto dish = datacenter->dish;
        cdd_win =new ConsumerDishDetailWindow(dish,datacenter->getCartDishNum(dish->merchant_id,dish->uuid));
        //连接窗口信号
        connect(cdd_win,&ConsumerDishDetailWindow::finished,this,&MainWidget::cddCloseSlot);
        //连接控件信号
        connect(cdd_win->cdd,
                &ConsumerDishDetailWidget::cartDishAddSignal,
                this,
                &MainWidget::cartDsihAddSlot);
        connect(cdd_win->cdd,
                &ConsumerDishDetailWidget::cartDishPopSignal,
                this,
                &MainWidget::cartDsihPopSlot);

        cdd_win->exec();
    });
    //订单生成
    connect(datacenter,&DataCenter::consumerOrderGenerateDone,this,[=](){
        toConsumerOrderListSlot();
    });

    //订单详情
    connect(datacenter,&DataCenter::consumerGetOrderInfoDone,this,[=](){
        clearAll();
        ConsumerOrderDetailWidget* cod = new ConsumerOrderDetailWidget(datacenter->consumer_order_item);
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
    });
}

void MainWidget::toMerchantDishListSlot()
{
    clearAll();
    MerchantDishListWidget* mdl = new MerchantDishListWidget;
    this->layout()->addWidget(mdl);

    //连导航栏
    connect(mdl->leftNavW,&MerchantNavWidget::toUserInfoSignal,this,&MainWidget::toMerchantUserInfoSlot);
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
    connect(mol->leftNavW,&MerchantNavWidget::toUserInfoSignal,this,&MainWidget::toMerchantUserInfoSlot);
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

void MainWidget::merchantDishEditSlot(QString dish_id)
{
    if(mde_win != nullptr)
    {
        return;//不允许同时打开多个窗口
    }
    mdd_win->close();//编辑菜品时关闭详情页
    mde_win = new MerchantDishEditWindow;
    //连接内部信号
    connect(mde_win->mde,&MerchantDishEditWidget::merchantDishSaveSignal,this,&MainWidget::merchantDishEditSaveSlot);
    connect(mde_win->mde,&MerchantDishEditWidget::merchantDishDelSignal,this,&MainWidget::merchantDishDelSlot);
    //连接窗口关闭信号
    connect(mde_win,&MerchantDishRegisterWindow::finished,this,&MainWidget::mdeCloseSlot);

    mde_win->exec();
}


void MainWidget::mdeCloseSlot()
{
    qDebug()<<"close mod window";
    sender()->deleteLater();
    mde_win = nullptr;
}

void MainWidget::merchantDishEditSaveSlot(MerchantDishEditWidget::Input input)
{

}

void MainWidget::merchantDishDelSlot(QString dish_id)
{

}

void MainWidget::toAdminOrderListSlot()
{
    clearAll();
    AdminOrderListWidget* aol = new AdminOrderListWidget;
    this->layout()->addWidget(aol);
}





