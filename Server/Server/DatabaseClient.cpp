#include "DatabaseClient.h"
using namespace btyGoose;
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

bool btyGoose::DatabaseClient::addDish(const data::Dish&dish)
{
    QMutexLocker locker(&mtx);
    try {
        sql::PreparedStatement* pstmt = con->prepareStatement(
            "INSERT INTO dishes (uuid, merchant_id, name, icon_path, description, base_price, price_factor, is_delete,merchant_name) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?,?)"
        );
        pstmt->setString(1, dish.uuid.toStdString());
        pstmt->setString(2, dish.merchant_id.toStdString());
        pstmt->setString(3, dish.name.toStdString());
        pstmt->setString(4, dish.icon_path.toStdString());
        pstmt->setString(5, dish.description.toStdString());
        pstmt->setDouble(6, dish.base_price);
        pstmt->setDouble(7, dish.price_factor);
        pstmt->setBoolean(8, dish.is_delete);
        pstmt->setString(9, dish.merchant_name.toStdString());
        pstmt->executeUpdate();
        delete pstmt;
        return true;
    }
    catch (sql::SQLException& e) {
        qDebug() << "Error inserting dish: " << e.what();
        return false;
    }
}

bool btyGoose::DatabaseClient::updateDish(const data::Dish&dish)
{
    QMutexLocker locker(&mtx);
    try {
        sql::PreparedStatement* pstmt = con->prepareStatement(
            "UPDATE dishes SET name = ?, icon_path = ?, description = ?, base_price = ?, price_factor = ?, is_delete = ?,merchant_name = ? "
            "WHERE uuid = ?"
        );
        //qDebug() << dish.name;
        pstmt->setString(1, dish.name.toStdString());
        pstmt->setString(2, dish.icon_path.toStdString());
        pstmt->setString(3, dish.description.toStdString());
        pstmt->setDouble(4, dish.base_price);
        pstmt->setDouble(5, dish.price_factor);
        pstmt->setBoolean(6, dish.is_delete);
        pstmt->setString(7, dish.merchant_name.toStdString());
        pstmt->setString(8, dish.uuid.toStdString());
        pstmt->executeUpdate();
        delete pstmt;
        return true;
    }
    catch (sql::SQLException& e) {
        qDebug() << "Error updating dish: " << e.what();
        return false;
    }
}

btyGoose::data::Dish btyGoose::DatabaseClient::searchDishByID(const QString& id)
{
    QMutexLocker locker(&mtx);
    try {
        sql::PreparedStatement* pstmt = con->prepareStatement(
            "SELECT * FROM dishes WHERE uuid = ?"
        );
        pstmt->setString(1, id.toStdString());
        sql::ResultSet* res = pstmt->executeQuery();
        data::Dish dish;
        if (res->next()) {
            dish.uuid = QString::fromStdString(res->getString("uuid"));
            dish.merchant_id = QString::fromStdString(res->getString("merchant_id"));
            dish.merchant_name = QString::fromStdString(res->getString("merchant_name"));
            dish.name = QString::fromStdString(res->getString("name"));
            dish.icon_path = QString::fromStdString(res->getString("icon_path"));
            dish.description = QString::fromStdString(res->getString("description"));
            dish.base_price = res->getDouble("base_price");
            dish.price_factor = res->getDouble("price_factor");
            dish.is_delete = res->getBoolean("is_delete");
        }
        delete res;
        delete pstmt;
        return dish;
    }
    catch (sql::SQLException& e) {
        qDebug() << "Error searching dish by ID: " << e.what();
        return data::Dish();  // 返回默认空的 Dish 对象
    }
}

