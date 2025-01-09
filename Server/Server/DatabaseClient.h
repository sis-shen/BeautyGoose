#pragma once

#include <QtCore/QCoreApplication>
#include <QTextStream>
#include <QList>
#include <QDebug>
#include <QThread>
#include "CoreData.h"

#include <jdbc/cppconn/prepared_statement.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/exception.h>
#include <jdbc/cppconn/metadata.h>
#include "jdbc/mysql_driver.h"
#include <jdbc/mysql_connection.h>
#include <sstream>

#include <QMutex>
namespace btyGoose
{

class DatabaseClient
{
public:
	DatabaseClient();
private:
	//QSqlDatabase db;
	QString user = "";
	QString password = "";
	QString host = "127.0.0.1";
	QString port = "3306";
	QString database = "";

	sql::Connection* con;
	sql::Statement* stmt;

	QMutex mtx;
public:

	bool loadConfig();
	void saveConfig();

	//Account CURD
	bool addAccount(const data::Account&);
	bool updateAccount(const data::Account&);
	data::Account searchAccountByID(const QString& id);
	data::Account searchAccountByName(const QString& name);
	data::Account searchAccountByPhone(const QString& phone);

	bool delAccountByID(const QString& id);

	//dish CURD
	bool addDish(const data::Dish&);
	bool updateDish(const data::Dish&);
	data::Dish searchDishByID(const QString& id);
	QList<data::Dish> getAllDishList();
	QList<data::Dish> getDishListByMerchant(const QString& id);
	bool delDishByID(const QString& id);

	//Order CURD
	bool addOrder(const data::Order&order);
	bool updateOrder(const data::Order&order);
	bool updateOrder(const QString&id ,const data::Order::Status stat);
	data::Order searchOrderByID(const QString& order_id);
	QList<data::Order> getOrderListByMerchant(const QString& merchant_id);
	QList<data::Order> getOrderListByMerchantWaiting(const QString& merchant_id);
	QList<data::Order> getOrderListByConsumer(const QString& consumer_id);
	QList<data::Order> getAllOrderList();
	bool delOrderByID(const QString& order_id);
	int clearOverTimeOrder();

	//OrderDish CURD
	bool addOrderDishesByID(const QString& order_id, const QList<data::OrderDish>& list);
	bool delOrderDishesByID(const QString& order_id);
	QList<data::OrderDish> getOrderDishesByID(const QString& order_id);
	
	//History CURD
	bool addHistory(const data::Order& order);
	QList<data::Order> getAllHistoryList();

	//HistoryDish CURD
	bool addHistoryDishesByID(const QString& order_id, const QList<data::OrderDish>& list);
	QList<data::OrderDish> getHistoryDishesByID(const QString& order_id);
};
}

