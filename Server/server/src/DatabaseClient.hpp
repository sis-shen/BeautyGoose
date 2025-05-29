/**
 * @file DatabaseClient.hpp
 * @author supdriver
 * @date 2025-05-26
 * @brief 数据库客户端类定义
 * @version 2.6.0
 */

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

/**
 * @class DatabaseClient
 * @brief 数据库客户端类
 * @details 封装数据库交互的接口,维护与数据库的连接
 */
class DatabaseClient
{
public:
	DatabaseClient();
	virtual ~DatabaseClient();
private:
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
	/**
     * @brief 初始化接口
     * @param _user 数据库用户名
     * @param _password 数据库密码
     * @param _host 数据库主机名
     * @param _port 数据库主机端口
     * @param _database 操作的数据库名称
     * @param _reconnect_interval 重连延迟
     */
	void init(const string&_user,const string&_password,const string&_host,const string&_port,const string&_database,const std::chrono::seconds _reconnect_interval);
	/**
     * @brief 开始运行
     */
	void start();

	//----------Account CURD ------
    /**
     * @brief 向数据库添加账户
     * @param acc 要添加的Account对象
     * @return 是否添加成功
     */
	bool addAccount(const data::Account&acc);

	/**
     * @brief 向数据库更新账户
     * @param acc 要更新的Account对象
     * @return 是否更新成功
     */
	bool updateAccount(const data::Account&acc);

	/**
     * @brief 通过uuid从数据库查询账户
     * @param id 账户的uuid
     * @return 返回查询结果，若找不到，则是空对象
     */
	data::Account searchAccountByID(const string& id);

	/**
     * @brief 通过用户名从数据库查询账户
     * @param name 账户的账户名
     * @return 返回查询结果，若找不到，则是空对象
     */
	data::Account searchAccountByName(const string& name);

	/**
     * @brief 通过电话号码从数据库查询账户
     * @param phone 账户的电话号码
     * @return 返回查询结果，若找不到，则是空对象
     */
	data::Account searchAccountByPhone(const string& phone);

	/**
     * @brief 通过uuid从数据库删除账户
     * @param id 账户的uuid
     * @return 返回删除结果
     */
	bool delAccountByID(const string& id);

	//----------dish CURD -----------
	/**
     * @brief 向数据库添加菜品
     * @param dish 要添加的Dish对象
     * @return 是否添加成功
     */
	bool addDish(const data::Dish&dish);

	/**
     * @brief 向数据库更新菜品
     * @param dish 要更新的Dish对象
     * @return 是否更新成功
     */
	bool updateDish(const data::Dish&dish);

	/**
     * @brief 通过uuid从数据库查询菜品
     * @param id 菜品的uuid
     * @return 返回查询结果，若找不到，则是空对象
     */
	data::Dish searchDishByID(const string& id);

	/**
     * @brief 从数据库获取所有的菜品
     * @return 返回查询结果,存储vector容器中
     */
	vector<data::Dish> getAllDishList();
	
	/**
     * @brief 通过uuid从数据库查询指定商家的所有菜品
     * @param id 商家账户的的uuid
     * @return 返回查询结果，若找不到，则是空对象
     */
	vector<data::Dish> getDishListByMerchant(const string& id);

	/**
     * @brief 通过uuid从数据库删除菜品
     * @param id 账户的uuid
     * @return 返回删除结果
     */
	bool delDishByID(const string& id);

	//------------- Order CURD ----------
    /**
     * @brief 向数据库添加订单
     * @param order 要添加的Order对象
     * @return 是否添加成功
     */
	bool addOrder(const data::Order&order);

	/**
     * @brief 向数据库更新订单
     * @param order 要更新的Order对象
     * @return 是否更新成功
     */
	bool updateOrder(const data::Order&order);

	/**
     * @brief 向数据库更新账户，金更新状态
     * @param id 要更新订单的uuid
     * @param stat 要更新订单的新状态
     * @return 是否更新成功
     */
	bool updateOrder(const string&id ,const data::Order::Status stat);

	/**
     * @brief 通过uuid从数据库查询订单
     * @param id 订单的uuid
     * @return 返回查询结果，若找不到，则是空对象
     */
	data::Order searchOrderByID(const string& order_id);

	/**
     * @brief 通过uuid从数据库查询指定商家的所有订单
     * @param id 商家账户的的uuid
     * @return 返回查询结果，若找不到，则是空对象
     */
	vector<data::Order> getOrderListByMerchant(const string& merchant_id);

	/**
     * @brief 通过uuid从数据库查询指定商家的所有待处理状态的订单
     * @param merchant_id 商家账户的的uuid
     * @return 返回查询结果，若找不到，则是空对象
     */
	vector<data::Order> getOrderListByMerchantWaiting(const string& merchant_id);

	/**
     * @brief 通过uuid从数据库查询指定消费者的所有订单
     * @param consumer_id 消费者账户的的uuid
     * @return 返回查询结果，若找不到，则是空对象
     */
	vector<data::Order> getOrderListByConsumer(const string& consumer_id);

	/**
     * @brief 从数据库获取所有的订单
     * @return 返回查询结果,存储vector容器中
     */
	vector<data::Order> getAllOrderList();

	/**
     * @brief 通过uuid从数据库删除菜品
     * @param order_id 账户的uuid
     * @return 返回删除结果
     */
	bool delOrderByID(const string& order_id);

	/**
     * @brief 清除过期订单
     * @return 返回删除结果
     */
	int clearOverTimeOrder();

	//--------------- OrderDish CURD -------------
     /**
     * @brief 向数据库批量添加订单菜品
     * @param order_id 指定订单的uuid
     * @param list 订单菜品列表
     * @return 是否添加成功
     */
	bool addOrderDishesByID(const string& order_id, const vector<data::OrderDish>& list);

     /**
     * @brief 通过订单uuid从数据库删除订单菜品
     * @param order_id 订单的uuid
     * @return 返回删除结果
     */
	bool delOrderDishesByID(const string& order_id);

     /**
     * @brief 通过订单uuid从数据库获取订单菜品列表
     * @param order_id 订单的uuid
     * @return 返回查询结果
     */
	vector<data::OrderDish> getOrderDishesByID(const string& order_id);
	
	//-----------History CURD----------

     /**
     * @brief 向数据库添加历史订单
     * @param order 要添加的Order对象
     * @return 是否添加成功
     */
	bool addHistory(const data::Order& order);

     /**
     * @brief 从数据库获取所有的历史订单
     * @return 是否添加成功
     */
	vector<data::Order> getAllHistoryList();

	//-----------HistoryDish CURD--------
     /**
     * @brief 通过订单uuid向数据库添加历史订单菜品列表
     * @param order_id 订单的uuid
     * @return 返回查询结果
     */
	bool addHistoryDishesByID(const string& order_id, const vector<data::OrderDish>& list);

     /**
     * @brief 通过历史订单uuid从数据库获取订单菜品列表
     * @param order_id 订单的uuid
     * @return 返回查询结果
     */
	vector<data::OrderDish> getHistoryDishesByID(const string& order_id);
};
}

