#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include "registerwidget.h"
#include "datacenter.h"
#include "loginbynamewidget.h"
#include "loginbyphone.h"
#include "consumerdishlistwidget.h"
#include "consumercartlistwidget.h"
#include "consumerorderlistwidget.h"
#include "consumerorderdetailwidget.h"
#include "consumerdishdetailwindow.h"
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
private:
//Account模块
    void initRegisterConnection(RegisterWidget*);
public slots:
//Account模块
    void toRegisterSlot();          //转到注册界面
    void toNameLoginSlot();         //转到用户名登录界面
    void toPhoneLoginSlot();        //转到电话号码登录界面
    void authCodeSlot();            //获取短信验证码
    void registerSlot(RegisterWidget::Input input); //发起注册请求
    void loginNameSlot(LoginByNameWidget::Input input);//发起登录请求
    void loginPhoneSlot(LoginByPhoneWidget::Input input);//发起登录请求

public:
//消费者子系统
    ConsumerDishDetailWindow* cdd_win = nullptr;      //显示菜品详情的唯一窗口
public slots:
    void toDishListSlot();              //转到菜品列表界面
    void cddCloseSlot();                //关闭窗口时执行的槽函数
    void dishInfoSlot(QString dish_id); //转到转到菜品详情窗口
    void cartDsihAddSlot(QString merchant_id,
                         QString dish_id);  //购物车添加一个菜品
    void cartDsihPopSlot(QString merchant_id,
                         QString dish_id);  //购物车减少一个菜品
    void toCartListSlot();              //转到购物车列表界面
    void cartClearSlot(QString merchant_id);    //清空某个购物车
    void orderGenerateSlot(QString merchant_id);//生成订单
    void toOrderListSlot();             //转到订单列表界面
    void orderInfoSlot(QString order_id);   //转到订单详情界面
    void toVIPSlot();                   //转到VIP界面
    void payOrderSlot(QString order_id);//支付某个订单
    void orderCancelSlot(QString order_id);//取消某个订单
private:
    QString host = "127.0.0.1";
    int port = 80;
    Ui::MainWidget *ui;
    DataCenter dc;
};
#endif // MAINWIDGET_H
