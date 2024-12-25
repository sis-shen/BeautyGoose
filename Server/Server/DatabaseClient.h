#pragma once

#include <qsqldatabase.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QFile>
#include <QTextStream>
#include <QList>
#include "CoreData.h"

namespace btyGoose
{

class DatabaseClient
{
public:
	DatabaseClient();


	bool loadConfig();
	void saveConfig();

	//Account CURD
	bool addAccount(const data::Account&);
	bool updataAccount(const data::Account&);
	data::Account searchAccountByID(const QString& id);
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

private:
	QSqlDatabase db;
	QString user;
	QString password;
	QString host = "127.0.0.1";
	QString port = "3306";
	QString database;
};
}

