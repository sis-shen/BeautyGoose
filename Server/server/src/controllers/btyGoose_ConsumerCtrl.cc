#include "btyGoose_ConsumerCtrl.h"

using namespace btyGoose;

void ConsumerCtrl::consumerOrderDetailDishlist(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    SUP_LOG_DEBUG("收到HTTP请求, method={},path={}","Post",req->getPath());

    std::string jsonStr = req->getBody().data();
    Json::Value jsonObj,resJson;
    Json::Reader reader;

    if (!reader.parse(jsonStr, jsonObj)) {
        SUP_LOG_ERROR("[消费者获取订单菜品列表]无效的JSON格式:{}",jsonStr);
        resJson["success"] = false;
        resJson["message"] = "无效的JSON格式";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
        resp->setStatusCode(k400BadRequest);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        callback(resp);
        return;
    }

    if (!jsonObj.isMember("order_id"))
    {
        SUP_LOG_ERROR("[消费者获取订单菜品列表] 不存在字段: order_id, 报文:{}", jsonStr);
        resJson["success"] = false;
        resJson["message"] = "无效的JSON格式";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
        resp->setStatusCode(k400BadRequest);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        callback(resp);
        return;
    }

    std::string order_id = jsonObj["order_id"].asString();
    std::string dish_list_json = redis->getOrderDishListJson(order_id);
    if(dish_list_json.empty())
    {
        std::vector<data::OrderDish> dishList = db->getOrderDishesByID(order_id);
        std::string dishListJson = json::OrderDishListToJsonArray(dishList);
        resJson["dish_list"] = dishListJson;
        redis->setOrderDishListJson(order_id,dishListJson);

    }
    else 
    {
        resJson["dish_list"] = dish_list_json;
    }
    SUP_LOG_INFO("[消费者获取订单菜品列表]成功,order_id:{}",order_id);
    auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
    resp->setStatusCode(k200OK);
    resp->setContentTypeCode(CT_APPLICATION_JSON);
    callback(resp);
}
void ConsumerCtrl::consumerDishList(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    SUP_LOG_DEBUG("收到HTTP请求, method={},path={}","Post",req->getPath());

    std::vector<data::Dish> dishList = db->getAllDishList();
    std::string dishListJson = json::DishListToJsonArray(dishList);
    Json::Value resJson;
    resJson["dish_list"] = dishListJson;
    SUP_LOG_INFO("[消费者获取菜品列表]成功,总数:{}",dishList.size());
    auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
    resp->setStatusCode(k200OK);
    resp->setContentTypeCode(CT_APPLICATION_JSON);
    callback(resp);
}
void ConsumerCtrl::consumerDishDishInfo(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    SUP_LOG_DEBUG("收到HTTP请求, method={},path={}","Post",req->getPath());

    std::string jsonStr = req->getBody().data();
    Json::Value jsonObj,resJson;
    Json::Reader reader;

    if (!reader.parse(jsonStr, jsonObj)) {
        SUP_LOG_ERROR("[消费者获取菜品详情]无效的JSON格式:{}",jsonStr);
        resJson["success"] = false;
        resJson["message"] = "无效的JSON格式";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
        resp->setStatusCode(k400BadRequest);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        callback(resp);
        return;
    }

    std::string dish_id = jsonObj["dish_id"].asString();
    data::Dish dish;
    dish = redis->getDishById(dish_id);
    if (dish.uuid.empty())
    {
        dish = db->searchDishByID(dish_id);
        if (!dish.uuid.empty())
        {
            redis->setDish(dish);
        }
    }

    if (!dish.uuid.empty())
    {
        resJson["dish"] = json::toJson(dish);
        SUP_LOG_INFO("[消费者获取菜品详情]成功,dish id:{}", dish_id);
        auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
        resp->setStatusCode(k200OK);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        callback(resp);
        return;
    }
    else
    {
        resJson["success"] = false;
        resJson["message"] = "菜品不存在";
        SUP_LOG_WARN("[消费者获取菜品详情]出错，菜品不存在");
        auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
        resp->setStatusCode(k500InternalServerError);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        callback(resp);
        return;
    }
}
void ConsumerCtrl::consumerOrderGenerate(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    SUP_LOG_DEBUG("收到HTTP请求, method={},path={}","Post",req->getPath());

    std::string jsonStr = req->getBody().data();
    Json::Value jsonObj,resJson;
    Json::Reader reader;

    if (!reader.parse(jsonStr, jsonObj)) {
        SUP_LOG_ERROR("[消费者订单生成] 无效的JSON格式: {}", jsonStr);
        resJson["success"] = false;
        resJson["message"] = "无效的JSON格式";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
        resp->setStatusCode(k400BadRequest);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        callback(resp);
        return;
    }

    data::Order order;
    order.uuid = util::makeId('O');
    order.merchant_id = jsonObj["merchant_id"].asString();
    order.merchant_name = jsonObj["merchant_name"].asString();
    order.consumer_id = jsonObj["consumer_id"].asString();
    order.consumer_name = jsonObj["consumer_name"].asString();
    order.level = static_cast<data::Account::Level>(jsonObj["level"].asInt());
    order.pay = jsonObj["pay"].asDouble();
    order.sum = jsonObj["count"].asInt();
    order.time = std::to_string(util::getSecTime());
    order.status = data::Order::Status::UNPAYED;

    SUP_LOG_DEBUG("[生成订单]order id:{}", order.uuid);

    // 先插入订单
    bool ok = db->addOrder(order);
    if (!ok)
    {
        SUP_LOG_ERROR("订单生成失败,id: {},来自消费者:{}", order.uuid, order.consumer_id);
    }
    // 如果缓存不存在，则重新全部加载
    if (SR::ins().checkRedisOrderListExist(order))
    {
        redis->setOrder(order);
    }
    std::vector<data::OrderDish> dish_list;
    Json::Value jsonArr = jsonObj["dish_arr"];
    for (const auto &value : jsonArr)
    {
        data::OrderDish dish;
        dish.order_id = order.uuid;
        dish.merchant_id = order.merchant_id;
        dish.name = value["dish_name"].asString();
        dish.dish_price = value["dish_price"].asDouble();
        dish.count = value["count"].asInt();
        dish.dish_id = value["dish_id"].asString();
        dish_list.push_back(dish);
        SUP_LOG_TRACE("生成一个OrderDish, dish id:{}", dish.dish_id);
    }
    ok = db->addOrderDishesByID(order.uuid, dish_list);
    if (!ok)
        SUP_LOG_ERROR("订单菜品插入失败,order_id: {},来自消费者:{}", order.uuid, order.consumer_id);
    Json::StreamWriterBuilder dish_writer;
    redis->setOrderDishListJson(order.uuid, json::OrderDishListToJsonArray(dish_list));

    resJson["order"] = json::toJson(order);
    SUP_LOG_INFO("[订单生成]成功,order_id:{}", order.uuid);
    auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
    resp->setStatusCode(k200OK);
    resp->setContentTypeCode(CT_APPLICATION_JSON);
    callback(resp);
}
void ConsumerCtrl::consumerOrderList(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    SUP_LOG_DEBUG("收到HTTP请求, method={},path={}","Post",req->getPath());

    std::string jsonStr = req->getBody().data();
    Json::Value jsonObj,resJson;
    Json::Reader reader;
    if (!reader.parse(jsonStr, jsonObj)) {
        SUP_LOG_ERROR("[消费者订单列表] 无效的JSON格式: {}", jsonStr);
        resJson["success"] = false;
        resJson["message"] = "无效的JSON格式";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
        resp->setStatusCode(k400BadRequest);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        callback(resp);
        return;
    }

    if (!jsonObj.isMember("consumer_id"))
    {
        SUP_LOG_ERROR("[消费者订单列表] 不存在字段: consumer_id, 报文:{}", jsonStr);
        resJson["success"] = false;
        resJson["message"] = "无效的JSON格式";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
        resp->setStatusCode(k400BadRequest);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        callback(resp);
        return;
    }

    std::string consumer_id = jsonObj["consumer_id"].asString();
    std::vector<data::Order> orderList;
    if (redis->hasOrderListByUserId(consumer_id))
    {
        orderList = redis->getOrderListByConsumer(consumer_id);
    }
    // 缓存不命中或者缓存为空
    if (orderList.empty())
    {
        orderList = db->getOrderListByConsumer(consumer_id);
        redis->setOrderList(orderList);
        redis->setOrderListByIdDone(consumer_id);
    }
    // std::cout << "consumer:" << consumer_id << " find orders " << orderList.size() << "\n";
    std::string dishListJson = json::OrderListToJsonArray(orderList);
    resJson["order_list"] = dishListJson;
    SUP_LOG_INFO("[消费者获取订单列表]成功，总数:{}", orderList.size());
    auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
    resp->setStatusCode(k200OK);
    resp->setContentTypeCode(CT_APPLICATION_JSON);
    callback(resp);

}
void ConsumerCtrl::consumerOrderPayConfirm(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    SUP_LOG_DEBUG("收到HTTP请求, method={},path={}","Post",req->getPath());

    std::string jsonStr = req->getBody().data();
    Json::Value jsonObj,resJson;
    Json::Reader reader;

    if (!reader.parse(jsonStr, jsonObj)) {
        SUP_LOG_ERROR("[消费者订单确认] 无效的JSON格式: {}", jsonStr);
        resJson["success"] = false;
        resJson["message"] = "无效的JSON格式";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
        resp->setStatusCode(k400BadRequest);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        callback(resp);
        return;
    }

    if (!jsonObj.isMember("order_id"))
    {
        SUP_LOG_ERROR("[消费者订单确认] 不存在字段: order_id, 报文:{}", jsonStr);
        resJson["success"] = false;
        resJson["message"] = "无效的JSON格式";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
        resp->setStatusCode(k400BadRequest);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        callback(resp);
        return;
    }
    data::Order order;
    std::string order_id = jsonObj["order_id"].asString();

    order = redis->getOrderById(order_id);
    if (order.uuid.empty())
    {
        order = db->searchOrderByID(order_id);
    }
    order.status = data::Order::Status::WAITING;
    db->updateOrder(order);
    redis->setOrder(order);
    SUP_LOG_INFO("[消费者确认支付订单]成功,order id", jsonObj["order_id"].asString());
    auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
    resp->setStatusCode(k200OK);
    resp->setContentTypeCode(CT_APPLICATION_JSON);
    callback(resp);
}
void ConsumerCtrl::consumerOrderCancel(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    SUP_LOG_DEBUG("收到HTTP请求, method={},path={}","Post",req->getPath());

    std::string jsonStr = req->getBody().data();
    Json::Value jsonObj,resJson;
    Json::Reader reader;

    if (!reader.parse(jsonStr, jsonObj)) {
        SUP_LOG_ERROR("[消费者订单取消] 无效的JSON格式: {}", jsonStr);
        resJson["success"] = false;
        resJson["message"] = "无效的JSON格式";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
        resp->setStatusCode(k400BadRequest);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        callback(resp);
        return;
    }

    if (!jsonObj.isMember("order_id"))
    {
        SUP_LOG_ERROR("[消费者订单取消] 不存在字段: order_id, 报文:{}", jsonStr);
        resJson["success"] = false;
        resJson["message"] = "无效的JSON格式";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
        resp->setStatusCode(k400BadRequest);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        callback(resp);
        return;
    }

    data::Order order;
    std::string order_id = jsonObj["order_id"].asString();
    order = redis->getOrderById(order_id);
    if (order.uuid.empty())
    {
        order = db->searchOrderByID(order_id);
    }
    std::string reason;
    if (order.status == data::Order::Status::UNPAYED)
    {
        order.status = data::Order::Status::CANCELED;
        db->updateOrder(order);
        reason = "订单取消成功";
    }
    else if (order.status == data::Order::Status::WAITING)
    {
        reason = "订单无法取消：已支付";
    }
    else
    {
        reason = "订单无法支付,订单状态码:" + std::to_string(static_cast<int>(order.status));
    }
    redis->setOrder(order);

    resJson["reason"] = reason;
    auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
    resp->setStatusCode(k200OK);
    resp->setContentTypeCode(CT_APPLICATION_JSON);
    callback(resp);
}

