#pragma once
#include <sw/redis++/redis.h>
#include <memory>
#include "CoreData.hpp"

namespace BeautyGoose{
using namespace std;
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

        _cli = make_shared<sw::redis::Redis>(sw::redis::Redis(opts));
    }

public:

private:
    shared_ptr<sw::redis::Redis> _cli;
};
}