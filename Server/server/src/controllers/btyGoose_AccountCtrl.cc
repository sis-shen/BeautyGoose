#include "btyGoose_AccountCtrl.h"

using namespace btyGoose;

// Add definition of your processing function here
void AccountCtrl::accountLoginUsername(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    SUP_LOG_DEBUG("收到HTTP请求, method={},path={}","Post",req->getPath());

    std::string jsonStr = req->getBody().data();
    Json::Value jsonObj,resJson;
    Json::Reader reader;

    // 解析JSON请求体
    if (!reader.parse(jsonStr, jsonObj))
    {
        SUP_LOG_ERROR("[用户名登录] 无效的JSON格式: {}", jsonStr);
        resJson["success"] = false;
        resJson["message"] = "无效的JSON格式";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
        resp->setStatusCode(k400BadRequest);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        callback(resp);
        return;
    }

    if (!jsonObj.isMember("name"))
    {
        SUP_LOG_ERROR("[用户名登录] 不存在字段: name, 报文:{}", jsonStr);
        resJson["success"] = false;
        resJson["message"] = "无效的JSON格式";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
        resp->setStatusCode(k400BadRequest);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        callback(resp);
        return;
    }

    data::Account record_acc;
    std::string name = jsonObj["name"].asString();
    record_acc = redis->getAccountByName(name);
    if (record_acc.uuid.empty())
    {
        // 缓存不命中
        record_acc = db->searchAccountByName(name);
        if (!record_acc.uuid.empty())
        {
            redis->setAccount(record_acc);
        }
    }

    if (!record_acc.name.empty() && jsonObj["password"].asString() == record_acc.password)
    {
        // 登录成功响应
        resJson["success"] = true;
        resJson["message"] = "登录认证成功";
        resJson["account"] = json::toJson(record_acc);
        SUP_LOG_INFO("登录成功,id:{}", record_acc.uuid);

    }
    else
    {
        // 登录失败响应
        resJson["success"] = false;
        resJson["message"] = "登录认证失败";
        SUP_LOG_INFO("登录失败,id:{}", record_acc.uuid);
    }
    auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
    resp->setStatusCode(k200OK);
    resp->setContentTypeCode(CT_APPLICATION_JSON);
    callback(resp);

}
void AccountCtrl::accountLoginPhone(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    SUP_LOG_DEBUG("收到HTTP请求, method={},path={}","Post",req->getPath());

    std::string jsonStr = req->getBody().data();
    Json::Value jsonObj,resJson;
    Json::Reader reader;
    // 解析JSON请求体
    if (!reader.parse(jsonStr, jsonObj))
    {
        SUP_LOG_ERROR("[用户名登录] 无效的JSON格式: {}", jsonStr);
        resJson["success"] = false;
        resJson["message"] = "无效的JSON格式";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
        resp->setStatusCode(k400BadRequest);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        callback(resp);
        return;
    }

    if (!jsonObj.isMember("phone"))
    {
        SUP_LOG_ERROR("[用户名登录] 不存在字段: phone, 报文:{}", jsonStr);
        resJson["success"] = false;
        resJson["message"] = "无效的JSON格式";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
        resp->setStatusCode(k400BadRequest);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        callback(resp);
        return;
    }

    data::Account record_acc;
    std::string phone = jsonObj["phone"].asString();
    record_acc = redis->getAccountByPhone(phone);
    if (record_acc.uuid.empty())
    {
        // 缓存不命中
        record_acc = db->searchAccountByPhone(phone);
        if (!record_acc.uuid.empty())
        {
            redis->setAccount(record_acc);
        }
    }

    if (!record_acc.name.empty() && jsonObj["password"].asString() == record_acc.password)
    {
        // 登录成功响应
        resJson["success"] = true;
        resJson["message"] = "登录认证成功";
        resJson["account"] = json::toJson(record_acc);
        SUP_LOG_INFO("登录成功,id:{}", record_acc.uuid);

    }
    else
    {
        // 登录失败响应
        resJson["success"] = false;
        resJson["message"] = "登录认证失败";
        SUP_LOG_INFO("登录失败,id:{}", record_acc.uuid);
    }
    auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
    resp->setStatusCode(k200OK);
    resp->setContentTypeCode(CT_APPLICATION_JSON);
    callback(resp);

}
void AccountCtrl::accountRegister(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    SUP_LOG_DEBUG("收到HTTP请求, method={},path={}","Post",req->getPath());

    std::string jsonStr = req->getBody().data();
    Json::Value jsonObj,resJson;
    Json::Reader reader;

    if (!reader.parse(jsonStr, jsonObj)) {
        SUP_LOG_ERROR("[账户注册]无效的JSON格式:{}",jsonStr);
        resJson["success"] = false;
        resJson["message"] = "无效的JSON格式";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
        resp->setStatusCode(k400BadRequest);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        callback(resp);
        return;
    }

    data::Account acc;
    acc.name = jsonObj["name"].asString();
    acc.password = jsonObj["password"].asString();
    acc.phone = jsonObj["phone"].asString();
    acc.nickname = jsonObj["nickname"].asString();
    acc.type = static_cast<data::Account::Type>(jsonObj["type"].asInt());
    acc.level = data::Account::Level::MEMBER;
    acc.uuid = util::makeId('A');

    // 验证UUID生成
    if (acc.uuid.empty())
    {
        SUP_LOG_ERROR("[账户注册]UUID生成失败:空的uuid");
        resJson["message"] = "UUID生成错误";
        resJson["success"] = false;
        auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
        resp->setStatusCode(k500InternalServerError);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        callback(resp);
        return;
    }

    // 验证码校验
    if (!AuthenticateAuthCode(acc.phone, jsonObj["auth_code"].asString()))
    {
        SUP_LOG_INFO("[账户注册]验证码错误,注册失败");
        resJson["message"] = "注册失败,验证码错误";
        resJson["success"] = false;
        auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
        resp->setStatusCode(k200OK);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        return;
    }

    // 用户名查重
    if (!db->searchAccountByName(acc.name).name.empty())
    {
        LOG_INFO("[账户注册]失败：用户名已存在");
        resJson["message"] = "注册失败,用户名已存在:" + acc.name;
        resJson["success"] = false;
        auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
        resp->setStatusCode(k200OK);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        return;
    }

    // 手机号查重
    if (!db->searchAccountByPhone(acc.phone).name.empty())
    {
        LOG_INFO("[账户注册]失败：手机号已存在");
        resJson["message"] = "注册失败,手机号已被占用:" + acc.phone;
        resJson["success"] = false;
        auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
        resp->setStatusCode(k200OK);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        return;
    }

    // 创建账户
    if (!db->addAccount(acc))
    {
        SUP_LOG_ERROR("[账户注册]新用户插入数据库失败");
        resJson["message"] = "UUID生成错误";
        resJson["success"] = false;
        auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
        resp->setStatusCode(k500InternalServerError);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        callback(resp);
        return;
    }

    redis->setAccount(acc);

    SUP_LOG_INFO("[账户注册]成功,id:", acc.uuid);
    resJson["success"] = true;
    resJson["message"] = "注册成功";
    auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
    resp->setStatusCode(k200OK);
    resp->setContentTypeCode(CT_APPLICATION_JSON);
    callback(resp);
}
void AccountCtrl::accountUpdateLevel(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    SUP_LOG_DEBUG("收到HTTP请求, method={},path={}","Post",req->getPath());

    std::string jsonStr = req->getBody().data();
    Json::Value jsonObj,resJson;
    Json::Reader reader;

    if (!reader.parse(jsonStr, jsonObj))
    {
        SUP_LOG_ERROR("[账户等级更新]无效的JSON格式:{}", jsonStr);

        resJson["success"] = false;
        resJson["message"] = "无效的JSON格式";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
        resp->setStatusCode(k400BadRequest);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        callback(resp);
        return;
    }

    data::Account acc;
    std::string id = jsonObj["id"].asString();
    acc = redis->getAccountById(id);
    if (acc.uuid.empty())
        acc = db->searchAccountByID(id);
    std::string level = jsonObj["level"].asString();

    SUP_LOG_INFO("[用户等级更新]id:{},{}->{}", acc.uuid, acc.level, level);

    if (level == "VIP")
    {
        acc.level = data::Account::Level::VIP;
    }
    else if (level == "SUVIP")
    {
        acc.level = data::Account::Level::SUVIP;
    }
    else
    {
        SUP_LOG_ERROR("未知的等级类型: {}",level);
        resJson["success"] = false;
        resJson["message"] = "未知等级类型: "+ level;
        auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
        resp->setStatusCode(k400BadRequest);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        callback(resp);
        return;
    }

    db->updateAccount(acc);
    redis->setAccount(acc);
    resJson["level"] = static_cast<int>(acc.level);
    SUP_LOG_INFO("[用户等级更新]成功");
    auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
    resp->setStatusCode(k200OK);
    resp->setContentTypeCode(CT_APPLICATION_JSON);
    callback(resp);
}
void AccountCtrl::accountUpdateNickname(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    SUP_LOG_DEBUG("收到HTTP请求, method={},path={}","Post",req->getPath());

    std::string jsonStr = req->getBody().data();
    Json::Value jsonObj,resJson;
    Json::Reader reader;

    // 解析JSON请求
    if (!reader.parse(jsonStr, jsonObj))
    {
        SUP_LOG_ERROR("[账户更新昵称]无效的JSON格式:{}", jsonStr);

        resJson["success"] = false;
        resJson["message"] = "无效的请求格式";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
        resp->setStatusCode(k400BadRequest);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        callback(resp);
        return;
    }

    // 参数校验
    if (!jsonObj.isMember("id") || !jsonObj.isMember("nickname"))
    {
        std:string resStr;
        if(!jsonObj.isMember("id"))
            resStr += "缺少必要字段id ";
        if(!jsonObj.isMember("nickname"))
            resStr += "缺少必要字段nickname";

        resJson["message"] = resStr;
        auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
        resp->setStatusCode(k400BadRequest);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        callback(resp);
    }

    // 获取用户信息
    std::string userId = jsonObj["id"].asString();
    std::string newNickname = jsonObj["nickname"].asString();

    SUP_LOG_INFO("[用户昵称更新]id:{},?->{}", userId, newNickname);

    // 查询并更新账户
    data::Account acc;
    acc = redis->getAccountById(userId);
    if (acc.uuid.empty())
        acc = db->searchAccountByID(userId);
    acc.nickname = newNickname;

    if (!db->updateAccount(acc))
    {
        SUP_LOG_ERROR("[用户昵称更新]新昵称更新数据库失败");
        resJson["message"] = "UUID生成错误";
        resJson["success"] = false;
        auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
        resp->setStatusCode(k500InternalServerError);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        callback(resp);
        return;
    }

    // 验证更新结果
    data::Account updatedAcc = db->searchAccountByID(userId);
    redis->setAccount(updatedAcc);
    resJson["nickname"] = updatedAcc.nickname;
    resJson["success"] = true;
    SUP_LOG_INFO("[用户昵称更新]成功，id:{}", updatedAcc.uuid);
    auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
    resp->setStatusCode(k200OK);
    resp->setContentTypeCode(CT_APPLICATION_JSON);
    callback(resp);

}

bool AccountCtrl::AuthenticateAuthCode(const string& phone, const string& auth_code)
{
////////////////////////////////////////
//      等待接入第三方SDK
/////////////////////////////////////////
    return true;
}