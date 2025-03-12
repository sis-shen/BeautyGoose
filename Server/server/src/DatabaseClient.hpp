#pragma once

#include "CoreData.hpp"
#include "logger.hpp"

#include <cppconn/prepared_statement.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>
#include <cppconn/metadata.h>
#include "mysql_driver.h"
#include <mysql_connection.h>
#include <sstream>
#include <string>
#include <vector>
#include <mutex>
#include <atomic>
#include <condition_variable>

using std::vector;
using std::string;

namespace btyGoose
{

class DatabaseClient
{
public:
	DatabaseClient();
	virtual ~DatabaseClient();
private:
	//QSqlDatabase db;
	string user = "";
	string password = "";
	string host = "127.0.0.1";
	string port = "3306";
	string database = "";

	sql::Connection* con;
	sql::Statement* stmt;

	std::mutex mtx;

private:
	//守护线程相关成员变量
	std::atomic<bool> connected_= false;	//表示连接状态
    std::chrono::seconds reconnect_interval_;//两次重连的最小间隔时间
    std::condition_variable reconnect_cv_;	//重连条件变量
    std::mutex reconnect_mtx_;		//重连锁
    std::thread reconnect_thread_;	//守护线程
    bool stop_reconnect_ = false;	//停止重连的标记

	void reconnectLoop();
	bool checkConnection();
public:
	void init(const string&_user,const string&_password,const string&_host,const string&_port,const string&_database,const std::chrono::seconds _reconnect_interval);

	void start();

	//Account CURD
	bool addAccount(const data::Account&);
	bool updateAccount(const data::Account&);
	data::Account searchAccountByID(const string& id);
	data::Account searchAccountByName(const string& name);
	data::Account searchAccountByPhone(const string& phone);

	bool delAccountByID(const string& id);

	//dish CURD
	bool addDish(const data::Dish&);
	bool updateDish(const data::Dish&);
	data::Dish searchDishByID(const string& id);
	vector<data::Dish> getAllDishList();
	vector<data::Dish> getDishListByMerchant(const string& id);
	bool delDishByID(const string& id);

	//Order CURD
	bool addOrder(const data::Order&order);
	bool updateOrder(const data::Order&order);
	bool updateOrder(const string&id ,const data::Order::Status stat);
	data::Order searchOrderByID(const string& order_id);
	vector<data::Order> getOrderListByMerchant(const string& merchant_id);
	vector<data::Order> getOrderListByMerchantWaiting(const string& merchant_id);
	vector<data::Order> getOrderListByConsumer(const string& consumer_id);
	vector<data::Order> getAllOrderList();
	bool delOrderByID(const string& order_id);
	int clearOverTimeOrder();

	//OrderDish CURD
	bool addOrderDishesByID(const string& order_id, const vector<data::OrderDish>& list);
	bool delOrderDishesByID(const string& order_id);
	vector<data::OrderDish> getOrderDishesByID(const string& order_id);
	
	//History CURD
	bool addHistory(const data::Order& order);
	vector<data::Order> getAllHistoryList();

	//HistoryDish CURD
	bool addHistoryDishesByID(const string& order_id, const vector<data::OrderDish>& list);
	vector<data::OrderDish> getHistoryDishesByID(const string& order_id);
};
}

