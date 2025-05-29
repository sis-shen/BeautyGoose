/**
 * @file SharedResources.h
 * @author supdriver
 * @date 2025-05-26
 * @brief 管理共享资源
 * @version 2.6.0
 */

#pragma once
#include "../DatabaseClient.hpp"
#include "../RedisClient.hpp"
#include "../CoreData.hpp"
#include <drogon/HttpController.h>
#include "../logger.hpp"

namespace btyGoose
{
    //SharedResources
    /**
     * @class SR
     * @brief SharedResources共享资源类
     * @details 用于维护业务所共享的资源，如MySQL连接资源，Redis连接资源等
     */
    class SR
    {
    private:
        SR(){};
        SR(const SR&) =delete;
        SR operator=(const SR&) = delete;
    public:
        /**
         * @brief 单例模式，获取单例对象
         * @return 单例对象
         */
        static SR& ins()
        {
            static SR sr;
            return sr;
        }

        /**
         * @brief 设置数据库连接对象
         * @param ptr 数据库连接对象的智能指针
         */
        void setDB(std::shared_ptr<DatabaseClient>& ptr)
        {
            // LOG_TRACE("HTTPServer: 开始初始化db");
            _db = ptr;
        }

        /**
         * @brief 设置Redis连接对象
         * @param ptr Redis连接对象的智能指针
         */
        void setRedis(std::shared_ptr<RedisClient>&ptr)
        {
            _redis = ptr;
        }

        /**
         * @brief 检测订单对应的商家订单列表和消费者订单列表是否存在
         * @param order 订单对象
         */
        bool checkRedisOrderListExist(const data::Order &order)
        {
            bool has_consumer = _redis->hasOrderListByUserId(order.consumer_id);
            bool has_merchant = _redis->hasOrderListByUserId(order.merchant_id);

            if (has_consumer && has_merchant)
                return true;
            if (!has_consumer)
            {
                auto order_list = _db->getOrderListByConsumer(order.consumer_id);
                _redis->setOrderList(order_list);
                _redis->setOrderListByIdDone(order.consumer_id);
            }
            if (!has_merchant)
            {
                auto dish_list = _db->getOrderListByMerchant(order.merchant_id);
                _redis->setOrderList(dish_list);
                _redis->setOrderListByIdDone(order.merchant_id);
            }
            return false;
        }

        const std::shared_ptr<DatabaseClient> db(){return _db;}
        const std::shared_ptr<RedisClient> redis(){return _redis;}
    private:
        std::shared_ptr<DatabaseClient> _db;
	    std::shared_ptr<RedisClient> _redis;
    };
}