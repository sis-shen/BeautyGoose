/**
 * @file logger.hpp
 * @author supdriver
 * @date 2025-05-26
 * @brief RedisClient接口声明
 * @version 2.6.0
 */


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

    /**
     * @brief 构造函数
     * @param ip Redis服务器的ip
     * @param port 服务器端口
     * @param db Redis数据库号
     * @param keep_alive 是否保持长连接
     * @param password 登录密码
     */
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

    /**
     * @brief 检查是否依然保持连接
     * @return 是否连接成功
     */
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


    /**
     * @brief 清空Redis
     */
    void flushall()
    {
        _redis->flushall();
    }

    //---------订单相关---------
    /**
     * @brief 向Redis缓存Order对象
     * @param order 要缓存的Order对象
     */
    void setOrder(const data::Order& order);

    /**
     * @brief 向Redis缓存OrderDish列表
     * @param order_id 要缓存的Order的uuid
     * @param dish_list_json 订单菜品列表序列化字符串
     */
    void setOrderDishListJson(const string&order_id,const string& dish_list_json);

    /**
     * @brief 向Redis获取OrderDish列表序列化字符串
     * @param order_id 对应订单的id
     * @return 序列化字符串
     */
    string getOrderDishListJson(const string&order_id);

    /**
     * @brief 向Redis获取Order对象
     * @param order_id Order的uuid
     * @return Order对象
     */
    data::Order getOrderById(const string& id);

    /**
     * @brief 向Redis缓存Order列表
     * @param order_list 要缓存的Order列表
     */
    void setOrderList(const vector<data::Order>& order_list);

    /**
     * @brief 设置OrderList的过期时间
     * @param id 要设置过期时间的订单的uuid
     */
    void setOrderListByIdDone(const string&id);

    /**
     * @brief 通过商家uuid获取订单列表
     * @param id 商家uuid
     */
    vector<data::Order> getOrderListByMerchant(const string&id);

    /**
     * @brief 通过商家uuid获取等待状态的订单列表
     * @param id 商家uuid
     */
    vector<data::Order> getOrderListByMerchantWaiting(const string&id);

    /**
     * @brief 通过消费者uuid获取订单列表
     * @param id 消费者uuid
     */
    vector<data::Order> getOrderListByConsumer(const string&id);

    /**
     * @brief 检查该用户id下是否有订单列表缓存
     * @param id 用户uuid
     */
    bool hasOrderListByUserId(const string& id);

    /**
     * @brief 通过订单id删除订单缓存
     * @param id 订单uuid
     */
    void delOrderById(const string&id);

    

    //----------菜品相关---------

    /**
     * @brief 向Redis缓存Dish对象
     * @param dish 要缓存的Dish对象
     */
    void setDish(const data::Dish& dish);

    /**
     * @brief 从Redis获取Dish对象
     * @param id 要获取订单的uuid
     */
    data::Dish getDishById(const string&id);

    /**
     * @brief 通过菜品id删除菜品缓存
     * @param id 菜品uuid
     */
    void delDishById(const string&id);

    /**
     * @brief 通过商家uuid获取菜品列表
     * @param id 商家uuid
     * @return 菜品列表序列化字符串
     */
    string getDishListJsonByMerchant(const string&id);

    /**
     * @brief 设置商家菜品列表缓存
     * @param id 商家uuid
     * @param json 菜品列表序列化字符串
     */
    void setDishListJsonByMerchant(const string&id,const string&json);

    /**
     * @brief 删除指定商家的菜品列表缓存
     * @param id 商家uuid
     */
    void delDishListJsonByMerchant(const string&id);    //缓存会有失效的时候
    
    //-------------账号相关-------------
    /**
     * @brief 通过账户名获取账户对象缓存
     * @param name 账户名
     * @return 账户对象
     */
    data::Account getAccountByName(const string&name);

    /**
     * @brief 通过账户uuid获取账户对象缓存
     * @param id 账户uuid
     * @return 账户对象
     */
    data::Account getAccountById(const string&id);

    /**
     * @brief 通过手机号获取账户对象缓存
     * @param phone 手机号
     * @return 账户对象
     */
    data::Account getAccountByPhone(const string&phone);

    /**
     * @brief 向Redis设置账户对象缓存
     * @param acc 账户对象
     */
    void setAccount(const data::Account& acc);

private:
    shared_ptr<sw::redis::Redis> _redis;
};
}