#ifndef DATACENTER_H
#define DATACENTER_H
#include "CoreData.h"
#include "DataCenterCoreData.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

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
    DataCenter(const QString& host = "127.0.0.1",int port = 80);

     client;     //处理短链连接的http请求
    QString configFileName = "ServerConfig.json";
    QString host = "127.0.0.1";
    int port = 80;
    bool loadConfig();

public:
//////////////////////
///账户子系统
////////////////////////
/// 核心数据缓存
    data::Account* account = nullptr;
    //接口
    bool getAuthcode(const QString&phoneNumber);
    bool registerAccount();
    bool loginByName();
    bool loginByPhone();
    bool accountUpdate();
public:
    QIcon QRCode;
    //第三方支付子系统
    bool getQRCode();
    void PaySuccessSignal();
    void PayFailSignal();
public:
    //消费者子系统
    DishList dish_list;
    const DishList& getConsumerDishList();
    data::Dish getDish();
    CartList cart_list;
    bool consumerCartAdd();
    bool consumerCartPop();
    OrderList order_list;
    bool consumerOrderGenerate();

};

#endif // DATACENTER_H
