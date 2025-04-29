#pragma once
#include "../DatabaseClient.hpp"
#include "../RedisClient.hpp"
#include "../CoreData.hpp"
#include <drogon/HttpController.h>
#include "../logger.hpp"

namespace btyGoose
{
    //SharedResources
    class SR
    {
    private:
        SR(){};
        SR(const SR&) =delete;
        SR operator=(const SR&) = delete;
    public:
        static SR& ins()
        {
            static SR sr;
            return sr;
        }

        void setDB(std::shared_ptr<DatabaseClient>& ptr)
        {
            // LOG_TRACE("HTTPServer: 开始初始化db");
            _db = ptr;
        }

        void setRedis(std::shared_ptr<RedisClient>&ptr)
        {
            _redis = ptr;
        }

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