QList<btyGoose::data::Dish> btyGoose::DatabaseClient::getAllDishList()
{
    QMutexLocker locker(&mtx);
    QList<data::Dish> dishList;
    try {
        sql::Statement* stmt = con->createStatement();
        sql::ResultSet* res = stmt->executeQuery("SELECT * FROM dishes");
        while (res->next()) {
            data::Dish dish;
            dish.uuid = QString::fromStdString(res->getString("uuid"));
            dish.merchant_id = QString::fromStdString(res->getString("merchant_id"));
            dish.merchant_name = QString::fromStdString(res->getString("merchant_name"));
            dish.name = QString::fromStdString(res->getString("name"));
            dish.icon_path = QString::fromStdString(res->getString("icon_path"));
            dish.description = QString::fromStdString(res->getString("description"));
            dish.base_price = res->getDouble("base_price");
            dish.price_factor = res->getDouble("price_factor");
            dish.is_delete = res->getBoolean("is_delete");
            if(dish.is_delete == false)
                dishList.append(dish);
        }
        delete res;
        delete stmt;
    }
    catch (sql::SQLException& e) {
        qDebug() << "Error getting all dishes: " << e.what();
    }
    return dishList;
}



QList<btyGoose::data::Dish> btyGoose::DatabaseClient::getDishListByMerchant(const QString& id)
{
    QMutexLocker locker(&mtx);
    QList<data::Dish> dishList;
    try {
        sql::PreparedStatement* pstmt = con->prepareStatement(
            "SELECT * FROM dishes WHERE merchant_id = ?"
        );
        pstmt->setString(1, id.toStdString());
        sql::ResultSet* res = pstmt->executeQuery();
        while (res->next()) {
            data::Dish dish;
            dish.uuid = QString::fromStdString(res->getString("uuid"));
            dish.merchant_id = QString::fromStdString(res->getString("merchant_id"));
            dish.merchant_name = QString::fromStdString(res->getString("merchant_name"));
            dish.name = QString::fromStdString(res->getString("name"));
            dish.icon_path = QString::fromStdString(res->getString("icon_path"));
            dish.description = QString::fromStdString(res->getString("description"));
            dish.base_price = res->getDouble("base_price");
            dish.price_factor = res->getDouble("price_factor");
            dish.is_delete = res->getBoolean("is_delete");
            if(dish.is_delete == false)
                dishList.append(dish);
        }
        delete res;
        delete pstmt;
    }
    catch (sql::SQLException& e) {
        qDebug() << "Error getting dishes by merchant: " << e.what();
    }
    return dishList;
}

bool btyGoose::DatabaseClient::delDishByID(const QString& id)
{
    QMutexLocker locker(&mtx);
    try {
        // 更新 is_delete 字段为 true，表示菜品被删除
        sql::PreparedStatement* pstmt = con->prepareStatement(
            "UPDATE dishes SET is_delete = ? WHERE uuid = ?"
        );
        pstmt->setBoolean(1, true);  // 设置 is_delete 为 true
        pstmt->setString(2, id.toStdString());  // 根据 uuid 找到菜品
        pstmt->executeUpdate();
        delete pstmt;
        return true;
    }
    catch (sql::SQLException& e) {
        qDebug() << "Error logically deleting dish by ID: " << e.what();
        return false;
    }
}

//

// 添加订单
bool DatabaseClient::addOrder(const data::Order& order) {
    QMutexLocker locker(&mtx);
    try {
        sql::PreparedStatement* pstmt = con->prepareStatement(
            "INSERT INTO orders (uuid, merchant_id, merchant_name, consumer_id, consumer_name, time, level, pay, status,sum) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?,?)"
        );
        pstmt->setString(1, order.uuid.toStdString());
        pstmt->setString(2, order.merchant_id.toStdString());
        pstmt->setString(3, order.merchant_name.toStdString());
        pstmt->setString(4, order.consumer_id.toStdString());
        pstmt->setString(5, order.consumer_name.toStdString());
        pstmt->setString(6, order.time.toStdString());
        pstmt->setInt(7, order.level);
        pstmt->setDouble(8, order.pay);
        pstmt->setInt(9, static_cast<int>(order.status));  // Status 转换为 int
        pstmt->setInt(10, order.sum);  // Status 转换为 int
        pstmt->executeUpdate();
        delete pstmt;
        return true;
    }
    catch (sql::SQLException& e) {
        qDebug() << "Error adding order: " << e.what();
        return false;
    }
}

