#include "NetClient.h"
#include "datacenter.h"
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
                QString Message = responseObj["message"].toString();
                if (success) {
                    qDebug() << "Account registration successful!";
                } else {
                    QString errorMessage = responseObj["message"].toString();
                    qDebug() << "Account registration failed: " << errorMessage;
                    // 处理失败的情况
                }
                emit datacenter->getRegisterDone(success,Message);
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

void NetClient::accountLoginByName(const QString &name, const QString &password)
{
    // 1. 构造 JSON 数据
    QJsonObject jsonObj;
    jsonObj["name"] = name;
    jsonObj["password"] = password;  // 直接使用传入的已加密密码

    // 将 JSON 对象转换为字符串
    QJsonDocument doc(jsonObj);
    QByteArray jsonData = doc.toJson();

    // 2. 创建 HTTP 请求并设置 URL 和请求头
    QUrl url(httpUrl + "/account/login/username");  //
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
                QString Message = responseObj["message"].toString();
                if (success) {
                    qDebug() << "Account login by name successful!";
                    QString accJson = responseObj["account"].toString();
                    data::Account* acc = new data::Account;
                    acc->loadFromJson(accJson.toStdString());
                    datacenter->account = acc;//向数据中心存入账户信息
                } else {
                    QString errorMessage = responseObj["message"].toString();
                    qDebug() << "Account login by name failed: " << errorMessage;
                    // 处理失败的情况
                }
                emit datacenter->getLoginByNameDone(success,Message);
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

void NetClient::consumerGetDishList()
{
    // 2. 创建 HTTP 请求并设置 URL 和请求头
    QUrl url(httpUrl + "/consumer/dish/list");  //
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json;charset=UTF-8");

    // 3. 发送 POST 请求
    QNetworkReply *reply = httpClient.post(request,"");//没有报文

    // 4. 连接信号和槽以处理响应
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            // 处理服务器返回的数据
            QByteArray responseData = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(responseData);
            QJsonObject responseObj = doc.object();
            // qDebug()<<responseData;
            auto list  = datacenter->DishListFromJsonArray(responseObj["dish_list"].toString());
            // qDebug()<<"获取到的菜品数量:"<<list.size();
            auto table = datacenter->dish_list_table;
            if(!table->empty())
            {
                table->clear();
            }
            for(const auto&dish:list)
            {

                if(table->find(dish.merchant_id) == table->end())
                {
                    // qDebug()<<"新增了一个商家";
                    table->insert(dish.merchant_id,QList<data::Dish>());
                }
                auto it = table->find(dish.merchant_id);
                (*it).append(dish);
                // qDebug()<<dish.merchant_name;
                // qDebug()<<dish.merchant_id<<"菜品数量"<<table->value(dish.merchant_id).size();
            }
            emit datacenter->consumerGetDishListDone();
        } else {
            qDebug() << "Request failed: " << reply->errorString();
        }

        // 释放回复对象
        reply->deleteLater();
    });
}

