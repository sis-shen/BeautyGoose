#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include "datacenter.h"
//账户界面
#include "registerwidget.h"
#include "loginbynamewidget.h"
#include "loginbyphone.h"
#include "consumeruserinfowidget.h"
#include "merchantuserinfowidget.h"
//消费者界面
#include "consumerdishlistwidget.h"
#include "consumercartlistwidget.h"
#include "consumerorderlistwidget.h"
#include "consumerorderdetailwidget.h"
#include "consumerdishdetailwindow.h"
#include "consumervipwidget.h"
//商家界面
#include "merchantdishlistwidget.h"
#include "merchantorderlistwidget.h"
#include "merchantdishregisterwindow.h"
#include "merchantorderdetailwindow.h"
#include "merchantdishdetailwindow.h"
#include "merchantdisheditwindow.h"
//管理员界面
#include "adminorderlistwidget.h"
//第三方支付界面
#include "thirdpaywindow.h"

#include "ColorConfig.h"

#include <QMessageBox>
#include <QList>
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWidget;
}
QT_END_NAMESPACE

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget *parent = nullptr);
    ~MainWidget();
private:
    //控件操作
    void clearAll();

    enum Status{
        Register,
        LoginByName,
        LoginByPhone,
        UserInfo,
        DishList,
        OrderList,
        CartList,
        VIP,
        HistoryList
    };

    Status state;
private:
//Account模块
public slots:
//Account模块
    void toRegisterSlot();          //转到注册界面
    void toNameLoginSlot();         //转到用户名登录界面
    void toPhoneLoginSlot();        //转到电话号码登录界面
    void authCodeSlot();            //获取短信验证码
    void registerSlot(RegisterWidget::Input input); //发起注册请求
    void loginNameSlot(LoginByNameWidget::Input input);//发起登录请求
    void loginPhoneSlot(LoginByPhoneWidget::Input input);//发起登录请求
    void toConsumerUserInfoSlot();  //转到消费者个人界面
    void toMerchantUserInfoSlot();  //转到商家个人界面
    void changeNicknameSlot(QString nickname);      //修改昵称

    void initAccountResponseConnection();   //使用lambda表达式配合信号槽处理所有的响应
public:
//消费者子系统
    ConsumerDishDetailWindow* cdd_win = nullptr;      //显示菜品详情的唯一窗口
    ThirdPayWindow* tpw_win = nullptr;              //第三方支付窗口
public slots:
    void toConsumerDishListSlot();              //转到菜品列表界面
    void cddCloseSlot();                //关闭消费者菜品详情窗口时执行的槽函数
    void consumerDishInfoSlot(QString dish_id); //转到转到菜品详情窗口
    void cartDsihAddSlot(QString merchant_id,
                         QString dish_id);  //购物车添加一个菜品
    void cartDsihPopSlot(QString merchant_id,
                         QString dish_id);  //购物车减少一个菜品
    void toCartListSlot();              //转到购物车列表界面
    void cartClearSlot(QString merchant_id);    //清空某个购物车
    void orderGenerateSlot(QString merchant_id);//生成订单
    void toConsumerOrderListSlot();             //转到订单列表界面
    void consumerOrderInfoSlot(QString order_id);   //转到订单详情界面
    void toVIPSlot();                   //转到VIP界面
    void accountUpgradeSlot(QString level);        //用户升级实现
    void payOrderSlot(QString order_id);//支付某个订单
    void payConfirmlySlot(QString order_id);//真正开始支付某个订单
    void tpwCloseSlot();
    void consumerOrderCancelSlot(QString order_id);//取消某个订单

    void initConsumerResponeConnection();
public:
    //商家子系统
    MerchantDishRegisterWindow* mdr_win = nullptr;    //商家注册菜品窗口
    MerchantOrderDetailWindow* mod_win = nullptr;       //商家订单处理窗口
    MerchantDishDetailWindow* mdd_win = nullptr;        //商家查看菜品详情
    MerchantDishEditWindow* mde_win = nullptr;      //商家编辑菜品窗口
public slots:
    void toMerchantDishListSlot();  //转到商家的菜品列表
    void toMerchantDishInfoWindowSlot(QString dish_id);    //转到商家菜品详情窗口
    void toMerchantOrderListSlot();//前往商家订单列表
    void toMerchantOrderDetailSlot(const QString& order_id);   //商家查看订单详情
    void toDishRegisterWindowSlot();    //打开商品注册窗口
    void mdrCloseSlot();
    void merchantDishRegisterSlot(MerchantDishRegisterWidget::Input input);
    void modCloseSlot();        //订单处理窗口关闭处理函数
    void merchantOrderAcceptSlot(QString order_id);  //商家接单处理函数
    void merchantOrderRejecttSlot(QString order_id);  //商家接单处理函数
    void mddCloseSlot();        //商家菜品详情窗口关闭处理函数
    void merchantDishEditSlot(QString dish_id);//商家菜品编辑窗口
    void mdeCloseSlot();        //商家菜品编辑窗口关闭处理函数
    void merchantDishEditSaveSlot(MerchantDishEditWidget::Input input);
    void merchantDishDelSlot(QString dish_id);

    void initMerchantResponseConnection();
public:
    //管理员子系统
    void toAdminOrderListSlot();    //打开管理员界面

    void initAdminResponseConnection();
private:
    QString host = "127.0.0.1";
    int port = 80;
    Ui::MainWidget *ui;
};
#endif // MAINWIDGET_H
