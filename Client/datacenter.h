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
    bool loginByNameAsync(const QString&name,const QString&password);
    bool loginByPhoneAsync();
    bool accountUpdateAsync();
public:
    QIcon QRCode;
    //第三方支付子系统
    bool getQRCode();
    void PaySuccessSignal();
    void PayFailSignal();
public:
    //消费者子系统
//缓存数据
    DishList* dish_list = nullptr;
    data::Dish* dish = nullptr;
    CartList* cart_list = nullptr;
    ConsumerOrderList* consumer_order_list =nullptr;
    data::Order* data_order = nullptr;
//接口
    void consumerGetDishListAsync();    //用户数据在本对象的account里
    void consumerGetDishInfoAsync(const QString&dihs_id);
    bool consumerCartAdd(const QString merchant_id,const QString dish_id);
    bool consumerCartPop(const QString merchant_id,const QString& dish_id);
    bool consumerOrderGenerate();

signals:
///////////////////////////////////
///账户子系统
//////////////////////////////////
    void getAccountDone();
    void getRegisterDone(bool ok,const QString& reason);
    void getLoginByNameDone(bool ok,const QString& reason);
    void getLoginByPhoneDone(bool ok,const QString& reason);

///////////////////////////////////
///消费者子系统
//////////////////////////////////

    void getDishListDone();
};


#endif // DATACENTER_H
