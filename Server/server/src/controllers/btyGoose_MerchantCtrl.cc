#include "btyGoose_MerchantCtrl.h"
#include <drogon/HttpResponse.h>

using namespace btyGoose;
using namespace drogon;

// Add definition of your processing function here
void MerchantCtrl::merchantDishInfoDebug(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    SUP_LOG_DEBUG("收到HTTP请求, method={},path={}","Post",req->getPath());
    ScopedTimer timer("菜品详情查询");
    std::string jsonStr = req->getBody().data();
    Json::Value jsonObj;
    Json::Reader reader;
    
    if (!reader.parse(jsonStr, jsonObj)) {
        SUP_LOG_ERROR("[商家查询菜品详情] uuid生成错误");
        jsonObj = Json::objectValue;
    }
    ///////////////////////
    SUP_LOG_TRACE("Json解析完成,    耗时 {}μs",timer.staged());
    //////////////////////////
    Json::Value resJson;
    
    auto missing_dish = [&resJson,&callback](){
        resJson["success"] = false;
        resJson["message"] = "菜品不存在";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        resp->setStatusCode(k404NotFound);
        callback(resp);
        SUP_LOG_ERROR("[商家查询菜品详情]出错,信息:{}","菜品不存在");
    };

    if (!jsonObj.isMember("dish_id")) {
        missing_dish();
        return;
    }

    std::string dish_id = jsonObj["dish_id"].asString();
    data::Dish dish;
    //////////////////
    timer.staged();
    /////////////////
    dish = redis->getDishById(dish_id);
    //////////////////
    SUP_LOG_TRACE("完成Redis的get查询,  耗时 {}μs",timer.staged());
    /////////////////
    if(dish.uuid.empty())
    {
        timer.staged();
        dish = db->searchDishByID(dish_id);
        SUP_LOG_TRACE("完成数据库查询数据，耗时 {}μs",timer.staged());
        if (dish.uuid.empty()) {
            missing_dish();
            return;
        }
        timer.staged();
        redis->setDish(dish);
        SUP_LOG_TRACE("完成Redis设置缓存，耗时 {}μs",timer.staged());
    }
    timer.staged();
    resJson["dish"] = json::toJson(dish);
    auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
    SUP_LOG_TRACE("完成Json序列化和报文写入,耗时 {}μs",timer.staged());
    resp->setContentTypeCode(CT_APPLICATION_JSON);
    resp->setStatusCode(k200OK);
    SUP_LOG_INFO("[商家查询菜品详情]成功");
    callback(resp);
}

void MerchantCtrl::merchantDishList(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    SUP_LOG_DEBUG("收到HTTP请求, method={},path={}","Post",req->getPath());

    std::string jsonStr = req->getBody().data();
    Json::Value jsonObj;
    Json::Reader reader;
    if (!reader.parse(jsonStr, jsonObj)) {
        SUP_LOG_ERROR("[商家获取菜品列表]无效的JSON格式:{}",jsonStr);
        // std::cerr << "Invalid Json: " << jsonStr << "\n";
        jsonObj = Json::objectValue;
    }
    Json::Value resJson;
        if (jsonObj.empty()) {
            resJson["message"] = "Json序列化解析失败";
            auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
            resp->setStatusCode(k400BadRequest);
            resp->setContentTypeCode(CT_APPLICATION_JSON);
            callback(resp);
            return;
        }
        
        std::string merchant_id = jsonObj["merchant_id"].asString();
        std::string dishListJson = redis->getDishListJsonByMerchant(merchant_id); 
        SUP_LOG_TRACE("Redis获取到的菜品列表json: {}",dishListJson);
        if(dishListJson.empty())
        {
            std::vector<data::Dish> dishList = db->getDishListByMerchant(merchant_id);
            dishListJson = json::DishListToJsonArray(dishList);
            SUP_LOG_TRACE("数据库查询到菜品数量:{} , 获得的菜品json: {}",dishList.size(),dishListJson);
            redis->setDishListJsonByMerchant(merchant_id,dishListJson);
        }
        resJson["dish_list"] = dishListJson;
        auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
        resp->setStatusCode(k200OK);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        SUP_LOG_INFO("[商家获取菜品列表]成功");
        callback(resp);
}


