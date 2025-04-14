#pragma once
#include "httplib.h"
#include "DatabaseClient.hpp"
#include "RedisClient.hpp"
using std::string;
using std::vector;
namespace btyGoose
{

class HTTPServer
{
	class HTTPException
	{
	private:
		string message;
	public:
		HTTPException(const string& msg = "a problem")
			:message(msg)
		{}

		string what()const {
			return message;
		}
	};
private: 
	HTTPServer();//单例模式化
	HTTPServer(const HTTPServer&) = delete;
	HTTPServer operator=(const HTTPServer&) =delete;

	static HTTPServer* _ins;
public:
	static HTTPServer* getInstance()
	{
		if(_ins == nullptr)
		{
			_ins = new HTTPServer();
		}

		return _ins;
	}
	// void initDB(const string&_user,const string&_password,const string&_host,const string&_port,const string&_database,const std::chrono::seconds _reconnect_interval);
	void setDB(std::shared_ptr<DatabaseClient>& ptr);
	void setRedis(std::shared_ptr<RedisClient>&ptr);
	void init(const std::string&ip,const uint32_t port);

	void start();

	void initTestAPI();
	void initAccountAPI();
	bool AuthenticateAuthCode(const string& phone,const string&auth_code);	//这里留着接入第三方API
	void initConsumerAPI();
	void initMerchantAPI();
	void initAdminAPI();

	bool getPay(double pay);
	//工具函数
	string DishListToJsonArray(const vector<data::Dish>& dishList);
	vector<data::Dish> DishListFromJsonArray(const string& jsonString);

	string OrderListToJsonArray(const vector<data::Order>&orderList);
	vector<data::Order> OrderListFromJsonArray(const string& jsonString);

	string OrderDishListToJsonArray(const vector<data::OrderDish>&dishList);

	bool checkRedisOrderListExist(const data::Order&order);


private:
	httplib::Server svr;
	std::shared_ptr<DatabaseClient> db;
	std::shared_ptr<RedisClient> redis;
	string host;
	int port;
};
}


