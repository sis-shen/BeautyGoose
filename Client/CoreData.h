#pragma once
#include <qstring.h>
#include <qbytearray.h>

#include <string>
#include <QString>
#include <QUuid>
#include <QDateTime>
#include <QFile>
#include <QDebug>
#include <QFileInfo>
#include <iostream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QFile>
namespace btyGoose
{


	/// <summary>
	/// 定义核心数据结构
	/// </summary>
	namespace data
	{
		struct Account	
		{
			///账户类型
			enum Type
			{
				CONSUMER,	//消费者账户
				MERCHANT,	//商家账户
				ADMIN		//管理员账户
			};
			enum Level
			{
				MEMBER,		//普通用户
				VIP,		//VIP用户
				SUVIP		//超级VIP用户
			};

			QString uuid = "";		//唯一id
			QString name = "";		//账户名
			QString password = "";	//密码
			QString nickname = "";	//昵称
			QByteArray icon = QByteArray();	//图标
			Type type = CONSUMER;			//账户类型
			double balance = 0;		//账户余额
			QString phone = "";		//电话号码
			Level level = MEMBER;		//优惠等级

			QString toJson()
			{
				QJsonObject jsonObj;

				// 填充基本字段
				jsonObj["uuid"] = uuid;
				jsonObj["name"] = name;
				jsonObj["password"] = password;
				jsonObj["nickname"] = nickname;
				jsonObj["icon"] = QString(icon.toBase64()); // 图标可能是二进制数据，使用Base64编码
				jsonObj["balance"] = balance;
				jsonObj["phone"] = phone;

				// 填充类型字段，转换枚举类型为字符串
				jsonObj["type"] = (type == CONSUMER) ? "CONSUMER" : (type == MERCHANT) ? "MERCHANT" : "ADMIN";
				jsonObj["level"] = (level == MEMBER) ? "MEMBER" : (level == VIP) ? "VIP" : "SUVIP";

				// 转换为 JSON 字符串并返回
				QJsonDocument doc(jsonObj);
				return QString(doc.toJson(QJsonDocument::Compact));
			}

			bool loadFromJson(const std::string& jsonStr)
			{
				QString qJsonString = QString::fromStdString(jsonStr);

				// 解析 JSON 字符串
				QJsonDocument doc = QJsonDocument::fromJson(qJsonString.toUtf8());
				if (doc.isObject()) {
					QJsonObject jsonObj = doc.object();

					// 从 JSON 中加载数据到结构体成员
					if (jsonObj.contains("uuid") && jsonObj["uuid"].isString()) {
						uuid = jsonObj["uuid"].toString();
					}

					if (jsonObj.contains("name") && jsonObj["name"].isString()) {
						name = jsonObj["name"].toString();
					}

					if (jsonObj.contains("password") && jsonObj["password"].isString()) {
						name = jsonObj["password"].toString();
					}

					if (jsonObj.contains("nickname") && jsonObj["nickname"].isString()) {
						nickname = jsonObj["nickname"].toString();
					}

					if (jsonObj.contains("icon") && jsonObj["icon"].isString()) {
						icon = QByteArray::fromBase64(jsonObj["icon"].toString().toUtf8());
					}

					if (jsonObj.contains("type") && jsonObj["type"].isString()) {
						QString typeStr = jsonObj["type"].toString();
						if (typeStr == "CONSUMER") {
							type = CONSUMER;
						}
						else if (typeStr == "MERCHANT") {
							type = MERCHANT;
						}
						else if (typeStr == "ADMIN") {
							type = ADMIN;
						}
					}

					if (jsonObj.contains("balance") && jsonObj["balance"].isDouble()) {
						balance = jsonObj["balance"].toDouble();
					}

					if (jsonObj.contains("phone") && jsonObj["phone"].isString()) {
						phone = jsonObj["phone"].toString();
					}

					if (jsonObj.contains("level") && jsonObj["level"].isString()) {
						QString levelStr = jsonObj["level"].toString();
						if (levelStr == "MEMBER") {
							level = MEMBER;
						}
						else if (levelStr == "VIP") {
							level = VIP;
						}
						else if (levelStr == "SUVIP") {
							level = SUVIP;
						}
					}


					return true;
				}
				else
				{
					qDebug() << "Invalid Json string : " << jsonStr;
					return false;
				}
			}
		};

		class Dish
		{
			QString uuid = "";			//菜品id
			QString merchant_id = "";	//商家uuid
			QString name = "";			//菜品名称
			QString icon_path = "";		//网络图片的url
			QString description = "";	//菜品的描述
			double base_price = 0;		//基础价格
			double price_factor = 1;	//价格影响因素