void MerchantCtrl::merchantDishRegister(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    SUP_LOG_DEBUG("收到HTTP请求, method={},path={}","Post",req->getPath());
    std::string jsonStr = req->getBody().data();
    Json::Value jsonObj;
    Json::Reader reader;
    if (!reader.parse(jsonStr, jsonObj)) {
        SUP_LOG_ERROR("[商家注册菜品]无效的JSON格式:{}",jsonStr);
        jsonObj.clear();
    }
    Json::Value resJson;
    if (jsonObj.empty()) {
        resJson["message"] = "Json序列化解析失败";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
        resp->setStatusCode(k400BadRequest);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        callback(resp);
        return;
    }

        data::Dish dish;
        dish.uuid = util::makeId('D');
        dish.name = jsonObj["name"].asString();
        dish.merchant_id = jsonObj["merchant_id"].asString();
        dish.merchant_name = jsonObj["merchant_name"].asString();
        dish.icon_path = jsonObj["icon_path"].asString();
        dish.description = jsonObj["introduction"].asString();
        dish.base_price = jsonObj["price"].asDouble();
        dish.price_factor = jsonObj["price_factor"].asDouble();
        dish.is_delete = false;

        if (dish.uuid.empty()) {
            resJson["message"] = "uuid生成错误";
            auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
            resp->setStatusCode(k500InternalServerError);
            resp->setContentTypeCode(CT_APPLICATION_JSON);
            callback(resp);
            return;
        }

        if (db->searchDishByID(dish.uuid).name.empty()) {
            if (!db->addDish(dish)) {
                resJson["message"] = "数据库添加菜品出错";
                auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
                resp->setStatusCode(k500InternalServerError);
                resp->setContentTypeCode(CT_APPLICATION_JSON);
                callback(resp);
                return;
            }
            redis->setDish(dish);
            redis->delDishListJsonByMerchant(dish.merchant_id);//缓存失效
            // std::cout << "成功向数据库插入新菜品 " << dish.name << "\n";
            resJson["success"] = true;
            resJson["message"] = "菜品创建成功";
            auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
            resp->setStatusCode(k200OK);
            resp->setContentTypeCode(CT_APPLICATION_JSON);
            SUP_LOG_INFO("[商家注册菜品]成功,dish id:{}",dish.uuid);
            callback(resp);
        } else {
            resJson["message"] = "uuid生成重复";
            SUP_LOG_ERROR("[商家注册菜品] uuid生成重复");
            auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
            resp->setStatusCode(k500InternalServerError);
            resp->setContentTypeCode(CT_APPLICATION_JSON);
            callback(resp);
            return;
        }
    }
