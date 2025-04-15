#include "RedisClient.hpp"
namespace btyGoose
{
using namespace std;

void RedisClient::setAccount(const data::Account& acc)
{
    unordered_map<string,string> m;

    m["uuid"] = acc.uuid;
    m["name"] = acc.name;
    m["password"] = acc.password;
    m["nickname"] = acc.nickname;
    m["icon"] = acc.icon;
    m["type"] = to_string(static_cast<int>(acc.type));
    m["balance"] = to_string(acc.balance);
    m["phone"] = acc.phone;
    m["level"] = to_string(static_cast<int>(acc.level));

    _redis->hmset(account_prefix+"hash:"+acc.name,m.begin(),m.end());
    _redis->set(account_prefix+"id:"+acc.uuid,account_prefix+acc.name);
    _redis->set(account_prefix+"phone:"+acc.phone,account_prefix+acc.name);
}

data::Account RedisClient::getAccountByName(const string&name)
{
    unordered_map<string,string> m;
    _redis->hgetall(account_prefix+"hash:"+name,std::inserter(m,m.begin()));

    data::Account acc;
    if(m.empty())return acc;
    acc.uuid = m["uuid"];
    acc.name = m["name"] ;
    acc.password = m["password"];
    acc.nickname = m["nickname"];
    acc.icon = m["icon"];
    acc.type = static_cast<data::Account::Type>(stoi(m["type"]));
    acc.balance =  stod(m["balance"]);
    acc.phone = m["phone"];
    acc.level = static_cast<data::Account::Level>(stoi(m["level"]));

    return acc;
}

data::Account RedisClient::getAccountById(const string&id)
{
    auto res = _redis->get(account_prefix+"id:"+id);
    if(!res.has_value())
    {
        return {};
    }

    return getAccountByName(res.value());
}

data::Account RedisClient::getAccountByPhone(const string&phone)
{
    auto res = _redis->get(account_prefix+"phone:"+phone);
    if(!res.has_value())
    {
        return {};
    }

    return getAccountByName(res.value());
}

void RedisClient::setDish(const data::Dish& dish)
{
    //被删除的菜品不缓存
    if(dish.is_delete == true) return;
    unordered_map<string,string> m;

    m["uuid"] = dish.uuid;
    m["merchant_id"] = dish.merchant_id;
    m["merchant_name"] = dish.merchant_name;
    m["name"] = dish.name;
    m["icon_path"] = dish.icon_path;
    m["description"] = dish.description;
    m["base_price"] = to_string(dish.base_price);
    m["price_factor"] = to_string(dish.price_factor);

    _redis->hset(dish_prefix+"id:"+dish.uuid,m.begin(),m.end());
}

void RedisClient::delDishById(const string&id)
{
    _redis->del(dish_prefix+"id:"+id);
}

data::Dish RedisClient::getDishById(const string&id)
{
    unordered_map<string,string> m;
    _redis->hgetall(dish_prefix+"id:"+id,std::inserter(m,m.begin()));

    if(m.empty()) return {};

    data::Dish dish;

    dish.uuid = m["uuid"];
    dish.merchant_id = m["merchant_id"];
    dish.merchant_name = m["merchant_name"];
    dish.name = m["name"];
    dish.icon_path = m["icon_path"];
    dish.description = m["description"];
    dish.base_price = stod(m["base_price"]);
    dish.price_factor = stod(m["price_factor"]);
    dish.is_delete = false;

    return dish;
}

void RedisClient::setDishListJsonByMerchant(const string&id,const string&jsonstr)
{
    _redis->set(dish_prefix+"merchant:"+id,jsonstr);
    _redis->expire(dish_prefix+"merchant:"+id,expire_time);
}

void RedisClient::delDishListJsonByMerchant(const string&id)
{
    _redis->del(dish_prefix+"merchant:"+id);
}

string RedisClient::getDishListJsonByMerchant(const string&id)
{
    auto res = _redis->get(dish_prefix+"merchant:"+id);
    if(res.has_value())return res.value();
    return "";
}

//============= 订单相关 =================//
void RedisClient::setOrder(const data::Order&order)
{
    unordered_map<string,string> m;

    m["merchant_id"] = order.merchant_id;
    m["merchant_name"] = order.merchant_name;
    m["consumer_id"] = order.consumer_id;
    m["consumer_name"] = order.consumer_name;
    m["time"] = order.time;
    m["level"] = to_string(static_cast<int>(order.level));
    m["pay"] = to_string(order.pay);
    m["uuid"] = order.uuid;
    m["status"] = to_string(static_cast<int>(order.status));
    m["sum"] = to_string(order.sum);

    _redis->hset(order_prefix+"id:"+order.uuid,m.begin(),m.end());
    _redis->sadd(order_prefix+"merchant:"+order.merchant_id,order.uuid);
    _redis->sadd(order_prefix+"consumer:"+order.consumer_id,order.uuid);
}

bool RedisClient::hasOrderListByUserId(const string&id)
{
    return _redis->exists(order_prefix+"exist:"+id);
}

void RedisClient::setOrderListByIdDone(const string&id)
{
    string key =order_prefix+"exist:"+id; 
    _redis->set(key,"1");
    //我们认为5分钟内数据有完整性
    ////////////////////////////////////
    //TODO 风险分析
    ////////////////////////////////////
    _redis->expire(key,expire_time);
}

void RedisClient::setOrderDishListJson(const string&order_id,const string& dish_list_json)
{
    _redis->set(order_prefix+"dishlist:"+order_id,dish_list_json);
}

string RedisClient::getOrderDishListJson(const string&order_id)
{
    auto res = _redis->get(order_prefix+"dishlist:"+order_id);
    if(res.has_value())return res.value();
    else return "";
}

data::Order RedisClient::getOrderById(const string& id)
{
    unordered_map<string,string> m;

    _redis->hgetall(order_prefix+"id:"+id,std::inserter(m,m.begin()));
    if(m.empty())return {};

    data::Order order;

    order.merchant_id = m["merchant_id"];
    order.merchant_name = m["merchant_name"];
    order.consumer_id = m["consumer_id"];
    order.consumer_name = m["consumer_name"];
    order.time = m["time"];
    order.level = static_cast<data::Account::Level>(stoi(m["level"]));
    order.pay = stod(m["pay"]);
    order.uuid = m["uuid"];
    order.status = static_cast<data::Order::Status>(stoi(m["status"]));
    order.sum = stoi(m["sum"]);
    return order;
}

void RedisClient::delOrderById(const string&id)
{
    _redis->del(order_prefix+"id:"+id);
}

void RedisClient::setOrderList(const vector<data::Order> order_list)
{
    for(const auto& order:order_list)
    {
        setOrder(order);
    }
}
vector<data::Order> RedisClient::getOrderListByMerchant(const string&id)
{
    unordered_set<string> s;
    _redis->smembers(order_prefix+"merchant:"+id,std::inserter(s,s.begin()));
    if(s.empty())return {};
    vector<data::Order> order_list;
    for(const auto&id:s)
    {
        auto order = getOrderById(id);
        if(order.uuid.empty())continue;
        order_list.push_back(order);
    }
    return order_list;
}
vector<data::Order> RedisClient::getOrderListByMerchantWaiting(const string&id)
{
    auto list = getOrderListByMerchant(id);
    if(list.empty())return {};
    vector<data::Order> ret;
    for(auto order:list)
    {
        if(order.status == data::Order::WAITING)
            ret.push_back(order);
    }
    return ret;

}
vector<data::Order> RedisClient::getOrderListByConsumer(const string&id)
{
    unordered_set<string> s;
    _redis->smembers(order_prefix+"consumer:"+id,std::inserter(s,s.begin()));
    if(s.empty())return {};
    vector<data::Order> order_list;
    for(const auto&id:s)
    {
        auto order = getOrderById(id);
        if(order.uuid.empty())continue;
        order_list.push_back(order);
    }
    return order_list;
}





}