void NetClient::consumerGetDishInfo(const QString &dish_id)
{
    // 1. 构造 JSON 数据
    QJsonObject jsonObj;
    jsonObj["dish_id"] = dish_id;

    // 将 JSON 对象转换为字符串
    QJsonDocument doc(jsonObj);
    QByteArray jsonData = doc.toJson();

    // 2. 创建 HTTP 请求并设置 URL 和请求头
    QUrl url(httpUrl + "/consumer/dish/dishInfo");  //
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
                data::Dish* dish = new data::Dish;
                dish->loadFromJson(responseObj["dish"].toString().toStdString());
                if(datacenter->dish)
                {
                    delete datacenter->dish;
                    datacenter->dish = nullptr;
                }
                datacenter->dish = dish;    //导入缓冲区
                emit datacenter->consumergetDishInfoDone();
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

void NetClient::consumerOrderGenerate(const QString &merchant_id)
{
    // 1. 构造 JSON 数据
    QJsonObject jsonObj;

    if(datacenter->cart_list->cart_table->find(merchant_id) == datacenter->cart_list->cart_table->end())
    {
        qDebug()<<"商家不存在";
        return;
    }
    Cart::ptr cart = *(datacenter->cart_list->cart_table->find(merchant_id));
    jsonObj["merchant_id"] =cart->merchant_id;
    jsonObj["merchant_name"] = cart->merchant_name;
    jsonObj["consumer_id"] = datacenter->account->uuid;
    jsonObj["consumer_name"] = datacenter->account->name;
    jsonObj["level"] = static_cast<int>(datacenter->account->level);
    jsonObj["pay"] = cart->pay;
    jsonObj["count"] = cart->cnt;

    QJsonArray jsonArr;

    for(auto it = cart->dish_table->begin();it!=cart->dish_table->end();++it)
    {
        QJsonObject obj;
        obj["dish_id"] = it.value()->dish_id;
        obj["merchant_id"] = cart->merchant_id;
        obj["dish_name"] = it.value()->dish_name;
        obj["dish_price"] = it.value()->dish_price;
        obj["count"] = it.value()->cnt;
        jsonArr.append(obj);
    }

    jsonObj["dish_arr"] = jsonArr;

    //清空购物车
    datacenter->cart_list->cart_table->erase(datacenter->cart_list->cart_table->find(merchant_id));
    // 将 JSON 对象转换为字符串
    QJsonDocument doc(jsonObj);
    QByteArray jsonData = doc.toJson();

    // 2. 创建 HTTP 请求并设置 URL 和请求头
    QUrl url(httpUrl + "/consumer/order/generate");  //
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
                // qDebug()<<doc.toJson();
                QJsonObject responseObj = doc.object();
                data::Order order;
                // qDebug()<<order.status;
                // qDebug()<<responseObj["order"].toString();
                order.loadFromJson(responseObj["order"].toString().toStdString());
                datacenter->order_table->insert(order.uuid,order);    //导入订单列表
                // qDebug()<<"NetClient order stat"<<order.status;
                emit datacenter->consumerOrderGenerateDone();
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

void NetClient::consumerGetOrderList(const QString &consumer_id)
{
    // 1. 构造 JSON 数据
    QJsonObject jsonObj;
    jsonObj["consumer_id"] = consumer_id;

    // 将 JSON 对象转换为字符串
    QJsonDocument doc(jsonObj);
    QByteArray jsonData = doc.toJson();

    // 2. 创建 HTTP 请求并设置 URL 和请求头
    QUrl url(httpUrl + "/consumer/order/list");  //
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
            QJsonObject responseObj = doc.object();
            // qDebug()<<responseData;
            auto list  = datacenter->OrderListFromJsonArray(responseObj["order_list"].toString());
            //先清空旧数据
            datacenter->order_table->clear();
            for(const data::Order&order:list)
            {
                // qDebug()<<order.name;
                datacenter->order_table->insert(order.uuid,order);
            }
            emit datacenter->consumerGetOrderListDone();

        } else {
            qDebug() << "Request failed: " << reply->errorString();
        }

        // 释放回复对象
        reply->deleteLater();
    });
}

