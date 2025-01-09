#ifndef NETCLIENT_H
#define NETCLIENT_H

#include <QObject>
#include <QString>
#include <QUrl>
#include <QWebSocket>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include "DataCenterCoreData.h"

//提前声明
class DataCenter;

namespace network
{
class NetClient : public QObject
{
    Q_OBJECT
public:
    explicit NetClient(DataCenter* datacenter,const QString& hUrl,const QString& sUrl);

    DataCenter* datacenter;
    QString httpUrl;
    QString sockUrl;

    //http客户端
    QNetworkAccessManager httpClient;
    //socket客户端
    QWebSocket websocktClient;



//网络接口
    void ping();

public:
/////////////////////////////////////////
/// Account子系统
/////////////////////////////////////////
    void accountRegister(const QString&name,const QString&password,const QString& phone,
                         const QString&nickname,const QString auth_code ,int type);
    void accountLoginByName(const QString&name,const QString&password);
    void accountUpdateLevel(const QString&id,const QString& level);
/////////////////////////////////////////
/// Consumer子系统
/////////////////////////////////////////
    void consumerGetDishList();
    void consumerGetDishInfo(const QString& dish_id);
    void consumerOrderGenerate(const QString& merchant_id);
    void consumerGetOrderList(const QString&consumer_id);
    void consumerGetOrdrerDishList(const QString&order_id);
    void consumerOrderPayConfirm(const QString&order_id);
/////////////////////////////////////////
/// Merchant子系统
/////////////////////////////////////////
    void merchantGetDishList(const QString&merchant_id);
    void merchantDishRegister(const QString& name,const QString&link,
                                   double price,double price_factor = 1,
                              const QString& introduction = "");
    void merchantDishEditSave(const QString&dish_id,const QString& name,const QString&link,
                              double price,double price_factor = 1,
                              const QString& introduction = "");
    void merchantGetDishInfo(const QString& dish_id);
    void merchantDishEditDel(const QString&dish_id);
    void merchantGetOrderList(const QString&merchant_id);
    void merchantGetOrderDishList(const QString&order_id);

/////////////////////////////////////////
/// Admin 子系统
/////////////////////////////////////////
    void adminGetOrderList();
signals:
};
}
#endif // NETCLIENT_H