			QString toJson()
			{
				QJsonObject jsonObj;

				// 填充基本字段
				jsonObj["uuid"] = uuid;
				jsonObj["merchant_id"] = merchant_id;
				jsonObj["name"] = name;
				jsonObj["icon_path"] = icon_path;
				jsonObj["description"] = description;
				jsonObj["base_price"] = base_price;
				jsonObj["price_factor"] = price_factor;

				// 将 QJsonObject 转换为 JSON 字符串
				QJsonDocument doc(jsonObj);
				return QString(doc.toJson(QJsonDocument::Compact));
			}

			bool loadFromJson(const std::string& jsonStr)
			{
				QString qJsonString = QString::fromStdString(jsonStr);

				// 解析 JSON 字符串
				QJsonDocument doc = QJsonDocument::fromJson(qJsonString.toUtf8());
				if (doc.isObject()) {
					QJsonObject jsonObj = doc.object();

					// 从 JSON 中加载数据到成员变量
					if (jsonObj.contains("uuid") && jsonObj["uuid"].isString()) {
						uuid = jsonObj["uuid"].toString();
					}

					if (jsonObj.contains("merchant_id") && jsonObj["merchant_id"].isString()) {
						merchant_id = jsonObj["merchant_id"].toString();
					}

					if (jsonObj.contains("name") && jsonObj["name"].isString()) {
						name = jsonObj["name"].toString();
					}

					if (jsonObj.contains("icon_path") && jsonObj["icon_path"].isString()) {
						icon_path = jsonObj["icon_path"].toString();
					}

					if (jsonObj.contains("description") && jsonObj["description"].isString()) {
						description = jsonObj["description"].toString();
					}

					if (jsonObj.contains("base_price") && jsonObj["base_price"].isDouble()) {
						base_price = jsonObj["base_price"].toDouble();
					}

					if (jsonObj.contains("price_factor") && jsonObj["price_factor"].isDouble()) {
						price_factor = jsonObj["price_factor"].toDouble();
					}

					return true;

				}
				else
				{
					qDebug() << "Invalid Json string : " << jsonStr;
					return false;
				}
			}

		};

		struct Order
		{
			enum Status
			{
				UNPAYED,		//未支付状态
				WAITING,		//等待商家处理
				OVER_TIME,		//订单超时
				REJECTED,		//商家拒收
				SUCCESS,		//成功完成
				ERR,			//订单出错
				FATAL			//订单出现致命错误
			};

			QString merchant_id;		//商家id
			QString merchant_name;		//商家账户名称
			QString consumer_id;		//消费者id
			QString consumer_name;		//消费者账户名称
			QString time;				//订单时间
			Account::Level level;		//优惠等级
			double pay;					//订单价格
			QString uuid;				//订单id
			Status status;				//订单状态
			int sum = 0;				//订单内总菜品数

			QString toJson()
			{
				QJsonObject jsonObj;

				// 填充字段
				jsonObj["merchant_id"] = merchant_id;
				jsonObj["merchant_name"] = merchant_name;
				jsonObj["consumer_id"] = consumer_id;
				jsonObj["consumer_name"] = consumer_name;
				jsonObj["time"] = time;
				jsonObj["pay"] = pay;
				jsonObj["uuid"] = uuid;
				jsonObj["sum"] = sum;

				// 转换枚举类型为字符串
				jsonObj["status"] = (status == UNPAYED) ? "UNPAYED" :
					(status == WAITING) ? "WAITING" :
					(status == OVER_TIME) ? "OVER_TIME" :
					(status == REJECTED) ? "REJECTED" :
					(status == SUCCESS) ? "SUCCESS" :
					(status == ERR) ? "ERR" : "FATAL";

				// 转换 Account::Level 枚举为字符串
				jsonObj["level"] = (level == Account::MEMBER) ? "MEMBER" :
					(level == Account::VIP) ? "VIP" : "SUVIP";

				// 将 QJsonObject 转换为 JSON 字符串
				QJsonDocument doc(jsonObj);
				return QString(doc.toJson(QJsonDocument::Compact));
			}

