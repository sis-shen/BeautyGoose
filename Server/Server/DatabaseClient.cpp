#include "DatabaseClient.h"

btyGoose::DatabaseClient::DatabaseClient()
{
    if (!loadConfig())
    {
        saveConfig();
        exit(1);
    }
    sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
    std::string hostName = "tcp://";
    hostName += host.toStdString();
    hostName += ":";
    hostName += port.toStdString();
    con = driver->connect(hostName, user.toStdString(), password.toStdString());
    con->setSchema(database.toStdString());
    LOG() << "数据库初始连接状态:" << !con->isClosed();
    qDebug() << "数据库地址:" << hostName;
    qDebug() << "数据库用户名:" << user;
    qDebug() << "数据库名称" << database;
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
    // 使用 QMutexLocker 确保线程安全
    QMutexLocker locker(&mtx);

    try {
        // 构建 SQL 插入语句
        QString sql = QString(
            "INSERT INTO account (uuid, phone, name, password, nickname, level, icon_data, type, balance) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)"
        );

        // 创建预处理语句
        sql::PreparedStatement* pstmt = con->prepareStatement(sql.toStdString());

        // 绑定参数
        pstmt->setString(1, account.uuid.toStdString());
        pstmt->setString(2, account.phone.toStdString());
        pstmt->setString(3, account.name.toStdString());
        pstmt->setString(4, account.password.toStdString());
        pstmt->setString(5, account.nickname.toStdString());
        pstmt->setInt(6, static_cast<int>(account.level));  // 使用 int 存储 Level 枚举
        // 使用 std::istringstream 来处理图标数据
        std::istringstream iconStream(std::string(account.icon.data(), account.icon.size()));
        pstmt->setBlob(7, &iconStream);  // 使用 std::istream* 作为 BLOB 类型
        pstmt->setInt(8, static_cast<int>(account.type));  // 使用 int 存储 Type 枚举
        pstmt->setInt64(9, static_cast<long long>(account.balance));  // balance 作为 int unsigned，转为 long long

        // 执行插入操作
        pstmt->executeUpdate();

        // 清理资源
        delete pstmt;

        return true;
    }
    catch (sql::SQLException& e) {
        qDebug() << "Error while inserting account: " << e.what();
        return false;
    }
}

bool btyGoose::DatabaseClient::updataAccount(const data::Account&)
{
    return false;
}
//btyGoose::data::Account btyGoose::DatabaseClient::searchAccountByID(const QString& id)
//{
//    return btyGoose::data::Account();
//}
//
//btyGoose::data::Account btyGoose::DatabaseClient::searchAccountByName(const QString& name)
//{
//    return btyGoose::data::Account();
//}
//
//btyGoose::data::Account btyGoose::DatabaseClient::searchAccountByPhone(const QString& phone)
//{
//    return btyGoose::data::Account();
//}

btyGoose::data::Account btyGoose::DatabaseClient::searchAccountByID(const QString& id)
{
    QMutexLocker locker(&mtx);  // 确保线程安全

    try {
        // 准备 SQL 查询
        QString sql = QString("SELECT * FROM account WHERE uuid = ?");

        // 创建 SQL 语句对象
        sql::PreparedStatement* pstmt = con->prepareStatement(sql.toStdString());

        // 绑定参数
        pstmt->setString(1, id.toStdString());

        // 执行查询
        sql::ResultSet* res = pstmt->executeQuery();

        // 检查是否有查询结果
        if (res->next()) {
            // 将查询结果映射到 Account 结构体
            btyGoose::data::Account account;
            account.uuid = QString::fromStdString(res->getString("uuid"));
            account.phone = QString::fromStdString(res->getString("phone"));
            account.name = QString::fromStdString(res->getString("name"));
            account.password = QString::fromStdString(res->getString("password"));
            account.nickname = QString::fromStdString(res->getString("nickname"));
            account.level = static_cast<btyGoose::data::Account::Level>(res->getInt("level"));
            account.type = static_cast<btyGoose::data::Account::Type>(res->getInt("type"));
            account.balance = res->getDouble("balance");

            // 处理 BLOB 数据
            std::istream* blobStream = res->getBlob("icon_data");
            if (blobStream) {
                std::ostringstream oss;
                oss << blobStream->rdbuf();
                account.icon = QByteArray::fromStdString(oss.str());
            }

            // 清理资源
            delete res;
            delete pstmt;

            return account;
        }

        // 如果没有找到记录，返回一个空账户
        delete res;
        delete pstmt;
        return btyGoose::data::Account();
    }
    catch (sql::SQLException& e) {
        qDebug() << "Error while searching account by ID: " << e.what();
        return btyGoose::data::Account();
    }
}

