#include "DatabaseClient.h"

btyGoose::DatabaseClient::DatabaseClient()
{
    if (loadConfig() == false)
    {
        //qDebug() << "配置文件加载出错";
        qDebug() << "fail to load";
        QCoreApplication::quit();
    }
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setDatabaseName(database);
    db.setHostName(host);
    db.setUserName(user);
    db.setPassword(password);

}

bool btyGoose::DatabaseClient::loadConfig()
{
    QFile file("databaseConfig.json");
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);

        if (doc.isObject()) {
            QJsonObject jsonObject = doc.object();
            user = jsonObject["user"].toString();
            password = jsonObject["password"].toString();
            host = jsonObject["host"].toString();
            port = jsonObject["port"].toString();
            database = jsonObject["database"].toString();
            return true;
        }
    }
    //反序列化失败
    user = "unkown";
    password = "unkown";
    database = "unkown";
    saveConfig();
    return false;
}

void btyGoose::DatabaseClient::saveConfig()
{
    QFile file("databaseConfig.json");
    if (file.open(QIODevice::WriteOnly)) {
        QJsonObject jsonObject;
        jsonObject["user"] = user;
        jsonObject["password"] = password;
        jsonObject["host"] = host;
        jsonObject["port"] = port;
        jsonObject["database"] = database;

        QJsonDocument doc(jsonObject);
        file.write(doc.toJson());
        file.close();
    }
    else
    {
        std::cerr << "database配置文件打开错误";
    }
}

bool btyGoose::DatabaseClient::addAccount(const data::Account& account)
{
    // 创建数据库连接并验证
    if (!db.isOpen() && !db.open()) {
        qDebug() << "Database connection failed!";
        return false;
    }

    // 构建插入 SQL 语句
    QSqlQuery query;
    query.prepare("INSERT INTO accounts (uuid, name, password, nickname, icon_data, type, balance, phone, level) "
        "VALUES (:uuid, :name, :password, :nickname, :icon, :type, :balance, :phone, :level)");

    // 绑定参数
    query.bindValue(":uuid", account.uuid);
    query.bindValue(":name", account.name);
    query.bindValue(":password", account.password);
    query.bindValue(":nickname", account.nickname);
    query.bindValue(":icon", account.icon);
    query.bindValue(":type", static_cast<int>(account.type));  // 将枚举转为 int 存储
    query.bindValue(":balance", account.balance);
    query.bindValue(":phone", account.phone);
    query.bindValue(":level", static_cast<int>(account.level));  // 将枚举转为 int 存储

    // 执行查询
    if (!query.exec()) {
        qDebug() << "Failed to insert account: " << query.lastError().text();
        return false;
    }

    qDebug() << "Account added successfully!";
    return true;
    
}

bool btyGoose::DatabaseClient::updataAccount(const data::Account&)
{
    return false;
}

btyGoose::data::Account btyGoose::DatabaseClient::searchAccountByID(const QString& id)
{
    return data::Account();
}

btyGoose::data::Account btyGoose::DatabaseClient::searchAccountByName(const QString& name)
{
    return data::Account();
}

btyGoose::data::Account btyGoose::DatabaseClient::searchAccountByPhone(const QString& phone)
{
    return data::Account();
}

bool btyGoose::DatabaseClient::delAccountByID(const QString& id)
{
    return false;
}

bool btyGoose::DatabaseClient::addDish(const data::Dish&)
{
    return false;
}

bool btyGoose::DatabaseClient::updateDish(const data::Dish&)
{
    return false;
}

btyGoose::data::Dish btyGoose::DatabaseClient::searchDishByID(const QString& id)
{
    return data::Dish();
}

QList<btyGoose::data::Dish> btyGoose::DatabaseClient::getDishListByMerchant(const QString& id)
{
    return QList<data::Dish>();
}

bool btyGoose::DatabaseClient::delDishByID(const QString& id)
{
    return false;
}

bool btyGoose::DatabaseClient::addOrder(const data::Order&)
{
    return false;
}

bool btyGoose::DatabaseClient::updateOrder(const data::Order&)
{
    return false;
}

bool btyGoose::DatabaseClient::updateOrder(const data::Order::Status)
{
    return false;
}

btyGoose::data::Order btyGoose::DatabaseClient::searchOrderByID(const QString& id)
{
    return data::Order();
}

QList<btyGoose::data::Order> btyGoose::DatabaseClient::getOrderListByMerchant(const QString& id)
{
    return QList<data::Order>();
}

bool btyGoose::DatabaseClient::delOrderByID(const QString& id)
{
    return false;
}

int btyGoose::DatabaseClient::clearOverTimeOrder()
{
    return 0;
}

bool btyGoose::DatabaseClient::addHistory(const data::Order&)
{
    return false;
}

bool btyGoose::DatabaseClient::clearOverTimeHistory()
{
    return false;
}

QList<btyGoose::data::Order> btyGoose::DatabaseClient::getAllHistory()
{
    return QList<data::Order>();
}