// 更新订单
bool DatabaseClient::updateOrder(const data::Order& order) {
    QMutexLocker locker(&mtx);
    try {
        sql::PreparedStatement* pstmt = con->prepareStatement(
            "UPDATE orders SET merchant_id = ?, merchant_name = ?, consumer_id = ?, consumer_name = ?, time = ?, "
            "level = ?, pay = ?, status = ? WHERE uuid = ?"
        );
        pstmt->setString(1, order.merchant_id.toStdString());
        pstmt->setString(2, order.merchant_name.toStdString());
        pstmt->setString(3, order.consumer_id.toStdString());
        pstmt->setString(4, order.consumer_name.toStdString());
        pstmt->setString(5, order.time.toStdString());
        pstmt->setInt(6, order.level);
        pstmt->setDouble(7, order.pay);
        pstmt->setInt(8, static_cast<int>(order.status));  // Status 转换为 int
        pstmt->setString(9, order.uuid.toStdString());
        pstmt->executeUpdate();
        delete pstmt;
        return true;
    }
    catch (sql::SQLException& e) {
        qDebug() << "Error updating order: " << e.what();
        return false;
    }
}

// 更新订单状态
bool DatabaseClient::updateOrder(const QString& id, const data::Order::Status stat) {
    QMutexLocker locker(&mtx);
    try {
        sql::PreparedStatement* pstmt = con->prepareStatement(
            "UPDATE orders SET status = ? WHERE uuid = ?"
        );
        pstmt->setInt(1, static_cast<int>(stat));  // Status 转换为 int
        pstmt->setString(2, id.toStdString());
        pstmt->executeUpdate();
        delete pstmt;
        return true;
    }
    catch (sql::SQLException& e) {
        qDebug() << "Error updating order status: " << e.what();
        return false;
    }
}

// 查找订单
data::Order DatabaseClient::searchOrderByID(const QString& order_id) {
    QMutexLocker locker(&mtx);
    try {
        sql::PreparedStatement* pstmt = con->prepareStatement(
            "SELECT * FROM orders WHERE uuid = ?"
        );
        pstmt->setString(1, order_id.toStdString());
        sql::ResultSet* res = pstmt->executeQuery();
        data::Order order;
        if (res->next()) {
            order.uuid = QString::fromStdString(res->getString("uuid"));
            order.merchant_id = QString::fromStdString(res->getString("merchant_id"));
            order.merchant_name = QString::fromStdString(res->getString("merchant_name"));
            order.consumer_id = QString::fromStdString(res->getString("consumer_id"));
            order.consumer_name = QString::fromStdString(res->getString("consumer_name"));
            order.time = QString::fromStdString(res->getString("time"));
            order.level = static_cast<data::Account::Level>(res->getInt("level"));
            order.pay = res->getDouble("pay");
            order.status = static_cast<data::Order::Status>(res->getInt("status"));
        }
        delete res;
        delete pstmt;
        return order;
    }
    catch (sql::SQLException& e) {
        qDebug() << "Error searching order by ID: " << e.what();
        return data::Order();  // 返回默认空的 Order 对象
    }
}

