#pragma once
#include "httplib.h"
#include "DatabaseClient.h"


namespace btyGoose
{

class HTTPServer
{
	class HTTPException
	{
	private:
		QString message;
	public:
		HTTPException(const QString& msg = "a problem")
			:message(msg)
		{}

		QString what()const {
			return message;
		}
	};
public:
	HTTPServer();

	bool loadConfig();
	void saveConfig();

	void initTestAPI();
	void initAccountAPI();
	bool AuthenticateAuthCode(const QString& phone,const QString&auth_code);	//这里留着接入第三方API
	void initConsumerAPI();
	void initMerchantAPI();
	void initAdminAPI();

	bool getPay(double pay);
	//工具函数
	QString toJsonArray(const QList<data::Dish>& dishList);
private:
	httplib::Server svr;
	DatabaseClient db;
	QString host = "127.0.0.1";
	int port = 80;
};
}