btyGoose::data::Account btyGoose::DatabaseClient::searchAccountByName(const QString& name)
{
    QMutexLocker locker(&mtx);  // 确保线程安全

    try {
        // 准备 SQL 查询
        QString sql = QString("SELECT * FROM account WHERE name = ?");

        // 创建 SQL 语句对象
        sql::PreparedStatement* pstmt = con->prepareStatement(sql.toStdString());

        // 绑定参数
        pstmt->setString(1, name.toStdString());

        // 执行查询
        sql::ResultSet* res = pstmt->executeQuery();

        // 检查是否有查询结果
        if (res->next()) {
            // 将查询结果映射到 Account 结构体
            btyGoose::data::Account account;
            account.uuid = QString::fromStdString(res->getString("uuid"));
            account.phone = QString::fromStdString(res->getString("phone"));
            account.name = QString::fromStdString(res->getString("name"));
            account.password = QString::fromStdString(res->getString("password"));
            account.nickname = QString::fromStdString(res->getString("nickname"));
            account.level = static_cast<btyGoose::data::Account::Level>(res->getInt("level"));
            account.type = static_cast<btyGoose::data::Account::Type>(res->getInt("type"));
            account.balance = res->getDouble("balance");

            // 处理 BLOB 数据
            std::istream* blobStream = res->getBlob("icon_data");
            if (blobStream) {
                std::ostringstream oss;
                oss << blobStream->rdbuf();
                account.icon = QByteArray::fromStdString(oss.str());
            }

            // 清理资源
            delete res;
            delete pstmt;

            return account;
        }

        // 如果没有找到记录，返回一个空账户
        delete res;
        delete pstmt;
        return btyGoose::data::Account();
    }
    catch (sql::SQLException& e) {
        qDebug() << "Error while searching account by name: " << e.what();
        return btyGoose::data::Account();
    }
}

btyGoose::data::Account btyGoose::DatabaseClient::searchAccountByPhone(const QString& phone)
{
    QMutexLocker locker(&mtx);  // 确保线程安全

    try {
        // 准备 SQL 查询
        QString sql = QString("SELECT * FROM account WHERE phone = ?");

        // 创建 SQL 语句对象
        sql::PreparedStatement* pstmt = con->prepareStatement(sql.toStdString());

        // 绑定参数
        pstmt->setString(1, phone.toStdString());

        // 执行查询
        sql::ResultSet* res = pstmt->executeQuery();

        // 检查是否有查询结果
        if (res->next()) {
            // 将查询结果映射到 Account 结构体
            btyGoose::data::Account account;
            account.uuid = QString::fromStdString(res->getString("uuid"));
            account.phone = QString::fromStdString(res->getString("phone"));
            account.name = QString::fromStdString(res->getString("name"));
            account.password = QString::fromStdString(res->getString("password"));
            account.nickname = QString::fromStdString(res->getString("nickname"));
            account.level = static_cast<btyGoose::data::Account::Level>(res->getInt("level"));
            account.type = static_cast<btyGoose::data::Account::Type>(res->getInt("type"));
            account.balance = res->getDouble("balance");

            // 处理 BLOB 数据
            std::istream* blobStream = res->getBlob("icon_data");
            if (blobStream) {
                std::ostringstream oss;
                oss << blobStream->rdbuf();
                account.icon = QByteArray::fromStdString(oss.str());
            }

            // 清理资源
            delete res;
            delete pstmt;

            return account;
        }

        // 如果没有找到记录，返回一个空账户
        delete res;
        delete pstmt;
        return btyGoose::data::Account();
    }
    catch (sql::SQLException& e) {
        qDebug() << "Error while searching account by phone: " << e.what();
        return btyGoose::data::Account();
    }
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