// 获取商家的所有订单
QList<data::Order> DatabaseClient::getOrderListByMerchant(const QString& merchant_id) {
    QMutexLocker locker(&mtx);
    QList<data::Order> orderList;
    try {
        sql::PreparedStatement* pstmt = con->prepareStatement(
            "SELECT * FROM orders WHERE merchant_id = ?"
        );
        pstmt->setString(1, merchant_id.toStdString());
        sql::ResultSet* res = pstmt->executeQuery();
        while (res->next()) {
            data::Order order;
            order.uuid = QString::fromStdString(res->getString("uuid"));
            order.merchant_id = QString::fromStdString(res->getString("merchant_id"));
            order.merchant_name = QString::fromStdString(res->getString("merchant_name"));
            order.consumer_id = QString::fromStdString(res->getString("consumer_id"));
            order.consumer_name = QString::fromStdString(res->getString("consumer_name"));
            order.time = QString::fromStdString(res->getString("time"));
            order.level = static_cast<data::Account::Level>(res->getInt("level"));
            order.pay = res->getDouble("pay");
            order.status = static_cast<data::Order::Status>(res->getInt("status"));
            order.sum = res->getInt("sum");
            orderList.append(order);
        }
        delete res;
        delete pstmt;
    }
    catch (sql::SQLException& e) {
        qDebug() << "Error getting orders by merchant: " << e.what();
    }
    return orderList;
}

QList<data::Order> btyGoose::DatabaseClient::getOrderListByMerchantWaiting(const QString& merchant_id)
{
    QMutexLocker locker(&mtx);
    QList<data::Order> orderList;
    try {
        sql::PreparedStatement* pstmt = con->prepareStatement(
            "SELECT * FROM orders WHERE merchant_id = ? and status = ?"
        );
        pstmt->setString(1, merchant_id.toStdString());
        pstmt->setInt(2, static_cast<int>(data::Order::Status::WAITING));
        qDebug() << merchant_id <<"|"<< static_cast<int>(data::Order::Status::WAITING);
        sql::ResultSet* res = pstmt->executeQuery();
        while (res->next()) {
            data::Order order;
            order.uuid = QString::fromStdString(res->getString("uuid"));
            order.merchant_id = QString::fromStdString(res->getString("merchant_id"));
            order.merchant_name = QString::fromStdString(res->getString("merchant_name"));
            order.consumer_id = QString::fromStdString(res->getString("consumer_id"));
            order.consumer_name = QString::fromStdString(res->getString("consumer_name"));
            order.time = QString::fromStdString(res->getString("time"));
            order.level = static_cast<data::Account::Level>(res->getInt("level"));
            order.pay = res->getDouble("pay");
            order.status = static_cast<data::Order::Status>(res->getInt("status"));
            order.sum = res->getInt("sum");
            orderList.append(order);
        }
        delete res;
        delete pstmt;
    }
    catch (sql::SQLException& e) {
        qDebug() << "Error getting orders by merchant: " << e.what();
    }
    return orderList;
}

QList<data::Order> btyGoose::DatabaseClient::getOrderListByConsumer(const QString& consumer_id)
{
    QMutexLocker locker(&mtx);
    QList<data::Order> orderList;
    try {
        sql::PreparedStatement* pstmt = con->prepareStatement(
            "SELECT * FROM orders WHERE consumer_id = ?"
        );
        pstmt->setString(1, consumer_id.toStdString());
        sql::ResultSet* res = pstmt->executeQuery();
        while (res->next()) {
            data::Order order;
            order.uuid = QString::fromStdString(res->getString("uuid"));
            order.merchant_id = QString::fromStdString(res->getString("merchant_id"));
            order.merchant_name = QString::fromStdString(res->getString("merchant_name"));
            order.consumer_id = QString::fromStdString(res->getString("consumer_id"));
            order.consumer_name = QString::fromStdString(res->getString("consumer_name"));
            order.time = QString::fromStdString(res->getString("time"));
            order.level = static_cast<data::Account::Level>(res->getInt("level"));
            order.pay = res->getDouble("pay");
            order.status = static_cast<data::Order::Status>(res->getInt("status"));
            order.sum = res->getInt("sum");
            orderList.append(order);
        }
        delete res;
        delete pstmt;
    }
    catch (sql::SQLException& e) {
        qDebug() << "Error getting orders by merchant: " << e.what();
    }
    return orderList;
}