void MerchantCtrl::merchantDishInfo(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    SUP_LOG_DEBUG("收到HTTP请求, method={},path={}","Post",req->getPath());
    std::string jsonStr = req->getBody().data();
    Json::Value jsonObj;
    Json::Reader reader;
    
    if (!reader.parse(jsonStr, jsonObj)) {
        SUP_LOG_ERROR("[商家查询菜品详情] uuid生成错误");
        jsonObj = Json::objectValue;
    }
    Json::Value resJson;
    
    auto missing_dish = [&resJson,&callback](){
        resJson["success"] = false;
        resJson["message"] = "菜品不存在";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        resp->setStatusCode(k404NotFound);
        callback(resp);
        SUP_LOG_ERROR("[商家查询菜品详情]出错,信息:{}","菜品不存在");
    };

    if (!jsonObj.isMember("dish_id")) {
        missing_dish();
        return;
    }

    std::string dish_id = jsonObj["dish_id"].asString();
    data::Dish dish;
    dish = redis->getDishById(dish_id);
    if(dish.uuid.empty())
    {
        dish = db->searchDishByID(dish_id);
        if (dish.uuid.empty()) {
            missing_dish();
            return;
        }
        redis->setDish(dish);
    }
    resJson["dish"] = json::toJson(dish);
    auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
    resp->setContentTypeCode(CT_APPLICATION_JSON);
    resp->setStatusCode(k200OK);
    SUP_LOG_INFO("[商家查询菜品详情]成功");
    callback(resp);
}
void MerchantCtrl::merhcnatDishUpdate(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    SUP_LOG_DEBUG("收到HTTP请求, method={},path={}","Post",req->getPath());
    std::string jsonStr = req->getBody().data();
    Json::Value jsonObj;
    Json::Reader reader;
    if (!reader.parse(jsonStr, jsonObj)) {
        SUP_LOG_ERROR("[商家更新菜品]无效的JSON格式:{}",jsonStr);
        jsonObj = Json::objectValue;
    }
    Json::Value resJson;

    if (jsonObj.empty() || !jsonObj.isMember("dish_id")) {
        if(jsonObj.empty())
            resJson["message"] = "Json序列化解析失败";
        else
            resJson["message"] = "缺少字段dish_id";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
        resp->setStatusCode(k400BadRequest);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        callback(resp);
        return;
    }

    std::string dish_id = jsonObj["dish_id"].asString();
    data::Dish dish;
    dish = redis->getDishById(dish_id);
    if(dish.uuid.empty())
    {
        dish = db->searchDishByID(dish_id);
    }
    // std::cout << "找到菜品ID: " << dish.uuid << "\n";

    dish.name = jsonObj["name"].asString();
    dish.merchant_id = jsonObj["merchant_id"].asString();
    dish.merchant_name = jsonObj["merchant_name"].asString();
    dish.icon_path = jsonObj["icon_path"].asString();
    dish.description = jsonObj["introduction"].asString();
    dish.base_price = jsonObj["price"].asDouble();
    dish.price_factor = jsonObj["price_factor"].asDouble();

    if (!dish.uuid.empty()) {
        if (!db->updateDish(dish)) {
            resJson["message"] = "数据库更新失败";
            auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
            resp->setStatusCode(k500InternalServerError);
            resp->setContentTypeCode(CT_APPLICATION_JSON);
            callback(resp);
            return;
        }
        redis->setDish(dish);
        redis->delDishListJsonByMerchant(dish.merchant_id);
        // std::cout << "成功更新菜品: " << dish.name << "\n";
        resJson["success"] = true;
        resJson["message"] = "菜品更新成功";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
        resp->setStatusCode(k200OK);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        SUP_LOG_INFO("[商家更新菜品]成功,dish id:{}",dish.uuid);
        callback(resp);
    } else {
        resJson["message"] = "找不到菜品!";
        resJson["success"] = false;
        SUP_LOG_INFO("[商家更新菜品]失败：找不到菜品");
        auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
        resp->setStatusCode(k500InternalServerError);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        callback(resp);
        return;
    }

}
void MerchantCtrl::merhcnatDishDel(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    SUP_LOG_DEBUG("收到HTTP请求, method={},path={}","Post",req->getPath());
    std::string jsonStr = req->getBody().data();
    Json::Value jsonObj;
    Json::Reader reader;
    if (!reader.parse(jsonStr, jsonObj)) {
        SUP_LOG_ERROR("[商家删除菜品]无效的JSON格式:{}",jsonStr);
    
        jsonObj = Json::objectValue;
    }
    
    Json::Value resJson;
    
    if (jsonObj.empty() || !jsonObj.isMember("dish_id"))
    {
        if (jsonObj.empty())
            resJson["message"] = "Json序列化解析失败";
        else
            resJson["message"] = "缺少字段dish_id";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
        resp->setStatusCode(k400BadRequest);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        callback(resp);
        return;
    }
    
    std::string dish_id = jsonObj["dish_id"].asString();
    db->delDishByID(dish_id);
    redis->delDishById(dish_id);
    
    resJson["success"] = true;
    resJson["message"] = "删除成功!";
    SUP_LOG_INFO("[商家删除菜品]成功");
    auto resp = drogon::HttpResponse::newHttpJsonResponse(resJson);
    resp->setStatusCode(k200OK);
    resp->setContentTypeCode(CT_APPLICATION_JSON);
    callback(resp);

}
void MerchantCtrl::merhcnatOrderList(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    SUP_LOG_DEBUG("收到HTTP请求, method={},path={}","Post",req->getPath());
    std::string jsonStr = req.getBody().data();
    Json::Value jsonObj;
    Json::Reader reader;
    if (!reader.parse(jsonStr, jsonObj)) {
        SUP_LOG_ERROR("[商家获取订单列表]无效的JSON格式:{}",jsonStr);

        jsonObj = Json::objectValue;
    }

    Json::Value resJson;
    try {
        if (jsonObj.empty() || !jsonObj.isMember("merchant_id")) {
            throw HTTPException("缺少商家ID");
        }

        std::string merchant_id = jsonObj["merchant_id"].asString();
        std::vector<data::Order> orderList;
        if(redis->hasOrderListByUserId(merchant_id))
        {
            orderList = redis->getOrderListByMerchant(merchant_id);
        }
        if(orderList.empty())
        {
            orderList = db->getOrderListByMerchant(merchant_id);
            redis->setOrderList(orderList);
            redis->setOrderListByIdDone(merchant_id);
        }
        
        std::string orderListJson = OrderListToJsonArray(orderList);
        resJson["order_list"] = orderListJson;
        
        Json::StreamWriterBuilder writer;
        res.body = Json::writeString(writer, resJson);
        res.set_header("Content-Type", "application/json;charset=UTF-8");
        LOG_INFO("[商家获取订单列表]成功，数量:{}",orderList.size());
    }
    catch (const HTTPException& e) {
        res.status = 500;
        resJson["success"] = false;
        resJson["message"] = e.what();
        res.body = Json::writeString(Json::StreamWriterBuilder(), resJson);
        LOG_ERROR("[商家获取订单列表]出错，信息:{}",e.what());
    }
}
void MerchantCtrl::merhcnatOrderDetailDishList(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    SUP_LOG_DEBUG("收到HTTP请求, method={},path={}","Post",req->getPath());

}
void MerchantCtrl::merhcnatOrderAccept(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    SUP_LOG_DEBUG("收到HTTP请求, method={},path={}","Post",req->getPath());

}