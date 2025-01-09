#include "HTTPServer.h"

btyGoose::HTTPServer::HTTPServer()
{
    loadConfig();
    initTestAPI();
    initAccountAPI();
    initConsumerAPI();
    initMerchantAPI();
    initAdminAPI();
    svr.listen(host.toStdString(), port);
}

bool btyGoose::HTTPServer::loadConfig()
{

    QFile file("serverConfig.json");
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);

        if (doc.isObject()) {
            QJsonObject jsonObject = doc.object();
            host = jsonObject["host"].toString();
            port = jsonObject["port"].toInt();
            
            return true;
        }
    }
    //反序列化失败
    saveConfig();
    return false;
}

void btyGoose::HTTPServer::saveConfig()
{
    QFile file("serverConfig.json");
    if (file.open(QIODevice::WriteOnly)) {
        QJsonObject jsonObject;
        jsonObject["host"] = host;
        jsonObject["port"] = port;

        QJsonDocument doc(jsonObject);
        file.write(doc.toJson());
        file.close();
    }
    else
    {
        std::cerr << "server配置文件打开错误";
    }
}

//model

/*
 svr.Post("", [this](const httplib::Request& req, httplib::Response& res) {
        qDebug() << " get a post!";
        std::string jsonStr = req.body;
        QString qJsonString = QString::fromStdString(jsonStr);
        QJsonObject jsonObj;
        // 解析 JSON 字符串
        QJsonDocument doc = QJsonDocument::fromJson(qJsonString.toUtf8());
        if (doc.isObject()) {
            jsonObj = doc.object();
        }
        else
        {
            qDebug() << "Invalid Json" << jsonStr;
            jsonObj = QJsonObject();
        }
        QJsonObject resJson;
        try {
            if (jsonObj.isEmpty())
            {
                throw HTTPException("Json Serialization failed");
            }
        }catch (const HTTPException& e)
        {
            res.status = 500;
            resJson["success"] = false;
            resJson["message"] = e.what();
            QJsonDocument doc(resJson);
            res.body = doc.toJson().toStdString();
            res.set_header("Content-Type", "application/json;charset=UTF-8");
            qDebug() << e.what();
        }

    });
*/

void btyGoose::HTTPServer::initTestAPI()
{
    svr.Get("/ping", [=](const httplib::Request& req, httplib::Response& res) {
        qDebug() << "/ping";
        res.body = "pong";
        res.set_header("Content-Type", "application/text;charset=UTF-8");
        });
}

