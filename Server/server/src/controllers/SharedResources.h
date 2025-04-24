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

        const std::shared_ptr<DatabaseClient> db(){return _db;}
        const std::shared_ptr<RedisClient> redis(){return _redis;}
    private:
        std::shared_ptr<DatabaseClient> _db;
	    std::shared_ptr<RedisClient> _redis;
    };
}