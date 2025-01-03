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
signals:
};
}
#endif // NETCLIENT_H