void btyGoose::HTTPServer::initAccountAPI()
{
    //用户名登录
    svr.Post("/account/login/username", [this](const httplib::Request& req, httplib::Response& res) {
        LOG() << "/account/login/username get a post!";
        std::string jsonStr = req.body;
        QString qJsonString = QString::fromStdString(jsonStr);
        QJsonObject jsonObj;
        // 解析 JSON 字符串
        QJsonDocument doc = QJsonDocument::fromJson(qJsonString.toUtf8());
        if (doc.isObject()) {
            jsonObj = doc.object();
        }
        else
        {
            qDebug() << "Invalid Json" << jsonStr;
            jsonObj = QJsonObject();
        }
        QJsonObject resJson;
        try {
            if (jsonObj.isEmpty())
            {
                throw HTTPException("Json Serialization failed");
            }
            data::Account record_acc = db.searchAccountByName(jsonObj["name"].toString());
            if (!record_acc.name.isEmpty() && jsonObj["password"].toString() == record_acc.password)
            {
                res.status = 200;
                resJson["message"] = "登录认证成功!";
                resJson["success"] = true;
                resJson["account"] = record_acc.toJson();
                QJsonDocument doc(resJson);
                res.body = doc.toJson().toStdString();
                res.set_header("Content-Type", "application/json;charset=UTF-8");

            }
            else
            {
                res.status = 200;
                resJson["message"] = "登录认证失败!";
                resJson["success"] = false;
                QJsonDocument doc(resJson);
                res.body = doc.toJson().toStdString();
                res.set_header("Content-Type", "application/json;charset=UTF-8");
            }
        }
        catch (const HTTPException& e)
        {
            res.status = 500;
            resJson["success"] = false;
            resJson["message"] = e.what();
            QJsonDocument doc(resJson);
            res.body = doc.toJson().toStdString();
            res.set_header("Content-Type", "application/json");
            qDebug() << e.what();
        }

        LOG() << "login by name done";
    });

    svr.Post("/account/login/phone", [this](const httplib::Request& req, httplib::Response& res) {
        qDebug() << "/account/login/phone get a post!";
        std::string jsonStr = req.body;
        QString qJsonString = QString::fromStdString(jsonStr);
        QJsonObject jsonObj;
        // 解析 JSON 字符串
        QJsonDocument doc = QJsonDocument::fromJson(qJsonString.toUtf8());
        if (doc.isObject()) {
            jsonObj = doc.object();
        }
        else
        {
            qDebug() << "Invalid Json" << jsonStr;
            jsonObj = QJsonObject();
        }
        QJsonObject resJson;
        try {
            if (jsonObj.isEmpty())
            {
                throw HTTPException("Json Serialization failed");
            }
            data::Account record_acc = db.searchAccountByPhone(jsonObj["phone"].toString());
            if (!record_acc.phone.isEmpty() && jsonObj["password"].toString() == record_acc.password)
            {
                res.status = 200;
                resJson["message"] = "登录认证成功!";
                resJson["success"] = true;
                resJson["account"] = record_acc.toJson();
                QJsonDocument doc(resJson);
                res.body = doc.toJson().toStdString();
                res.set_header("Content-Type", "application/json;charset=UTF-8");
            }
            else
            {
                res.status = 200;
                resJson["message"] = "登录认证失败!";
                resJson["success"] = false;
                QJsonDocument doc(resJson);
                res.body = doc.toJson().toStdString();
                res.set_header("Content-Type", "application/json;charset=UTF-8");
            }
        }
        catch (const HTTPException& e)
        {
            res.status = 500;
            resJson["success"] = false;
            resJson["message"] = e.what();
            QJsonDocument doc(resJson);
            res.body = doc.toJson().toStdString();
            res.set_header("Content-Type", "application/json;charset=UTF-8");
            qDebug() << e.what();
        }

        });

    //账户注册
    svr.Post("/account/register", [this](const httplib::Request& req, httplib::Response& res) {
        qDebug() << "/account/register get a post!";
        LOG() << "开始注册";
        std::string jsonStr = req.body;
        QString qJsonString = QString::fromStdString(jsonStr);
        QJsonObject jsonObj;
        // 解析 JSON 字符串
        QJsonDocument doc = QJsonDocument::fromJson(qJsonString.toUtf8());
        if (doc.isObject()) {
            jsonObj = doc.object();
        }
        else
        {
            qDebug() << "Invalid Json" << jsonStr;
            jsonObj = QJsonObject();
        }
        QJsonObject resJson;
        res.set_header("Content-Type", "application/json;charset=UTF-8");
        try {
            if (jsonObj.isEmpty())
            {
                throw HTTPException("Json Serialization failed");
            }

            data::Account acc;
            acc.name = jsonObj["name"].toString();
            acc.password = jsonObj["password"].toString();  // 直接使用传入的已加密密码
            acc.phone = jsonObj["phone"].toString();
            acc.nickname = jsonObj["nickname"].toString();
            acc.type = (data::Account::Type)jsonObj["type"].toInt();
            acc.level = data::Account::Level::MEMBER;
            acc.uuid = util::makeId("A");

            if (acc.uuid.isEmpty())
            {
                res.status = 200;
                resJson["message"] = "uuid生成错误!";
                resJson["success"] = false;
                QJsonDocument doc(resJson);
                res.body = doc.toJson().toStdString();
                return;
            }
            if (!AuthenticateAuthCode(acc.phone, jsonObj["auth_code"].toString()))
            {
                res.status = 200;
                resJson["message"] = "验证码错误!";
                resJson["success"] = false;
                QJsonDocument doc(resJson);
                res.body = doc.toJson().toStdString();
                return;
            }
            if (!db.searchAccountByName(acc.name).name.isEmpty())
            {
                //账户名重复
                LOG() << "用户名已存在:" << acc.name;
                res.status = 200;
                resJson["message"] = "账户名已存在!";
                resJson["success"] = false;
                QJsonDocument doc(resJson);
                res.body = doc.toJson().toStdString();
                return;
            }
            LOG() << "用户名查重通过";
            if (!db.searchAccountByPhone(acc.phone).name.isEmpty())
            {
                //电话重复
                res.status = 200;
                LOG() << "电话查重失败";
                resJson["message"] = "手机号已被占用!";
                resJson["success"] = false;
                QJsonDocument doc(resJson);
                res.body = doc.toJson().toStdString();
                return;
            }
            LOG() << "电话查重通过";

            if (db.searchAccountByID(acc.uuid).name.isEmpty())
            {
                if (!db.addAccount(acc))
                {
                    throw HTTPException("databse add account failed");
                }
                LOG() << "成功向数据库插入新账号";
                resJson["success"] = true;
                resJson["message"] = "账号创建成功";
                QJsonDocument doc(resJson);
                res.body = doc.toJson().toStdString();
            }
            else
            {
                //uuid生成错误
                res.status = 200;
                resJson["message"] = "uuid生成错误!";
                resJson["success"] = false;
                QJsonDocument doc(resJson);
                res.body = doc.toJson().toStdString();
                return;
            }
        }
        catch (const HTTPException& e)
        {
            res.status = 500;
            resJson["success"] = false;
            resJson["message"] = e.what();
            QJsonDocument doc(resJson);
            res.body = doc.toJson().toStdString();
            res.set_header("Content-Type", "application/json;charset=UTF-8");
            qDebug() << e.what();
        }

        });


    svr.Post("/account/consumer/update", [this](const httplib::Request& req, httplib::Response& res) {
        qDebug() << "/account/consumer/update get a post!";
        std::string jsonStr = req.body;
        QString qJsonString = QString::fromStdString(jsonStr);
        QJsonObject jsonObj;
        // 解析 JSON 字符串
        QJsonDocument doc = QJsonDocument::fromJson(qJsonString.toUtf8());
        if (doc.isObject()) {
            jsonObj = doc.object();
        }
        else
        {
            qDebug() << "Invalid Json" << jsonStr;
            jsonObj = QJsonObject();
        }
        QJsonObject resJson;
        try {
            if (jsonObj.isEmpty())
            {
                throw HTTPException("Json Serialization failed");
            }

            QString uuid = jsonObj["uuid"].toString();
            double pay = jsonObj["pay"].toDouble();
            QString level = jsonObj["level"].toString();
            data::Account acc = db.searchAccountByID(uuid);
            bool ret = getPay(pay);
            if (ret == true)
            {
                //支付成功
                if (level == "VIP") acc.level = data::Account::Level::VIP;
                else if (level == "SUVIP") acc.level = data::Account::Level::SUVIP;
                else throw HTTPException("未知Level等级" + level);
         
            }
            else
            {
                QJsonObject resJson;
                resJson["success"] = false;
                resJson["message"] = "用户升级失败";
                QJsonDocument doc(resJson);
                res.body = doc.toJson().toStdString();
            }
            
        }
        catch (const HTTPException& e)
        {
            res.status = 500;
            resJson["success"] = false;
            resJson["message"] = e.what();
            QJsonDocument doc(resJson);
            res.body = doc.toJson().toStdString();
            res.set_header("Content-Type", "application/json;charset=UTF-8");
            qDebug() << e.what();
        }

        });

    svr.Post("/account/update/level", [this](const httplib::Request& req, httplib::Response& res) {
        LOG() << "/account/update/level get a post!";
        std::string jsonStr = req.body;
        QString qJsonString = QString::fromStdString(jsonStr);
        QJsonObject jsonObj;
        // 解析 JSON 字符串
        QJsonDocument doc = QJsonDocument::fromJson(qJsonString.toUtf8());
        if (doc.isObject()) {
            jsonObj = doc.object();
        }
        else
        {
            qDebug() << "Invalid Json" << jsonStr;
            jsonObj = QJsonObject();
        }
        QJsonObject resJson;
        try {
            if (jsonObj.isEmpty())
            {
                throw HTTPException("Json Serialization failed");
            }
            data::Account record_acc = db.searchAccountByID(jsonObj["id"].toString());
            QString level = jsonObj["level"].toString();
            qDebug() << "level change to: " << level;
            if (level == "VIP")
            {
                record_acc.level = data::Account::Level::VIP;
            }            
            else if (level == "SUVIP")
            {
                record_acc.level = data::Account::Level::SUVIP;
            }

            db.updateAccount(record_acc);
            record_acc = db.searchAccountByID(jsonObj["id"].toString());
            resJson["level"] = static_cast<int>(record_acc.level);
            QJsonDocument doc(resJson);
            res.body = doc.toJson().toStdString();
            res.set_header("Content-Type", "application/json");
        }
        catch (const HTTPException& e)
        {
            res.status = 500;
            resJson["success"] = false;
            resJson["message"] = e.what();
            QJsonDocument doc(resJson);
            res.body = doc.toJson().toStdString();
            res.set_header("Content-Type", "application/json");
            qDebug() << e.what();
        }

        LOG() << "login by name done";
        });
}

