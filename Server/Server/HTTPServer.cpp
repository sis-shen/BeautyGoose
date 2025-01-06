#include "HTTPServer.h"

btyGoose::HTTPServer::HTTPServer()
{
    loadConfig();
    initTestAPI();
    initAccountAPI();
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
        qDebug() << "/account/login/username get a post!";
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
            qDebug() << "开始查重用户名" << acc.name;
            if (!db.searchAccountByName(acc.name).name.isEmpty())
            {
                //账户名重复
                res.status = 200;
                resJson["message"] = "账户名已存在!";
                resJson["success"] = false;
                QJsonDocument doc(resJson);
                res.body = doc.toJson().toStdString();
                return;
            }

            if (!db.searchAccountByPhone(acc.phone).name.isEmpty())
            {
                //电话重复
                res.status = 200;
                resJson["message"] = "手机号已被占用!";
                resJson["success"] = false;
                QJsonDocument doc(resJson);
                res.body = doc.toJson().toStdString();
                return;
            }

            if (db.searchAccountByID(acc.uuid).name.isEmpty())
            {
                if (!db.addAccount(acc))
                {
                    throw HTTPException("databse add account failed");
                }
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

}

void btyGoose::HTTPServer::initMerchantAPI()
{
}

void btyGoose::HTTPServer::initAdminAPI()
{
}

bool btyGoose::HTTPServer::getPay(double pay)
{
    return false;
}
