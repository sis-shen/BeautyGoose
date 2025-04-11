#pragma once
#include <sw/redis++/redis.h>
#include <unordered_set>
#include <vector>
#include <unordered_map>
#include <memory>
#include "CoreData.hpp"

namespace btyGoose{

using std::string;
using std::shared_ptr;
using std::make_shared;
using std::vector;


//设定前缀
const std::string account_prefix = "data:Account:";
const std::string dish_prefix = "data:Dish:";
const std::string order_prefix = "data:Order:";
const std::string history_prefix = "data:History:";

class RedisClient
{
public:
    RedisClient(const string&ip,const uint16_t port,const uint16_t db,const bool keep_alive)
    {
        sw::redis::ConnectionOptions opts;
        opts.host = ip;
        opts.port = port;
        opts.db = db;
        opts.keep_alive = keep_alive;

        _redis = make_shared<sw::redis::Redis>(sw::redis::Redis(opts));
    }

    

public:

    //订单相关
    void setOrder(const data::Order& order);
    data::Order getOrderById(const string& id);
    void setOrderList(const vector<data::Order> order_list);
    vector<data::Order> getOrderListByMerchant(const string&id);
    vector<data::Order> getOrderListByMerchantWaiting(const string&id);
    vector<data::Order> getOrderListByConsumer(const string&id);
    

    //菜品相关
    data::Dish getDishById(const string&id);
    string getDishListJsonByMerchant(const string&id);
    void setDishListJsonByMerchant(const string&id,const string&json);
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