bool btyGoose::HTTPServer::AuthenticateAuthCode(const QString& phone, const QString& auth_code)
{
    ///////////////////////////
    // TODO
    // ///////////////////
    //这里就不具体实现了
    if (auth_code == "888888")
    {
        return true;
    }
    else
    {
        return false;
    }
}


void btyGoose::HTTPServer::initConsumerAPI()
{
    svr.Post("/consumer/dish/list", [this](const httplib::Request& req, httplib::Response& res) {
        LOG() << "/consumer/dish/list get a post!";
        QList<data::Dish> dishList = db.getAllDishList();
        QString dishListJson = DishListToJsonArray(dishList);
        QJsonObject resJson;
        resJson["dish_list"] = dishListJson;
        QJsonDocument doc(resJson);
        res.body = doc.toJson().toStdString();
        res.set_header("Content-Type", "application/json;charset=UTF-8");
        });

    svr.Post("/consumer/dish/dishInfo", [this](const httplib::Request& req, httplib::Response& res) {
        LOG() << "/consumer/dish/dishInfo get a post!";
        std::string jsonStr = req.body;
        QString qJsonString = QString::fromStdString(jsonStr);
        QJsonObject jsonObj;
        // 解析 JSON 字符串
        QJsonDocument doc = QJsonDocument::fromJson(qJsonString.toUtf8());
        if (doc.isObject()) {
            jsonObj = doc.object();
        }
        else
        {
            qDebug() << "Invalid Json" << jsonStr;
            jsonObj = QJsonObject();
        }
        QJsonObject resJson;
        try {
            if (jsonObj.isEmpty())
            {
                throw HTTPException("Json Serialization failed");
            }

            QString dish_id = jsonObj["dish_id"].toString();
            data::Dish dish = db.searchDishByID(dish_id);
            resJson["dish"] = dish.toJson();
            QJsonDocument doc(resJson);
            res.body = doc.toJson().toStdString();
            res.set_header("Content-Type", "application/json;charset=UTF-8");
        }
        catch (const HTTPException& e)
        {
            res.status = 500;
            resJson["success"] = false;
            resJson["message"] = e.what();
            QJsonDocument doc(resJson);
            res.body = doc.toJson().toStdString();
            res.set_header("Content-Type", "application/json;charset=UTF-8");
            qDebug() << e.what();
        }

        });

    //订单生成
    svr.Post("/consumer/order/generate", [this](const httplib::Request& req, httplib::Response& res) {
        LOG() << "/consumer/order/generate get a post!";
        std::string jsonStr = req.body;
        QString qJsonString = QString::fromStdString(jsonStr);
        QJsonObject jsonObj;
        // 解析 JSON 字符串
        QJsonDocument doc = QJsonDocument::fromJson(qJsonString.toUtf8());
        if (doc.isObject()) {
            jsonObj = doc.object();
        }
        else
        {
            qDebug() << "Invalid Json" << jsonStr;
            jsonObj = QJsonObject();
        }
        QJsonObject resJson;
        try {
            if (jsonObj.isEmpty())
            {
                throw HTTPException("Json Serialization failed");
            }

            data::Order order;
            order.uuid = util::makeId("O");
            order.merchant_id = jsonObj["merchant_id"].toString();
            order.merchant_name = jsonObj["merchant_name"].toString();
            order.consumer_id = jsonObj["consumer_id"].toString();
            order.consumer_name = jsonObj["consumer_name"].toString();
            order.level = static_cast<data::Account::Level>(jsonObj["level"].toInt());
            order.pay = jsonObj["pay"].toDouble();
            order.sum = jsonObj["count"].toInt();
            order.time = QString::number(util::getSecTime());
            order.status = data::Order::Status::UNPAYED;

            LOG() << "生成一个order,merchant id:" << order.merchant_id;

            bool ok = db.addOrder(order);
            Q_ASSERT(ok);
            QList<data::OrderDish> dish_list;
            QJsonArray jsonArr = jsonObj["dish_arr"].toArray();
            for (const QJsonValue& value : jsonArr)
            {
                LOG() << "生成一个OrderDish";
                QJsonObject obj = value.toObject();
                data::OrderDish dish;
                dish.order_id = order.uuid;
                dish.merchant_id = order.merchant_id;
                dish.name = obj["dish_name"].toString();
                dish.dish_price = obj["dish_price"].toDouble();
                dish.count = obj["count"].toInt();
                dish.dish_id = obj["dish_id"].toString();
                dish_list.append(dish);
            }
            ok = db.addOrderDishesByID(order.uuid, dish_list);
            Q_ASSERT(ok);

            //构建返回报文
            resJson["order"] = order.toJson();

            QJsonDocument doc(resJson);
            res.body = doc.toJson().toStdString();
            res.set_header("Content-Type", "application/json;charset=UTF-8");
        }
        catch (const HTTPException& e)
        {
            res.status = 500;
            resJson["success"] = false;
            resJson["message"] = e.what();
            QJsonDocument doc(resJson);
            res.body = doc.toJson().toStdString();
            res.set_header("Content-Type", "application/json;charset=UTF-8");
            qDebug() << e.what();
        }

        });

    svr.Post("/consumer/order/list", [this](const httplib::Request& req, httplib::Response& res) {
        LOG() << "/consumer/order/list get a post!";

        std::string jsonStr = req.body;
        QString qJsonString = QString::fromStdString(jsonStr);
        QJsonObject jsonObj;
        // 解析 JSON 字符串
        QJsonDocument doc = QJsonDocument::fromJson(qJsonString.toUtf8());
        if (doc.isObject()) {
            jsonObj = doc.object();
        }
        else
        {
            qDebug() << "Invalid Json" << jsonStr;
            jsonObj = QJsonObject();
        }
        QJsonObject resJson;
        try {
            if (jsonObj.isEmpty())
            {
                throw HTTPException("Json Serialization failed");
            }


            QList<data::Order> orderList = db.getOrderListByConsumer(jsonObj["consumer_id"].toString());
            LOG() << "consumer:" << jsonObj["consumer_id"].toString() << "find orders" << orderList.size();
            QString dishListJson = OrderListToJsonArray(orderList);
            QJsonObject resJson;
            resJson["order_list"] = dishListJson;
            QJsonDocument doc(resJson);
            res.body = doc.toJson().toStdString();
            res.set_header("Content-Type", "application/json;charset=UTF-8");
        }
        catch (const HTTPException& e)
        {
            res.status = 500;
            resJson["success"] = false;
            resJson["message"] = e.what();
            QJsonDocument doc(resJson);
            res.body = doc.toJson().toStdString();
            res.set_header("Content-Type", "application/json;charset=UTF-8");
            qDebug() << e.what();
        }

        });
}

