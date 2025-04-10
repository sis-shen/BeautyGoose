#include "HTTPServer.hpp"
#include "logger.hpp"
#include <gflags/gflags.h>
#include <sw/redis++/redis.h>

//服务器相关参数
DEFINE_string(host,"0.0.0.0","服务器监听的源ip地址范围");
DEFINE_uint32(port,80,"服务器的监听端口");

//数据库客户端相关参宿
DEFINE_string(db_user,"root","数据库的连接用户");
DEFINE_string(db_password,"root_password","数据库用户的密码");
DEFINE_string(db_host,"localhost","数据库服务所在的ip");
DEFINE_string(db_port,"3306","数据库服务开放的端口");
DEFINE_string(db_database,"db","数据库中存储数据的数据库");
DEFINE_int32(db_reconnect_interval,10,"数据库两次重连的最小间隔秒数");

//Redis客户端相关参数
DEFINE_string(redis_host,"127.0.0.1","Redis服务器的ip地址");
DEFINE_uint32(redis_port,6379,"Redis服务器的端口");
DEFINE_uint32(redis_db,0,"Redis数据库的变化，默认0，范围[0,15]");
DEFINE_bool(redis_keep_alive,true,"是否保持长连接");

//日志器相关参数
DEFINE_string(log_logfile,"./log/logfile","日志落地文件及路径");
DEFINE_bool(log_release_mode,false,"设置是否为发布模式输出日志,\ntrue:过滤低等级日志，输出到文件\nfalse:输出所有日志，输出到标准输出");
DEFINE_int32(log_release_level,spdlog::level::warn,"设置release模式下过滤的日志等级");

int main(int argc, char *argv[])
{
    google::ParseCommandLineFlags(&argc,&argv,true);//解析命令行参数
    btyGoose::init_logger(FLAGS_log_release_mode,FLAGS_log_logfile,FLAGS_log_release_level);//初始化日志器
    LOG_DEBUG("全局日志器初始化成功");

    auto db = std::make_shared<btyGoose::DatabaseClient>();
    db->init(FLAGS_db_user,FLAGS_db_password,FLAGS_db_host,FLAGS_db_port,FLAGS_db_database,                         
        std::chrono::seconds(FLAGS_db_reconnect_interval));
    btyGoose::HTTPServer::getInstance()->setDB(db);
    LOG_DEBUG("数据库对象设置成功");

    auto redis = std::make_shared<btyGoose::RedisClient>(
        btyGoose::RedisClient(FLAGS_redis_host,FLAGS_redis_port,FLAGS_redis_db,FLAGS_redis_keep_alive));

    btyGoose::HTTPServer::getInstance()->setRedis(redis);

    btyGoose::HTTPServer::getInstance()->init(FLAGS_host,FLAGS_port);
    btyGoose::HTTPServer::getInstance()->start();
    return 0;
}