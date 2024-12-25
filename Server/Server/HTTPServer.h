#pragma once
#include "httplib.h"
#include "DatabaseClient.h"

namespace btyGoose
{

class HTTPServer
{
	HTTPServer();

	bool loadConfig();
	void saveConfig();
private:
	httplib::Server server;
	DatabaseClient db;
	QString host = "127.0.0.1";
	QString port = "3306";
};
}


