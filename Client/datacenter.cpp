#include "datacenter.h"

DataCenter::DataCenter(const QString &host, int port)
{
    if(!loadConfig())
    {
        qDebug()<<configFileName<<"服务器地址配置错误";
        exit(1);
    }
    client = QSharedPointer<httplib::Client>(new httplib::Client(host.toStdString(),port));
}

bool DataCenter::loadConfig()
{
    QFile file(configFileName);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);

        if (doc.isObject()) {
            QJsonObject jsonObject = doc.object();
            host = jsonObject["host"].toString();
            port = jsonObject["port"].toInt();

            return true;
        }
    }
    return false;
}

bool DataCenter::getAuthcode(const QString &phoneNumber)
{
    return true;
}


bool DataCenter::registerAccount()
{
    return true;
}

bool DataCenter::loginByPhone()
{
    return true;
}