void btyGoose::HTTPServer::initMerchantAPI()
{
    svr.Post("/merchant/dish/list", [this](const httplib::Request& req, httplib::Response& res) {
        LOG() << "/merchant/dish/list get a post!";

        std::string jsonStr = req.body;
        QString qJsonString = QString::fromStdString(jsonStr);
        QJsonObject jsonObj;
        // 解析 JSON 字符串
        QJsonDocument doc = QJsonDocument::fromJson(qJsonString.toUtf8());
        if (doc.isObject()) {
            jsonObj = doc.object();
        }
        else
        {
            qDebug() << "Invalid Json" << jsonStr;
            jsonObj = QJsonObject();
        }
        QJsonObject resJson;
        try {
            if (jsonObj.isEmpty())
            {
                throw HTTPException("Json Serialization failed");
            }


            QList<data::Dish> dishList = db.getDishListByMerchant(jsonObj["merchant_id"].toString());
            LOG() << "merchant:" << jsonObj["merchant_id"].toString() << "find dishes" << dishList.size();
            QString dishListJson = DishListToJsonArray(dishList);
            QJsonObject resJson;
            resJson["dish_list"] = dishListJson;
            QJsonDocument doc(resJson);
            res.body = doc.toJson().toStdString();
            res.set_header("Content-Type", "application/json;charset=UTF-8");
        }
        catch (const HTTPException& e)
        {
            res.status = 500;
            resJson["success"] = false;
            resJson["message"] = e.what();
            QJsonDocument doc(resJson);
            res.body = doc.toJson().toStdString();
            res.set_header("Content-Type", "application/json;charset=UTF-8");
            qDebug() << e.what();
        }

        });

    //菜品注册
    svr.Post("/merchant/dish/register", [this](const httplib::Request& req, httplib::Response& res) {
        qDebug() << "/merchant/dish/register get a post!";
        LOG() << "菜品开始注册";
        std::string jsonStr = req.body;
        QString qJsonString = QString::fromStdString(jsonStr);
        QJsonObject jsonObj;
        // 解析 JSON 字符串
        QJsonDocument doc = QJsonDocument::fromJson(qJsonString.toUtf8());
        if (doc.isObject()) {
            jsonObj = doc.object();
        }
        else
        {
            qDebug() << "Invalid Json" << jsonStr;
            jsonObj = QJsonObject();
        }
        QJsonObject resJson;
        res.set_header("Content-Type", "application/json;charset=UTF-8");
        try {
            if (jsonObj.isEmpty())
            {
                throw HTTPException("Json Serialization failed");
            }

            data::Dish dish;
            dish.uuid = util::makeId("D");
            dish.name = jsonObj["name"].toString();
            dish.merchant_id = jsonObj["merchant_id"].toString();
            dish.merchant_name = jsonObj["merchant_name"].toString();
            dish.icon_path = jsonObj["icon_path"].toString();
            dish.description = jsonObj["introduction"].toString();
            dish.base_price = jsonObj["price"].toDouble();
            dish.price_factor = jsonObj["price_factor"].toDouble();
            dish.is_delete = false;

            if (dish.uuid.isEmpty())
            {
                res.status = 200;
                resJson["message"] = "uuid生成错误!";
                resJson["success"] = false;
                QJsonDocument doc(resJson);
                res.body = doc.toJson().toStdString();
                return;
            }

            if (db.searchDishByID(dish.uuid).name.isEmpty())
            {
                if (!db.addDish(dish))
                {
                    throw HTTPException("databse add dish failed");
                }
                LOG() << "成功向数据库插入新菜品"<<dish.name;
                resJson["success"] = true;
                resJson["message"] = "菜品创建成功";
                QJsonDocument doc(resJson);
                res.body = doc.toJson().toStdString();
            }
            else
            {
                //uuid生成错误
                res.status = 200;
                resJson["message"] = "uuid生成错误!";
                resJson["success"] = false;
                QJsonDocument doc(resJson);
                res.body = doc.toJson().toStdString();
                return;
            }
        }
        catch (const HTTPException& e)
        {
            res.status = 500;
            resJson["success"] = false;
            resJson["message"] = e.what();
            QJsonDocument doc(resJson);
            res.body = doc.toJson().toStdString();
            res.set_header("Content-Type", "application/json;charset=UTF-8");
            qDebug() << e.what();
        }

        });

    svr.Post("/merchant/dish/info", [this](const httplib::Request& req, httplib::Response& res) {
        LOG() << "/merchant/dish/info get a post!";
        std::string jsonStr = req.body;
        QString qJsonString = QString::fromStdString(jsonStr);
        QJsonObject jsonObj;
        // 解析 JSON 字符串
        QJsonDocument doc = QJsonDocument::fromJson(qJsonString.toUtf8());
        if (doc.isObject()) {
            jsonObj = doc.object();
        }
        else
        {
            qDebug() << "Invalid Json" << jsonStr;
            jsonObj = QJsonObject();
        }
        QJsonObject resJson;
        try {
            if (jsonObj.isEmpty())
            {
                throw HTTPException("Json Serialization failed");
            }

            QString dish_id = jsonObj["dish_id"].toString();
            data::Dish dish = db.searchDishByID(dish_id);
            resJson["dish"] = dish.toJson();
            QJsonDocument doc(resJson);
            res.body = doc.toJson().toStdString();
            res.set_header("Content-Type", "application/json;charset=UTF-8");
        }
        catch (const HTTPException& e)
        {
            res.status = 500;
            resJson["success"] = false;
            resJson["message"] = e.what();
            QJsonDocument doc(resJson);
            res.body = doc.toJson().toStdString();
            res.set_header("Content-Type", "application/json;charset=UTF-8");
            qDebug() << e.what();
        }

        });



    //菜品注册
    svr.Post("/merchant/dish/update", [this](const httplib::Request& req, httplib::Response& res) {
        qDebug() << "/merchant/dish/update get a post!";
        LOG() << "菜品开始更新";
        std::string jsonStr = req.body;
        QString qJsonString = QString::fromStdString(jsonStr);
        QJsonObject jsonObj;
        // 解析 JSON 字符串
        QJsonDocument doc = QJsonDocument::fromJson(qJsonString.toUtf8());
        if (doc.isObject()) {
            jsonObj = doc.object();
        }
        else
        {
            qDebug() << "Invalid Json" << jsonStr;
            jsonObj = QJsonObject();
        }
        QJsonObject resJson;
        res.set_header("Content-Type", "application/json;charset=UTF-8");
        try {
            if (jsonObj.isEmpty())
            {
                throw HTTPException("Json Serialization failed");
            }

            //qDebug() << jsonObj["dish_id"].toString();
            data::Dish dish = db.searchDishByID(jsonObj["dish_id"].toString());
            qDebug() << dish.uuid;
            dish.name = jsonObj["name"].toString();
            dish.merchant_id = jsonObj["merchant_id"].toString();
            dish.merchant_name = jsonObj["merchant_name"].toString();
            dish.icon_path = jsonObj["icon_path"].toString();
            dish.description = jsonObj["introduction"].toString();
            dish.base_price = jsonObj["price"].toDouble();
            dish.price_factor = jsonObj["price_factor"].toDouble();


            if (!dish.uuid.isEmpty())
            {
                if (!db.updateDish(dish))
                {
                    throw HTTPException("databse add dish failed");
                }
                LOG() << "成功向数据库更新新菜品" << dish.name;
                resJson["success"] = true;
                resJson["message"] = "菜品创建成功";
                QJsonDocument doc(resJson);
                res.body = doc.toJson().toStdString();
            }
            else
            {
      
                res.status = 200;
                resJson["message"] = "找不到菜品!";
                resJson["success"] = false;
                QJsonDocument doc(resJson);
                res.body = doc.toJson().toStdString();
                return;
            }
        }
        catch (const HTTPException& e)
        {
            res.status = 500;
            resJson["success"] = false;
            resJson["message"] = e.what();
            QJsonDocument doc(resJson);
            res.body = doc.toJson().toStdString();
            res.set_header("Content-Type", "application/json;charset=UTF-8");
            qDebug() << e.what();
        }

        });

    svr.Post("/merchant/dish/del", [this](const httplib::Request& req, httplib::Response& res) {
        LOG() << "/merchant/dish/del get a post!";
        std::string jsonStr = req.body;
        QString qJsonString = QString::fromStdString(jsonStr);
        QJsonObject jsonObj;
        // 解析 JSON 字符串
        QJsonDocument doc = QJsonDocument::fromJson(qJsonString.toUtf8());
        if (doc.isObject()) {
            jsonObj = doc.object();
        }
        else
        {
            qDebug() << "Invalid Json" << jsonStr;
            jsonObj = QJsonObject();
        }
        QJsonObject resJson;
        try {
            if (jsonObj.isEmpty())
            {
                throw HTTPException("Json Serialization failed");
            }

            QString dish_id = jsonObj["dish_id"].toString();
            db.delDishByID(dish_id);
            resJson["success"] = true;
            resJson["message"] = "删除成功!";
            QJsonDocument doc(resJson);
            res.body = doc.toJson().toStdString();
            res.set_header("Content-Type", "application/json;charset=UTF-8");
        }
        catch (const HTTPException& e)
        {
            res.status = 500;
            resJson["success"] = false;
            resJson["message"] = e.what();
            QJsonDocument doc(resJson);
            res.body = doc.toJson().toStdString();
            res.set_header("Content-Type", "application/json;charset=UTF-8");
            qDebug() << e.what();
        }

        });

    svr.Post("/merchant/order/list", [this](const httplib::Request& req, httplib::Response& res) {
        LOG() << "/merchant/order/list get a post!";

        std::string jsonStr = req.body;
        QString qJsonString = QString::fromStdString(jsonStr);
        QJsonObject jsonObj;
        // 解析 JSON 字符串
        QJsonDocument doc = QJsonDocument::fromJson(qJsonString.toUtf8());
        if (doc.isObject()) {
            jsonObj = doc.object();
        }
        else
        {
            qDebug() << "Invalid Json" << jsonStr;
            jsonObj = QJsonObject();
        }
        QJsonObject resJson;
        try {
            if (jsonObj.isEmpty())
            {
                throw HTTPException("Json Serialization failed");
            }


            QList<data::Order> orderList = db.getOrderListByMerchantWaiting(jsonObj["merchant_id"].toString());
            LOG() << "merchant:" << jsonObj["merchant_id"].toString() << "find orders" << orderList.size();
            QString dishListJson = OrderListToJsonArray(orderList);
            QJsonObject resJson;
            resJson["order_list"] = dishListJson;
            QJsonDocument doc(resJson);
            res.body = doc.toJson().toStdString();
            res.set_header("Content-Type", "application/json;charset=UTF-8");
        }
        catch (const HTTPException& e)
        {
            res.status = 500;
            resJson["success"] = false;
            resJson["message"] = e.what();
            QJsonDocument doc(resJson);
            res.body = doc.toJson().toStdString();
            res.set_header("Content-Type", "application/json;charset=UTF-8");
            qDebug() << e.what();
        }

        });

    svr.Post("/merchant/order/detail/dishlist", [this](const httplib::Request& req, httplib::Response& res) {
        LOG() << "/merchant/order/detail/dishlist get a post!";
        std::string jsonStr = req.body;
        QString qJsonString = QString::fromStdString(jsonStr);
        QJsonObject jsonObj;
        // 解析 JSON 字符串
        QJsonDocument doc = QJsonDocument::fromJson(qJsonString.toUtf8());
        if (doc.isObject()) {
            jsonObj = doc.object();
        }
        else
        {
            qDebug() << "Invalid Json" << jsonStr;
            jsonObj = QJsonObject();
        }
        QJsonObject resJson;
        try {
            if (jsonObj.isEmpty())
            {
                throw HTTPException("Json Serialization failed");
            }


            QList<data::OrderDish> dishList = db.getOrderDishesByID(jsonObj["order_id"].toString());
            LOG() << "order:" << jsonObj["order_id"].toString() << "find dishes" << dishList.size();
            QString dishListJson = OrderDishListToJsonArray(dishList);
            QJsonObject resJson;
            resJson["dish_list"] = dishListJson;
            QJsonDocument doc(resJson);
            res.body = doc.toJson().toStdString();
            res.set_header("Content-Type", "application/json;charset=UTF-8");
        }
        catch (const HTTPException& e)
        {
            res.status = 500;
            resJson["success"] = false;
            resJson["message"] = e.what();
            QJsonDocument doc(resJson);
            res.body = doc.toJson().toStdString();
            res.set_header("Content-Type", "application/json;charset=UTF-8");
            qDebug() << e.what();
        }

        });
}

