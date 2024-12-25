#pragma once
//====管理核心数据类=========
#include <qstring.h>
#include <qbytearray.h>

#include <QString>
#include <QUuid>
#include <QDateTime>
#include <QFile>
#include <QDebug>
#include <QFileInfo>
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
			QString nickname = "";	//昵称
			QByteArray icon = QByteArray();	//图标
			Type type = CONSUMER;			//账户类型
			double balance = 0;		//账户余额
			QString phone = "";		//电话号码
			Level level = MEMBER;		//优惠等级
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
			QString dish_id;			//菜品id
			QString dish_name;			//菜品名称
			QString base_price;			//基础价格
			QString price_factor;		//价格影响因素
			QString time;				//订单时间
			Account::Level level;		//优惠等级
			double pay;					//订单价格
			QString uuid;				//订单id
			Status status;				//订单状态
		};
	}
}