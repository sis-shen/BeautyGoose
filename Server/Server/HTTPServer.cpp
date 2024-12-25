#include "HTTPServer.h"

btyGoose::HTTPServer::HTTPServer()
{

}

bool btyGoose::HTTPServer::loadConfig()
{

    QFile file("serverConfig.json");
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);

        if (doc.isObject()) {
            QJsonObject jsonObject = doc.object();
            host = jsonObject["host"].toString();
            port = jsonObject["port"].toString();
            
            return true;
        }
    }
    //∑¥–Ú¡–ªØ ß∞‹
    user = "unkown";
    password = "unkown";
    database = "unkown";
    saveConfig();
    return false;
}