void btyGoose::HTTPServer::initAdminAPI()
{
    svr.Post("/admin/order/list", [this](const httplib::Request& req, httplib::Response& res) {
        LOG() << "/admin/order/list get a post!";

 
        QJsonObject resJson;
        try {
            QList<data::Order> orderList = db.getAllOrderList();
            LOG() << "admin:" << "find orders" << orderList.size();
            QString dishListJson = OrderListToJsonArray(orderList);
            QJsonObject resJson;
            resJson["order_list"] = dishListJson;
            QJsonDocument doc(resJson);
            res.body = doc.toJson().toStdString();
            res.set_header("Content-Type", "application/json;charset=UTF-8");
        }
        catch (const HTTPException& e)
        {
            res.status = 500;
            resJson["success"] = false;
            resJson["message"] = e.what();
            QJsonDocument doc(resJson);
            res.body = doc.toJson().toStdString();
            res.set_header("Content-Type", "application/json;charset=UTF-8");
            qDebug() << e.what();
        }

        });
}

bool btyGoose::HTTPServer::getPay(double pay)
{
    return false;
}

QString btyGoose::HTTPServer::DishListToJsonArray(const QList<data::Dish>& dishList)
{
    QJsonArray jsonArray;

    // 遍历 dishList，将每个 Dish 对象的 JSON 添加到 jsonArray 中
    for (const auto& dish : dishList)
    {
        // 将每个 Dish 对象转换为 JSON 字符串并加入到 JSON 数组中
        jsonArray.append(QJsonDocument::fromJson(dish.toJson().toUtf8()).object());
    }

    // 将 QJsonArray 转换为 JSON 字符串
    QJsonDocument doc(jsonArray);
    return QString(doc.toJson(QJsonDocument::Compact));
}

