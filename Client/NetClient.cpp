#include "NetClient.h"

namespace network
{

NetClient::NetClient(DataCenter *datacenter, const QString &hUrl, const QString &sUrl)
    :datacenter(datacenter),httpUrl(hUrl),sockUrl(sUrl)
{
    qDebug()<<"ping";
    ping();
}

void NetClient::ping()
{
    QNetworkRequest httpReq;
    httpReq.setUrl(httpUrl+"/ping");

    QNetworkReply* httpRes = httpClient.get(httpReq);
    connect(httpRes,&QNetworkReply::finished,this,[=](){
        //这里面的槽函数说明响应已经来了
        if(httpRes->error() == QNetworkReply::NoError)
        {
            //请求失败
            qDebug()<<"HTTP请求失败:"<<httpRes->errorString();
        }
        //获取响应到的body
        QByteArray body = httpRes->readAll();
        qDebug()<<"响应内容"<<body;
    });
}

void NetClient::accountRegister(const QString &name, const QString &password, const QString &phone, const QString &nickname, const QString auth_code, int type)
{
    // 1. 构造 JSON 数据
    QJsonObject jsonObj;
    jsonObj["name"] = name;
    jsonObj["password"] = password;  // 直接使用传入的已加密密码
    jsonObj["phone"] = phone;
    jsonObj["nickname"] = nickname;
    jsonObj["auth_code"] = auth_code;
    jsonObj["type"] = type;

    // 将 JSON 对象转换为字符串
    QJsonDocument doc(jsonObj);
    QByteArray jsonData = doc.toJson();

    // 2. 创建 HTTP 请求并设置 URL 和请求头
    QUrl url(httpUrl + "/account/register");  // 注册接口是 /account/register
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json;charset=UTF-8");

    // 3. 发送 POST 请求
    QNetworkReply *reply = httpClient.post(request, jsonData);

    // 4. 连接信号和槽以处理响应
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            // 处理服务器返回的数据
            QByteArray responseData = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(responseData);
            if (!doc.isNull()) {
                QJsonObject responseObj = doc.object();
                bool success = responseObj["success"].toBool();  // 假设响应中包含 success 字段
                if (success) {
                    qDebug() << "Account registration successful!";
                    // 可根据需求进一步处理成功结果
                } else {
                    QString errorMessage = responseObj["message"].toString();
                    qDebug() << "Account registration failed: " << errorMessage;
                    // 处理失败的情况
                }
            } else {
                qDebug() << "Invalid JSON response!";
            }
        } else {
            qDebug() << "Request failed: " << reply->errorString();
        }

        // 释放回复对象
        reply->deleteLater();
    });
}

}