void NetClient::consumerGetOrdrerDishList(const QString &order_id)
{
    // 1. 构造 JSON 数据
    QJsonObject jsonObj;
    jsonObj["order_id"] = order_id;

    // 将 JSON 对象转换为字符串
    QJsonDocument doc(jsonObj);
    QByteArray jsonData = doc.toJson();

    // 2. 创建 HTTP 请求并设置 URL 和请求头
    QUrl url(httpUrl + "/merchant/dish/info");  //
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json;charset=UTF-8");

    // 3. 发送 POST 请求
    QNetworkReply *reply = httpClient.post(request, jsonData);

    // 4. 连接信号和槽以处理响应
    connect(reply, &QNetworkReply::finished, [this, reply,order_id]() {
        if (reply->error() == QNetworkReply::NoError) {
            // 处理服务器返回的数据
            QByteArray responseData = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(responseData);
            if (!doc.isNull()) {
                QJsonObject responseObj = doc.object();
                data::Order* order = new data::Order;
                auto list  = datacenter->OrderDishListFromJsonArray(responseObj["dish_list"].toString());
                qDebug()<<"获取到的菜品数量:"<<list.size();
                QList<data::OrderDish>* dish_list = datacenter->consumer_order_dish_list;
                if(dish_list != nullptr)
                {
                    delete dish_list;
                    dish_list = nullptr;
                }
                datacenter->consumer_order_dish_list = new QList<data::OrderDish>(list);
                emit datacenter->consumerGetOrderDishListDone(order_id);
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

void NetClient::merchantGetDishList(const QString &merchant_id)
{
    // 1. 构造 JSON 数据
    QJsonObject jsonObj;
    jsonObj["merchant_id"] = merchant_id;

    // 将 JSON 对象转换为字符串
    QJsonDocument doc(jsonObj);
    QByteArray jsonData = doc.toJson();

    // 2. 创建 HTTP 请求并设置 URL 和请求头
    QUrl url(httpUrl + "/merchant/dish/list");  //
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
            QJsonObject responseObj = doc.object();
            // qDebug()<<responseData;
            auto list  = datacenter->DishListFromJsonArray(responseObj["dish_list"].toString());
            //先清空旧数据
            datacenter->merchant_dish_table->clear();
            for(const data::Dish&dish:list)
            {
                qDebug()<<dish.name;
                datacenter->merchant_dish_table->insert(dish.uuid,dish);
            }
            emit datacenter->merchantGetDishListDone();

        } else {
            qDebug() << "Request failed: " << reply->errorString();
        }

        // 释放回复对象
        reply->deleteLater();
    });
}

void NetClient::merchantDishRegister(const QString &name, const QString &link, double price,double price_factor, const QString &introduction)
{
    // 1. 构造 JSON 数据
    QJsonObject jsonObj;
    jsonObj["name"] = name;
    jsonObj["icon_path"] = link;  // 直接使用传入的已加密密码
    jsonObj["price"] = price;
    jsonObj["price_factor"] = price_factor;
    jsonObj["introduction"] = introduction;
    jsonObj["merchant_id"] = datacenter->account->uuid;
    jsonObj["merchant_name"] = datacenter->account->nickname;

    // 将 JSON 对象转换为字符串
    QJsonDocument doc(jsonObj);
    QByteArray jsonData = doc.toJson();

    // 2. 创建 HTTP 请求并设置 URL 和请求头
    QUrl url(httpUrl + "/merchant/dish/register");  // 注册接口是 /account/register
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
                QString Message = responseObj["message"].toString();
                if (success) {
                    qDebug() << "Register registration successful!";
                } else {
                    QString errorMessage = responseObj["message"].toString();
                    qDebug() << "Register registration failed: " << errorMessage;
                    // 处理失败的情况
                }
                emit datacenter->merchantDishRegisterDone(success,Message);
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

void NetClient::merchantDishEditSave(const QString &dish_id, const QString &name, const QString &link, double price, double price_factor, const QString &introduction)
{
    // 1. 构造 JSON 数据
    QJsonObject jsonObj;
    jsonObj["dish_id"] = dish_id;
    jsonObj["name"] = name;
    jsonObj["icon_path"] = link;  // 直接使用传入的已加密密码
    jsonObj["price"] = price;
    jsonObj["price_factor"] = price_factor;
    jsonObj["introduction"] = introduction;
    jsonObj["merchant_id"] = datacenter->account->uuid;
    jsonObj["merchant_name"] = datacenter->account->nickname;

    // 将 JSON 对象转换为字符串
    QJsonDocument doc(jsonObj);
    QByteArray jsonData = doc.toJson();

    // 2. 创建 HTTP 请求并设置 URL 和请求头
    QUrl url(httpUrl + "/merchant/dish/update");  // 注册接口是 /account/register
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
                QString Message = responseObj["message"].toString();
                if (success) {
                    qDebug() << "Register registration successful!";
                } else {
                    QString errorMessage = responseObj["message"].toString();
                    qDebug() << "Register registration failed: " << errorMessage;
                    // 处理失败的情况
                }
                emit datacenter->merchantDishEditSaveDone(success,Message);
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

void NetClient::merchantGetDishInfo(const QString &dish_id)
{
    // 1. 构造 JSON 数据
    QJsonObject jsonObj;
    jsonObj["dish_id"] = dish_id;

    // 将 JSON 对象转换为字符串
    QJsonDocument doc(jsonObj);
    QByteArray jsonData = doc.toJson();

    // 2. 创建 HTTP 请求并设置 URL 和请求头
    QUrl url(httpUrl + "/merchant/dish/info");  //
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
                data::Dish* dish = new data::Dish;
                dish->loadFromJson(responseObj["dish"].toString().toStdString());
                if(datacenter->dish)
                {
                    delete datacenter->dish;
                    datacenter->dish = nullptr;
                }
                datacenter->dish = dish;    //导入缓冲区
                emit datacenter->merchantGetDishInfoDone();
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

void NetClient::merchantDishEditDel(const QString &dish_id)
{
    // 1. 构造 JSON 数据
    QJsonObject jsonObj;
    jsonObj["dish_id"] = dish_id;

    // 将 JSON 对象转换为字符串
    QJsonDocument doc(jsonObj);
    QByteArray jsonData = doc.toJson();

    // 2. 创建 HTTP 请求并设置 URL 和请求头
    QUrl url(httpUrl + "/merchant/dish/del");  //
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
                emit datacenter->merchantDishEditDelDone();
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

void NetClient::merchantGetOrderList(const QString &merchant_id)
{
    // 1. 构造 JSON 数据
    QJsonObject jsonObj;
    jsonObj["merchant_id"] = merchant_id;

    // 将 JSON 对象转换为字符串
    QJsonDocument doc(jsonObj);
    QByteArray jsonData = doc.toJson();

    // 2. 创建 HTTP 请求并设置 URL 和请求头
    QUrl url(httpUrl + "/merchant/order/list");  //
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
            QJsonObject responseObj = doc.object();
            // qDebug()<<responseData;
            auto list  = datacenter->OrderListFromJsonArray(responseObj["order_list"].toString());
            //先清空旧数据
            datacenter->merchant_order_table->clear();
            for(const data::Order&order:list)
            {
                // qDebug()<<order.name;
                datacenter->merchant_order_table->insert(order.uuid,order);
            }
            emit datacenter->merchantGetOrderListDone();

        } else {
            qDebug() << "Request failed: " << reply->errorString();
        }

        // 释放回复对象
        reply->deleteLater();
    });
}

void NetClient::merchantGetOrderDishList(const QString &order_id)
{
    // 1. 构造 JSON 数据
    QJsonObject jsonObj;
    jsonObj["order_id"] = order_id;

    // 将 JSON 对象转换为字符串
    QJsonDocument doc(jsonObj);
    QByteArray jsonData = doc.toJson();

    // 2. 创建 HTTP 请求并设置 URL 和请求头
    QUrl url(httpUrl + "/merchant/order/detail/dishlist");  //
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json;charset=UTF-8");

    // 3. 发送 POST 请求
    QNetworkReply *reply = httpClient.post(request, jsonData);

    // 4. 连接信号和槽以处理响应
    connect(reply, &QNetworkReply::finished, [this, reply,order_id]() {
        if (reply->error() == QNetworkReply::NoError) {
            // 处理服务器返回的数据
            QByteArray responseData = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(responseData);
            QJsonObject responseObj = doc.object();
            // qDebug()<<responseData;
            auto list  = datacenter->OrderDishListFromJsonArray(responseObj["dish_list"].toString());
            qDebug()<<"获取到的菜品数量:"<<list.size();
            QList<data::OrderDish>* dish_list = datacenter->merchant_order_dish_list;
            if(dish_list != nullptr)
            {
                delete dish_list;
                dish_list = nullptr;
            }
            datacenter->merchant_order_dish_list = new QList<data::OrderDish>(list);
            emit datacenter->merchantGetOrderDishListDone(order_id);
        } else {
            qDebug() << "Request failed: " << reply->errorString();
        }

        // 释放回复对象
        reply->deleteLater();
    });
}

void NetClient::adminGetOrderList()
{
    // 2. 创建 HTTP 请求并设置 URL 和请求头
    QUrl url(httpUrl + "/admin/order/list");  //
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json;charset=UTF-8");

    // 3. 发送 POST 请求
    QNetworkReply *reply = httpClient.post(request, "");

    // 4. 连接信号和槽以处理响应
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            // 处理服务器返回的数据
            QByteArray responseData = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(responseData);
            QJsonObject responseObj = doc.object();
            // qDebug()<<responseData;
            auto list  = datacenter->OrderListFromJsonArray(responseObj["order_list"].toString());
            //先清空旧数据
            if(datacenter->admin_order_list == nullptr)
            {
                datacenter->admin_order_list=new QList<data::Order>;
            }
            datacenter->admin_order_list->clear();
            datacenter->admin_order_list->append(list.begin(),list.end());
            emit datacenter->adminGetOrderListDone();

        } else {
            qDebug() << "Request failed: " << reply->errorString();
        }

        // 释放回复对象
        reply->deleteLater();
    });
}


}
