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

	void initAccountAPI();
	void initConsumerAPI();
	void initMerchantAPI();
	void initAdminAPI();
private:
	httplib::Server svr;
	DatabaseClient db;
	QString host = "127.0.0.1";
	int port = 80;
};
}


