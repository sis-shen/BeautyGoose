#pragma once
#include <sw/redis++/redis.h>
#include <unordered_set>
#include <vector>
#include <unordered_map>
#include <memory>
#include "CoreData.hpp"
#include "logger.hpp"

namespace btyGoose{

using std::string;
using std::shared_ptr;
using std::make_shared;
using std::vector;




class RedisClient
{
    //设定前缀
    const std::string account_prefix = "data:Account:";
    const std::string dish_prefix = "data:Dish:";
    const std::string order_prefix = "data:Order:";
    const std::string history_prefix = "data:History:";
    const std::chrono::seconds expire_time = std::chrono::seconds(300);
public:
    RedisClient(const string&ip,const uint16_t port,const uint16_t db,const bool keep_alive,const string&password)
    {
        SUP_LOG_INFO("即将连接Redis服务器,地址 {}:{}",ip,port);
        sw::redis::ConnectionOptions opts;
        opts.host = ip;
        opts.port = port;
        opts.db = db;
        opts.keep_alive = keep_alive;
        opts.password = password;

        _redis = make_shared<sw::redis::Redis>(sw::redis::Redis(opts));
    }

    

public:
    bool isOK()
    {
        try {
            auto pong = _redis->ping();
            if (pong == "PONG") {
                return true;
            }
        } catch (const sw::redis::Error &e) {
            SUP_LOG_WARN("Redis连接失败：{}",e.what());
            return false;
        }
        return false;
    }    

    void flushall()
    {
        _redis->flushall();
    }

    //订单相关
    void setOrder(const data::Order& order);
    void setOrderDishListJson(const string&order_id,const string& dish_list_json);
    string getOrderDishListJson(const string&order_id);
    data::Order getOrderById(const string& id);
    void setOrderList(const vector<data::Order> order_list);
    void setOrderListByIdDone(const string&id);
    vector<data::Order> getOrderListByMerchant(const string&id);
    vector<data::Order> getOrderListByMerchantWaiting(const string&id);
    vector<data::Order> getOrderListByConsumer(const string&id);
    bool hasOrderListByUserId(const string& id);
    void delOrderById(const string&id);

    

    //菜品相关
    data::Dish getDishById(const string&id);
    void delDishById(const string&id);
    string getDishListJsonByMerchant(const string&id);
    void setDishListJsonByMerchant(const string&id,const string&json);
    void delDishListJsonByMerchant(const string&id);    //缓存会有失效的时候
    void setDish(const data::Dish& dish);
    
    //账号相关
    data::Account getAccountByName(const string&name);
    data::Account getAccountById(const string&id);
    data::Account getAccountByPhone(const string&phone);
    void setAccount(const data::Account& acc);

private:
    shared_ptr<sw::redis::Redis> _redis;
};
}