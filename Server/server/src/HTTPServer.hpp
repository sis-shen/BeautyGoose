#pragma once
#include "httplib.h"
#include "DatabaseClient.hpp"
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

	void start();

	bool loadConfig();
	void saveConfig();

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


private:
	httplib::Server svr;
	DatabaseClient db;
	string host = "127.0.0.1";
	int port = 80;
};
}