// 删除订单
bool DatabaseClient::delOrderByID(const QString& order_id) {
    QMutexLocker locker(&mtx);
    try {
        sql::PreparedStatement* pstmt = con->prepareStatement(
            "DELETE FROM orders WHERE uuid = ?"
        );
        pstmt->setString(1, order_id.toStdString());
        pstmt->executeUpdate();
        delete pstmt;
        return true;
    }
    catch (sql::SQLException& e) {
        qDebug() << "Error deleting order by ID: " << e.what();
        return false;
    }
}



int btyGoose::DatabaseClient::clearOverTimeOrder()
{
    return 0;
}

// 添加订单菜品
bool DatabaseClient::addOrderDishesByID(const QString& order_id, const QList<data::OrderDish>& list) {
    QMutexLocker locker(&mtx);
    try {
        for (const auto& dish : list) {
            sql::PreparedStatement* pstmt = con->prepareStatement(
                "INSERT INTO orderDish (order_id, dish_id, merchant_id, name, dish_price, count) "
                "VALUES (?, ?, ?, ?, ?, ?)"
            );
            pstmt->setString(1, order_id.toStdString());
            pstmt->setString(2, dish.dish_id.toStdString());
            pstmt->setString(3, dish.merchant_id.toStdString());
            pstmt->setString(4, dish.name.toStdString());
            pstmt->setDouble(5, dish.dish_price);
            pstmt->setInt(6, dish.count);
            pstmt->executeUpdate();
            delete pstmt;
        }
        return true;
    }
    catch (sql::SQLException& e) {
        qDebug() << "Error adding order dishes: " << e.what();
        return false;
    }
}

// 删除订单菜品
bool DatabaseClient::delOrderDishesByID(const QString& order_id) {
    QMutexLocker locker(&mtx);
    try {
        sql::PreparedStatement* pstmt = con->prepareStatement(
            "DELETE FROM orderDish WHERE order_id = ?"
        );
        pstmt->setString(1, order_id.toStdString());
        pstmt->executeUpdate();
        delete pstmt;
        return true;
    }
    catch (sql::SQLException& e) {
        qDebug() << "Error deleting order dishes by order ID: " << e.what();
        return false;
    }
}

// 获取订单菜品列表
QList<data::OrderDish> DatabaseClient::getOrderDishesByID(const QString& order_id) {
    QMutexLocker locker(&mtx);
    QList<data::OrderDish> dishList;
    try {
        sql::PreparedStatement* pstmt = con->prepareStatement(
            "SELECT * FROM orderDish WHERE order_id = ?"
        );
        pstmt->setString(1, order_id.toStdString());
        sql::ResultSet* res = pstmt->executeQuery();

        while (res->next()) {
            data::OrderDish dish;
            dish.order_id = QString::fromStdString(res->getString("order_id"));
            dish.dish_id = QString::fromStdString(res->getString("dish_id"));
            dish.merchant_id = QString::fromStdString(res->getString("merchant_id"));
            dish.name = QString::fromStdString(res->getString("name"));
            dish.dish_price = res->getDouble("dish_price");
            dish.count = res->getInt("count");
            dishList.append(dish);
        }

        delete res;
        delete pstmt;
    }
    catch (sql::SQLException& e) {
        qDebug() << "Error fetching order dishes by order ID: " << e.what();
    }
    return dishList;
}

bool btyGoose::DatabaseClient::addHistory(const data::Order& order)
{
    QMutexLocker locker(&mtx);  // 使用互斥锁保护数据库操作

    try {
        // 插入 SQL 语句
        std::string query = "INSERT INTO history (time, uuid, merchant_id, merchant_name, "
            "consumer_id, consumer_name, level, pay, status,sum) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?,?)";

        // 创建准备语句
        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement(query));

        // 设置参数
        pstmt->setString(1, order.time.toStdString());
        pstmt->setString(2, order.uuid.toStdString());
        pstmt->setString(3, order.merchant_id.toStdString());
        pstmt->setString(4, order.merchant_name.toStdString());
        pstmt->setString(5, order.consumer_id.toStdString());
        pstmt->setString(6, order.consumer_name.toStdString());
        pstmt->setInt(7, static_cast<int>(order.level));
        pstmt->setDouble(8, order.pay);
        pstmt->setInt(9, static_cast<int>(order.status));
        pstmt->setInt(10, order.sum);

        // 执行插入操作
        pstmt->executeUpdate();
        return true;
    }
    catch (sql::SQLException& e) {
        qDebug() << "插入历史记录失败：" << e.what();
        return false;
    }
}