QList<btyGoose::data::Dish> btyGoose::HTTPServer::DishListFromJsonArray(const QString& jsonString)
{
    QList<data::Dish> dishList;

    // 解析 JSON 字符串为 QJsonDocument
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8());

    // 如果文档有效且是一个 JSON 数组
    if (doc.isArray()) {
        QJsonArray jsonArray = doc.array();

        // 遍历数组中的每个元素
        for (const QJsonValue& value : jsonArray) {
            if (value.isObject()) {
                // 通过 Dish 类的 fromJson 方法将 QJsonObject 转换为 Dish 对象
                QJsonDocument doc(value.toObject());
                data::Dish dish;
                dish.loadFromJson(QString(doc.toJson()).toStdString());
                dishList.append(dish);
            }
        }
    }

    return dishList;
}

QString btyGoose::HTTPServer::OrderListToJsonArray(const QList<data::Order>& orderList)
{
    QJsonArray jsonArray;

    // 遍历 orderList，将每个 order 对象的 JSON 添加到 jsonArray 中
    for (const auto& order : orderList)
    {
        // 将每个 order 对象转换为 JSON 字符串并加入到 JSON 数组中
        jsonArray.append(QJsonDocument::fromJson(order.toJson().toUtf8()).object());
    }

    // 将 QJsonArray 转换为 JSON 字符串
    QJsonDocument doc(jsonArray);
    return QString(doc.toJson(QJsonDocument::Compact));
}

