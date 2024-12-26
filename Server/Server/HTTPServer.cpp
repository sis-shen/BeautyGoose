#include "HTTPServer.h"

btyGoose::HTTPServer::HTTPServer()
{
    loadConfig();
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
                resJson["message"] = "login success!";

                resJson["success"] = true;
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
                resJson["message"] = "login success!";

                resJson["success"] = true;
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
        try {
            if (jsonObj.isEmpty())
            {
                throw HTTPException("Json Serialization failed");
            }

            data::Account acc;
            acc.loadFromJson(req.body);

            data::Account record_acc = db.searchAccountByID(acc.uuid);
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

void btyGoose::HTTPServer::initConsumerAPI()
{

}

void btyGoose::HTTPServer::initMerchantAPI()
{
}

void btyGoose::HTTPServer::initAdminAPI()
{
}
