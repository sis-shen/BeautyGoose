#pragma once

#include <drogon/HttpController.h>
#include "SharedResources.h"

using namespace drogon;

namespace btyGoose
{
class AccountCtrl : public drogon::HttpController<AccountCtrl>
{
  public:
  AccountCtrl()
  {
    redis = SR::ins().redis();
    db = SR::ins().db();
  }
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(AccountCtrl::accountLoginUsername,"/account/login/username",Post);
    ADD_METHOD_TO(AccountCtrl::accountLoginPhone,"/account/login/phone",Post);
    ADD_METHOD_TO(AccountCtrl::accountRegister,"/account/register",Post);
    ADD_METHOD_TO(AccountCtrl::accountUpdateLevel,"/account/update/level",Post);
    ADD_METHOD_TO(AccountCtrl::accountUpdateNickname,"/account/update/nickname",Post);

    METHOD_LIST_END
    void accountLoginUsername(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    void accountLoginPhone(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    void accountRegister(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    void accountUpdateLevel(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    void accountUpdateNickname(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

    bool AuthenticateAuthCode(const string& phone, const string& auth_code);
  
    private:
    std::shared_ptr<RedisClient> redis;
    std::shared_ptr<DatabaseClient> db;
};
}
