#ifndef DATACENTER_H
#define DATACENTER_H
#include "CoreData.h"
#include "DataCenterCoreData.h"
#include "NetClient.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

#include <QCryptographicHash>


//网络
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>

#include <QFile>
#include <QObject>
#include <QIcon>
#include <QPointer>

using namespace btyGoose;
class DataCenter:public QObject
{
    Q_OBJECT;
public:
    //单例模式化
    static DataCenter* getInstance()
    {
        if(datacenter == nullptr)
        {
            datacenter = new DataCenter;
        }

        return datacenter;
    }
private:
    DataCenter();
    static DataCenter* datacenter;
    DataCenter(const DataCenter&) = delete;
    DataCenter operator=(const DataCenter&) =delete;
private:
    network::NetClient* client = nullptr;
public:
    //工具函数

    QList<data::Dish> DishListFromJsonArray(const QString& jsonString);
    QString DishListToJsonArray(const QList<data::Dish>& dishList);
    //处理短链连接的http请求
    QString configFileName = "ServerConfig.json";
    QString httpUrl = "http://127.0.0.1:80";
    QString sockUrl = "127.0.0.1:8001/websock";
    bool loadConfig();

public:
//////////////////////
///账户子系统
////////////////////////
/// 核心数据缓存
    data::Account* account = nullptr;
    //接口
    bool getAuthcode(const QString&phoneNumber);
    void accountRegisterAsync(const QString&name,const QString&password,const QString& phone,
                        const QString&nickname,const QString auth_code ,int type);
    void loginByNameAsync(const QString&name,const QString&password);
    void loginByPhoneAsync(const QString&phone,const QString&password);
    void accountUpdateAsync();
    void accountChangeNicknameAsync(const QString&nickname);
public:
    QIcon QRCode;
    //第三方支付子系统
    bool getQRCode();
    void PaySuccessSignal();
    void PayFailSignal();
public:
//////////////////////
///消费者子系统
////////////////////////
//缓存数据
    QHash<QString,QList<data::Dish>>* dish_list_table = nullptr;
    data::Dish* dish = nullptr;
    CartList* cart_list = nullptr;
    // ConsumerOrderList* consumer_order_list =nullptr;
    ConsumerOrderItem* consumer_order_item = nullptr;
    QHash<QString,data::Order>* order_table = nullptr;//order_id -> order
//接口
    void consumerGetDishListAsync();    //用户数据在本对象的account里
    void consumerGetDishInfoAsync(const QString&dihs_id);
    void consumerOrderGenerateAsync(const QString&merchant_id);//得知道生成的是谁的
    void consumerGetOrderInfoAsync(const QString&order_id);
    int getCartDishNum(const QString&merchant_id,const QString&dish_id);

public:
//////////////////////
///商家子系统
////////////////////////
/// 数据缓存
    QHash<QString,data::Dish>* merchant_dish_table;
    QHash<QString,data::Order>* merchant_order_table;
//接口
    void merchantGetDishListAysnc();
    void merchantDishRegisterAsync(const QString& name,const QString&link,
                                   double price,double price_factor = 1,
                                   const QString& introduction = "");
    //编辑菜品保存
    void merchantDishEditSaveAsync(const QString&dish_id,const QString& name,const QString&link,
                                   double price,double price_factor = 1,
                                   const QString& introduction = "");
    void merchantGetDishInfoAsync(const QString&dish_id);
    void merchantDishEditDelAsync(const QString&dish_id);
    void merchantGetOrderListAsync();

    QList<data::Order> OrderListFromJsonArray(const QString& jsonString);
signals:
///////////////////////////////////
///账户子系统
//////////////////////////////////
    void getAccountDone();
    void getRegisterDone(bool ok,const QString& reason);
    void getLoginByNameDone(bool ok,const QString& reason);
    void getLoginByPhoneDone(bool ok,const QString& reason);
    void accountChangeNicknameAsyncDone();

///////////////////////////////////
///消费者子系统
//////////////////////////////////

    void consumerGetDishListDone();
    void consumergetDishInfoDone();
    void consumerOrderGenerateDone();
    void consumerGetOrderInfoDone();
///////////////////////////////////
///商家者子系统
//////////////////////////////////
    void merchantGetDishListDone();
    void merchantDishRegisterDone(bool ok,const QString&reason);
    void merchantGetDishInfoDone();
    void merchantDishEditSaveDone(bool ok,const QString&reason);
    void merchantDishEditDelDone();
    void merchantGetOrderListDone();
};


#endif // DATACENTER_H