QList<btyGoose::data::Order> btyGoose::HTTPServer::OrderListFromJsonArray(const QString& jsonString)
{
    QList<btyGoose::data::Order> orderList;

    // 解析 JSON 字符串为 QJsonDocument
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8());

    // 如果文档有效且是一个 JSON 数组
    if (doc.isArray()) {
        QJsonArray jsonArray = doc.array();

        // 遍历数组中的每个元素
        for (const QJsonValue& value : jsonArray) {
            if (value.isObject()) {
                // 通过 Dish 类的 fromJson 方法将 QJsonObject 转换为 Dish 对象
                QJsonDocument doc(value.toObject());
                data::Order order;
                order.loadFromJson(QString(doc.toJson()).toStdString());
                orderList.append(order);
            }
        }
    }

    return orderList;
}

QString btyGoose::HTTPServer::OrderDishListToJsonArray(const QList<data::OrderDish>& dishList)
{
    QJsonArray jsonArray;

    // 遍历 orderList，将每个 order 对象的 JSON 添加到 jsonArray 中
    for (const auto& dish : dishList)
    {
        // 将每个 order 对象转换为 JSON 字符串并加入到 JSON 数组中
        jsonArray.append(QJsonDocument::fromJson(dish.toJson().toUtf8()).object());
    }

    // 将 QJsonArray 转换为 JSON 字符串
    QJsonDocument doc(jsonArray);
    return QString(doc.toJson(QJsonDocument::Compact));
}