			bool loadFromJson(const std::string& jsonStr)
			{
				QString qJsonString = QString::fromStdString(jsonStr);

				// 解析 JSON 字符串
				QJsonDocument doc = QJsonDocument::fromJson(qJsonString.toUtf8());
				if (doc.isObject()) {
					QJsonObject jsonObj = doc.object();

					// 从 JSON 中加载数据到成员变量
					if (jsonObj.contains("merchant_id") && jsonObj["merchant_id"].isString()) {
						merchant_id = jsonObj["merchant_id"].toString();
					}

					if (jsonObj.contains("merchant_name") && jsonObj["merchant_name"].isString()) {
						merchant_name = jsonObj["merchant_name"].toString();
					}

					if (jsonObj.contains("consumer_id") && jsonObj["consumer_id"].isString()) {
						consumer_id = jsonObj["consumer_id"].toString();
					}

					if (jsonObj.contains("consumer_name") && jsonObj["consumer_name"].isString()) {
						consumer_name = jsonObj["consumer_name"].toString();
					}

					if (jsonObj.contains("time") && jsonObj["time"].isString()) {
						time = jsonObj["time"].toString();
					}

					if (jsonObj.contains("level") && jsonObj["level"].isString()) {
						QString levelStr = jsonObj["level"].toString();
						if (levelStr == "MEMBER") {
							level = Account::MEMBER;
						}
						else if (levelStr == "VIP") {
							level = Account::VIP;
						}
						else if (levelStr == "SUVIP") {
							level = Account::SUVIP;
						}
					}

					if (jsonObj.contains("pay") && jsonObj["pay"].isDouble()) {
						pay = jsonObj["pay"].toDouble();
					}

					if (jsonObj.contains("uuid") && jsonObj["uuid"].isString()) {
						uuid = jsonObj["uuid"].toString();
					}

					if (jsonObj.contains("status") && jsonObj["status"].isString()) {
						QString statusStr = jsonObj["status"].toString();
						if (statusStr == "UNPAYED") {
							status = UNPAYED;
						}
						else if (statusStr == "WAITING") {
							status = WAITING;
						}
						else if (statusStr == "OVER_TIME") {
							status = OVER_TIME;
						}
						else if (statusStr == "REJECTED") {
							status = REJECTED;
						}
						else if (statusStr == "SUCCESS") {
							status = SUCCESS;
						}
						else if (statusStr == "ERR") {
							status = ERR;
						}
						else if (statusStr == "FATAL") {
							status = FATAL;
						}
					}

					if (jsonObj.contains("sum") && jsonObj["sum"].isDouble()) {
						sum = jsonObj["sum"].toInt();
					}

					return true;

				}
				else
				{
					qDebug() << "Invalid Json string : " << jsonStr;
					return false;
				}
			}
		};

		struct OrderDish
		{
			QString order_id = "";		//订单id
			QString dish_id = "";		//菜品id
			QString merchant_id = "";	//商家id
			QString name = "";			//菜品id
			double base_price = 0;		//当时的基础价格
			double price_factor = 0;	//当时的价格影响因素
			int count = 0;				//菜品数量

			// 将结构体转换为JSON字符串
			QString toJson()
			{
				QJsonObject jsonObj;

				// 使用operator[]插入各个字段到JSON对象中
				jsonObj["order_id"] = order_id;
				jsonObj["dish_id"] = dish_id;
				jsonObj["merchant_id"] = merchant_id;
				jsonObj["name"] = name;
				jsonObj["base_price"] = base_price;
				jsonObj["price_factor"] = price_factor;
				jsonObj["count"] = count;

				// 使用QJsonDocument将QJsonObject转为JSON字符串
				QJsonDocument doc(jsonObj);
				return QString(doc.toJson(QJsonDocument::Compact));  // 返回紧凑的JSON字符串
			}

			// 从JSON字符串加载数据
			bool loadFromJson(const std::string& jsonStr)
			{
				// 将std::string转换为QString
				QString jsonString = QString::fromStdString(jsonStr);

				// 解析JSON字符串为QJsonDocument
				QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8());

				// 如果JSON解析失败，返回false
				if (!doc.isObject())
				{
					qDebug() << "Invalid JSON format.";
					return false;
				}

				// 获取QJsonObject
				QJsonObject jsonObj = doc.object();

				// 检查每个字段是否存在且类型正确，如果存在，则赋值
				if (jsonObj.contains("order_id") && jsonObj["order_id"].isString()) {
					order_id = jsonObj["order_id"].toString();
				}

				if (jsonObj.contains("dish_id") && jsonObj["dish_id"].isString()) {
					dish_id = jsonObj["dish_id"].toString();
				}

				if (jsonObj.contains("merchant_id") && jsonObj["merchant_id"].isString()) {
					merchant_id = jsonObj["merchant_id"].toString();
				}

				if (jsonObj.contains("name") && jsonObj["name"].isString()) {
					name = jsonObj["name"].toString();
				}

				if (jsonObj.contains("base_price") && jsonObj["base_price"].isDouble()) {
					base_price = jsonObj["base_price"].toDouble();
				}

				if (jsonObj.contains("price_factor") && jsonObj["price_factor"].isDouble()) {
					price_factor = jsonObj["price_factor"].toDouble();
				}

				if (jsonObj.contains("count") && jsonObj["count"].isDouble()) {
					count = jsonObj["count"].toInt();  // 由于JSON可能存储为double类型，使用toInt()转换
				}

				return true;  // 成功加载
			}
		};
	};
}