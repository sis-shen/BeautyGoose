#pragma once

#include <drogon/HttpController.h>
#include "SharedResources.h"

using namespace drogon;

namespace btyGoose
{
  class MerchantCtrl : public drogon::HttpController<MerchantCtrl>
  {
  public:
    MerchantCtrl()
    {
      redis = SR::ins().redis();
      db = SR::ins().db();
    }
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(MerchantCtrl::merchantDishInfoDebug,"/debug/merchant/dish/info",Post);

    METHOD_LIST_END
    void merchantDishInfoDebug(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

    void merchantDishList(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    void merchantDishRegister(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    void merchantDishInfo(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    void merhcnatDishUpdate(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    void merhcnatDishDel(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    void merhcnatOrderList(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    void merhcnatOrderDetailDishList(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    void merhcnatOrderAccept(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    void merchantOrderReject(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);


  private:
    std::shared_ptr<RedisClient> redis;
    std::shared_ptr<DatabaseClient> db;
  };
}
