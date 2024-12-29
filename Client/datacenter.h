#ifndef DATACENTER_H
#define DATACENTER_H
#include "CoreData.h"
#include "DataCenterCoreData.h"
#include "httplib.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QObject>
#include <QIcon>
using namespace btyGoose;
class DataCenter:public QObject
{
    Q_OBJECT;
public:
    DataCenter(const QString& host = "127.0.0.1",int port = 80);
public:
    //账户子系统
    data::Account account;
    QString getAuthcode(const QString&phoneNumber);
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


private:
    httplib::Client client;
};

#endif // DATACENTER_H
