#pragma once

#include <QtCore/QCoreApplication>
#include <QTextStream>
#include <QList>
#include <QDebug>
#include <QThread>
#include "CoreData.h"

#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/exception.h>
#include "jdbc/mysql_driver.h"

#include <mutex>
#include <thread>
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

	std::mutex mtx;
public:

	bool loadConfig();
	void saveConfig();

	//Account CURD
	bool addAccount(const data::Account&);
	bool updataAccount(const data::Account&);
	data::Account searchAccountByID(const QString& id);
	data::Account searchAccountByName(const QString& name);
	data::Account searchAccountByPhone(const QString& phone);

	bool delAccountByID(const QString& id);

	//dish CURD
	bool addDish(const data::Dish&);
	bool updateDish(const data::Dish&);
	data::Dish searchDishByID(const QString& id);
	QList<data::Dish> getDishListByMerchant(const QString& id);
	bool delDishByID(const QString& id);

	//Order CURD
	bool addOrder(const data::Order&);
	bool updateOrder(const data::Order&);
	bool updateOrder(const data::Order::Status);
	data::Order searchOrderByID(const QString& id);
	QList<data::Order> getOrderListByMerchant(const QString& id);
	bool delOrderByID(const QString& id);
	int clearOverTimeOrder();

	//history CURD
	bool addHistory(const data::Order&);
	bool clearOverTimeHistory();
	QList<data::Order> getAllHistory();


};
}

