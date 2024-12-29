#include "datacenter.h"

DataCenter::DataCenter(const QString &host, int port):client(host.toStdString(),port) {}

QString DataCenter::getAuthcode(const QString &phoneNumber)
{

}

bool DataCenter::registerAccount()
{
    return true;
}

bool DataCenter::loginByPhone()
{
    return true;
}