QList<data::Order> btyGoose::DatabaseClient::getAllHistoryList()
{
    QMutexLocker locker(&mtx);  // 使用互斥锁保护数据库操作
    QList<data::Order> orderList;

    try {
        // 查询所有历史记录
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT * FROM history"));

        // 遍历查询结果
        while (res->next()) {
            data::Order order;
            order.time = QString::fromStdString(res->getString("time"));
            order.uuid = QString::fromStdString(res->getString("uuid"));
            order.merchant_id = QString::fromStdString(res->getString("merchant_id"));
            order.merchant_name = QString::fromStdString(res->getString("merchant_name"));
            order.consumer_id = QString::fromStdString(res->getString("consumer_id"));
            order.consumer_name = QString::fromStdString(res->getString("consumer_name"));
            order.level = static_cast<data::Account::Level>(res->getInt("level"));
            order.pay = res->getDouble("pay");
            order.status = static_cast<data::Order::Status>(res->getInt("status"));
            order.sum = res->getInt("sum");

            orderList.append(order);  // 添加到结果列表
        }
    }
    catch (sql::SQLException& e) {
        qDebug() << "查询历史记录失败：" << e.what();
    }

    return orderList;
}

bool DatabaseClient::addHistoryDishesByID(const QString& order_id, const QList<data::OrderDish>& list)
{
    try
    {
        // Start a transaction
        con->setAutoCommit(false);

        // Prepare the SQL query for inserting history dish records
        std::string query = "INSERT INTO historyDish(order_id, dish_id, merchant_id, name, dish_price, count) "
            "VALUES (?, ?, ?, ?, ?, ?)";

        // Prepare the statement
        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement(query));

        for (const auto& dish : list)
        {
            // Set the parameters for the prepared statement
            pstmt->setString(1, order_id.toStdString());  // order_id
            pstmt->setString(2, dish.dish_id.toStdString());  // dish_id
            pstmt->setString(3, dish.merchant_id.toStdString());  // merchant_id
            pstmt->setString(4, dish.name.toStdString());  // name
            pstmt->setDouble(5, dish.dish_price);  // dish_price
            pstmt->setInt(6, dish.count);  // count

            // Execute the prepared statement
            pstmt->executeUpdate();
        }

        // Commit the transaction
        con->commit();
        return true;
    }
    catch (sql::SQLException& e)
    {
        // Rollback in case of error
        con->rollback();
        qWarning() << "Error adding history dishes: " << e.what();
        return false;
    }
}

QList<data::OrderDish> DatabaseClient::getHistoryDishesByID(const QString& order_id)
{
    QList<data::OrderDish> dishList;

    try
    {
        // Prepare the SQL query to select dishes by order_id
        std::string query = "SELECT dish_id, merchant_id, name, dish_price, count FROM historyDish WHERE order_id = ?";

        // Prepare the statement
        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement(query));
        pstmt->setString(1, order_id.toStdString());

        // Execute the query
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        // Iterate through the result set and populate the dish list
        while (res->next())
        {
            data::OrderDish dish;
            dish.dish_id = QString::fromStdString(res->getString("dish_id"));
            dish.merchant_id = QString::fromStdString(res->getString("merchant_id"));
            dish.name = QString::fromStdString(res->getString("name"));
            dish.dish_price = res->getDouble("dish_price");
            dish.count = res->getInt("count");

            dishList.append(dish);
        }
    }
    catch (sql::SQLException& e)
    {
        qWarning() << "Error retrieving history dishes: " << e.what();
    }

    return dishList;
}