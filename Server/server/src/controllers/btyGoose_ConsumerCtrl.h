#pragma once

#include <drogon/HttpController.h>
#include "SharedResources.h"
using namespace drogon;

namespace btyGoose
{
class ConsumerCtrl : public drogon::HttpController<ConsumerCtrl>
{
  public:
  ConsumerCtrl()
  {
    redis = SR::ins().redis();
    db = SR::ins().db();
  }

  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(ConsumerCtrl::consumerOrderDetailDishlist,"/consumer/order/detail/dishlist",Post);
    ADD_METHOD_TO(ConsumerCtrl::consumerDishList,"/consumer/dish/list",Post);
    ADD_METHOD_TO(ConsumerCtrl::consumerDishDishInfo,"/consumer/dish/dishInfo",Post);
    ADD_METHOD_TO(ConsumerCtrl::consumerOrderGenerate,"/consumer/order/generate",Post);
    ADD_METHOD_TO(ConsumerCtrl::consumerOrderList,"/consumer/order/list",Post);
    ADD_METHOD_TO(ConsumerCtrl::consumerOrderPayConfirm,"/consumer/order/pay/confirm",Post);
    ADD_METHOD_TO(ConsumerCtrl::consumerOrderCancel,"/consumer/order/cancel",Post);
    METHOD_LIST_END
    void consumerOrderDetailDishlist(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    void consumerDishList(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    void consumerDishDishInfo(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    void consumerOrderGenerate(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    void consumerOrderList(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    void consumerOrderPayConfirm(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    void consumerOrderCancel(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

    private:
    std::shared_ptr<RedisClient> redis;
    std::shared_ptr<DatabaseClient> db;
};
}
