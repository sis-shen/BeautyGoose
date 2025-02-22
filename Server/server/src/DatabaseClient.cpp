#include "DatabaseClient.hpp"
#include <fstream>
#include <jsoncpp/json/json.h>

using namespace btyGoose;
using namespace std;

DatabaseClient::DatabaseClient()
{
    try {
        if(!loadConfig())
        {
            saveConfig();
            exit(-1);
        }
        sql::mysql::MySQL_Driver *driver = sql::mysql::get_mysql_driver_instance();
        con = driver->connect("tcp://"+host+":"+port, "btyGooseUser", "low_password");
        con->setSchema("BeautyGoose");
        stmt = con->createStatement();
    } catch (sql::SQLException &e) {
        cerr << "数据库连接失败喵！错误码: " << e.getErrorCode() 
             << ", 信息: " << e.what() << endl;
    }
}

void DatabaseClient::saveConfig()
{
    Json::Value root;
    root["user"] = user;
    root["password"] = password;
    root["host"] = host;
    root["port"] = port;
    root["database"] = database;

    Json::StreamWriterBuilder writer;
    writer["indentation"] = "    ";
    std::ofstream ofs("DBConfig.json",std::ios::out | std::ios::trunc);

    ofs << Json::writeString(writer, root);
    ofs.close();
}

bool DatabaseClient::loadConfig()
{
    std::ifstream ifs("DBConfig.json",std::ios::in);
    if(!ifs.is_open())
    {
        std::cerr<<"数据库配置文件打开失败\n";
        return false;
    }

    Json::Value root;
    Json::CharReaderBuilder readerBuilder;
    std::string errs;

    bool parsingSuccessful = Json::parseFromStream(readerBuilder, ifs, &root, &errs);

    if (!parsingSuccessful) {
        // 错误处理
        throw std::runtime_error("JSON解析失败: " + errs);
    }

    user = root["user"].asString();
    password = root["password"].asString();
    host = root["host"].asString();
    port = root["port"].asString() ;
    database = root["database"].asString();

    return true;
}

//━━━━━━━━━━━━━━ Account CRUD实现 ━━━━━━━━━━━━━━//
bool DatabaseClient::addAccount(const data::Account& acc) {
    lock_guard<mutex> lock(mtx);
    try {
        auto pstmt = con->prepareStatement(
            "INSERT INTO account(uuid, phone, name, password, nickname, "
            "level, icon_data, type, balance) VALUES (?,?,?,?,?,?,?,?,?)"
        );
        
        pstmt->setString(1, acc.uuid);
        pstmt->setString(2, acc.phone);
        pstmt->setString(3, acc.name);
        pstmt->setString(4, acc.password);
        pstmt->setString(5, acc.nickname);
        pstmt->setInt(6, static_cast<int>(acc.level));
        pstmt->setString(7, acc.icon); // BLOB数据处理
        pstmt->setInt(8, static_cast<int>(acc.type));
        pstmt->setDouble(9, acc.balance);

        return pstmt->executeUpdate() > 0;
    } catch (sql::SQLException& e) {
        cerr << "添加账户失败喵！错误码: " << e.getErrorCode() 
             << ", 信息: " << e.what() << endl;
        return false;
    }
}

bool DatabaseClient::updateAccount(const data::Account& acc) {
    lock_guard<mutex> lock(mtx);
    try {
        auto pstmt = con->prepareStatement(
            "UPDATE account SET phone=?, name=?, password=?, nickname=?, "
            "level=?, icon_data=?, type=?, balance=? WHERE uuid=?"
        );
        
        pstmt->setString(1, acc.phone);
        pstmt->setString(2, acc.name);
        pstmt->setString(3, acc.password);
        pstmt->setString(4, acc.nickname);
        pstmt->setInt(5, static_cast<int>(acc.level));
        pstmt->setString(6, acc.icon);
        pstmt->setInt(7, static_cast<int>(acc.type));
        pstmt->setDouble(8, acc.balance);
        pstmt->setString(9, acc.uuid);

        return pstmt->executeUpdate() > 0;
    } catch (sql::SQLException& e) {
        cerr << "更新账户失败喵！错误码: " << e.getErrorCode() 
             << ", 信息: " << e.what() << endl;
        return false;
    }
    return true;
}

data::Account DatabaseClient::searchAccountByID(const string& id) {
    lock_guard<mutex> lock(mtx);
    try {
        auto pstmt = con->prepareStatement(
            "SELECT * FROM account WHERE uuid=?"
        );
        pstmt->setString(1, id);
        
        auto res = unique_ptr<sql::ResultSet>(pstmt->executeQuery());
        if(res->next()) {
            data::Account acc;
            acc.uuid = res->getString("uuid");
            acc.name = res->getString("name");
            acc.password = res->getString("password");
            acc.nickname = res->getString("nickname");
            acc.icon = res->getString("icon_data");
            acc.type = static_cast<data::Account::Type>(res->getInt("type"));
            acc.balance = res->getDouble("balance");
            acc.level = static_cast<data::Account::Level>(res->getInt("level"));
            return acc;
        }
        return {}; // 返回空账户
    } catch (sql::SQLException& e) {
        cerr << "查询账户失败喵！错误码: " << e.getErrorCode() 
             << ", 信息: " << e.what() << endl;
        return {};
    }
}

data::Account DatabaseClient::searchAccountByName(const string& name) {
    lock_guard<mutex> lock(mtx);
    try {
        auto pstmt = con->prepareStatement(
            "SELECT * FROM account WHERE name=?"
        );
        pstmt->setString(1, name);
        
        auto res = unique_ptr<sql::ResultSet>(pstmt->executeQuery());
        if(res->next()) {
            data::Account acc;
            acc.uuid = res->getString("uuid");
            acc.name = res->getString("name");
            acc.password = res->getString("password");
            acc.nickname = res->getString("nickname");
            acc.icon = res->getString("icon_data");
            acc.type = static_cast<data::Account::Type>(res->getInt("type"));
            acc.balance = res->getDouble("balance");
            acc.level = static_cast<data::Account::Level>(res->getInt("level"));
            return acc;
        }
        return {};
    } catch (sql::SQLException& e) {
        cerr << "按名称查询账户失败喵！错误码: " << e.getErrorCode() 
        << ", 信息: " << e.what() << endl;
        return {};
    }
}

data::Account DatabaseClient::searchAccountByPhone(const string& phone) {
    lock_guard<mutex> lock(mtx);
    try {
        auto pstmt = con->prepareStatement(
            "SELECT * FROM account WHERE phone=?"
        );
        pstmt->setString(1, phone);
        
        auto res = unique_ptr<sql::ResultSet>(pstmt->executeQuery());
        if(res->next()) {
            data::Account acc;
            acc.uuid = res->getString("uuid");
            acc.name = res->getString("name");
            acc.password = res->getString("password");
            acc.nickname = res->getString("nickname");
            acc.icon = res->getString("icon_data");
            acc.type = static_cast<data::Account::Type>(res->getInt("type"));
            acc.balance = res->getDouble("balance");
            acc.level = static_cast<data::Account::Level>(res->getInt("level"));
            return acc;
        }
        return {}; // 返回空账户
    } catch (sql::SQLException& e) {
        cerr << "按手机号查询账户失败喵！错误码: " << e.getErrorCode() 
             << ", 信息: " << e.what() << endl;
        return {};
    }
}

bool DatabaseClient::delAccountByID(const string& id) {
    lock_guard<mutex> lock(mtx);
    try {
        auto pstmt = con->prepareStatement(
            "DELETE FROM account WHERE uuid=?"
        );
        pstmt->setString(1, id);
        return pstmt->executeUpdate() > 0;
    } catch (sql::SQLException& e) {
        cerr << "删除账户失败喵！错误码: " << e.getErrorCode() 
             << ", 信息: " << e.what() << endl;
        return false;
    }
}


//━━━━━━━━━━━━━━ Dish CRUD实现 ━━━━━━━━━━━━━━//
bool DatabaseClient::addDish(const data::Dish& dish) {
    std::lock_guard<std::mutex> lock(mtx);
    try {
        auto pstmt = con->prepareStatement(
            "INSERT INTO dishes(uuid, merchant_id, merchant_name, name, "
            "icon_path, description, base_price, price_factor, is_delete) "
            "VALUES (?,?,?,?,?,?,?,?,?)"
        );
        
        pstmt->setString(1, dish.uuid);
        pstmt->setString(2, dish.merchant_id);
        pstmt->setString(3, dish.merchant_name);
        pstmt->setString(4, dish.name);
        pstmt->setString(5, dish.icon_path);
        pstmt->setString(6, dish.description);
        pstmt->setDouble(7, dish.base_price);
        pstmt->setDouble(8, dish.price_factor);
        pstmt->setBoolean(9, dish.is_delete); // 自动转换bool为tinyint

        return pstmt->executeUpdate() > 0;
    } catch (sql::SQLException& e) {
        std::cerr << "添加菜品失败喵！(" << e.what() << ")" << std::endl;
        return false;
    }
}

bool DatabaseClient::updateDish(const data::Dish& dish) {
    std::lock_guard<std::mutex> lock(mtx);
    try {
        auto pstmt = con->prepareStatement(
            "UPDATE dishes SET merchant_id=?, merchant_name=?, name=?, "
            "icon_path=?, description=?, base_price=?, price_factor=?, "
            "is_delete=? WHERE uuid=?"
        );
        
        pstmt->setString(1, dish.merchant_id);
        pstmt->setString(2, dish.merchant_name);
        pstmt->setString(3, dish.name);
        pstmt->setString(4, dish.icon_path);
        pstmt->setString(5, dish.description);
        pstmt->setDouble(6, dish.base_price);
        pstmt->setDouble(7, dish.price_factor);
        pstmt->setBoolean(8, dish.is_delete);
        pstmt->setString(9, dish.uuid);

        return pstmt->executeUpdate() > 0;
    } catch (sql::SQLException& e) {
        std::cerr << "更新菜品失败喵！(" << e.what() << ")" << std::endl;
        return false;
    }
}

data::Dish DatabaseClient::searchDishByID(const std::string& id) {
    std::lock_guard<std::mutex> lock(mtx);
    try {
        auto pstmt = con->prepareStatement(
            "SELECT * FROM dishes WHERE uuid=?"
        );
        pstmt->setString(1, id);
        
        auto res = std::unique_ptr<sql::ResultSet>(pstmt->executeQuery());
        if(res->next()) {
            data::Dish dish;
            dish.uuid = res->getString("uuid");
            dish.merchant_id = res->getString("merchant_id");
            dish.merchant_name = res->getString("merchant_name");
            dish.name = res->getString("name");
            dish.icon_path = res->getString("icon_path");
            dish.description = res->getString("description");
            dish.base_price = res->getDouble("base_price");
            dish.price_factor = res->getDouble("price_factor");
            dish.is_delete = res->getBoolean("is_delete");
            return dish;
        }
        return {}; // 返回空菜品
    } catch (sql::SQLException& e) {
        std::cerr << "查询菜品失败喵！(" << e.what() << ")" << std::endl;
        return {};
    }
}

std::vector<data::Dish> DatabaseClient::getAllDishList() {
    std::lock_guard<std::mutex> lock(mtx);
    std::vector<data::Dish> dishes;
    try {
        auto res = std::unique_ptr<sql::ResultSet>(
            stmt->executeQuery("SELECT * FROM dishes WHERE is_delete=0")
        );
        
        while(res->next()) {
            data::Dish dish;
            dish.uuid = res->getString("uuid");
            dish.merchant_id = res->getString("merchant_id");
            dish.merchant_name = res->getString("merchant_name");
            dish.name = res->getString("name");
            dish.icon_path = res->getString("icon_path");
            dish.description = res->getString("description");
            dish.base_price = res->getDouble("base_price");
            dish.price_factor = res->getDouble("price_factor");
            dish.is_delete = res->getBoolean("is_delete");
            dishes.push_back(dish);
        }
    } catch (sql::SQLException& e) {
        std::cerr << "获取菜品列表失败喵！(" << e.what() << ")" << std::endl;
    }
    return dishes;
}

std::vector<data::Dish> DatabaseClient::getDishListByMerchant(const std::string& id) {
    std::lock_guard<std::mutex> lock(mtx);
    std::vector<data::Dish> dishes;
    try {
        auto pstmt = con->prepareStatement(
            "SELECT * FROM dishes WHERE merchant_id=? AND is_delete=0"
        );
        pstmt->setString(1, id);
        
        auto res = std::unique_ptr<sql::ResultSet>(pstmt->executeQuery());
        while(res->next()) {
            data::Dish dish;
            dish.uuid = res->getString("uuid");
            dish.merchant_id = res->getString("merchant_id");
            dish.merchant_name = res->getString("merchant_name");
            dish.name = res->getString("name");
            dish.icon_path = res->getString("icon_path");
            dish.description = res->getString("description");
            dish.base_price = res->getDouble("base_price");
            dish.price_factor = res->getDouble("price_factor");
            dish.is_delete = res->getBoolean("is_delete");
            dishes.push_back(dish);
        }
    } catch (sql::SQLException& e) {
        std::cerr << "获取商家菜品失败喵！(" << e.what() << ")" << std::endl;
    }
    return dishes;
}

bool DatabaseClient::delDishByID(const std::string& id) {
    std::lock_guard<std::mutex> lock(mtx);
    try {
        auto pstmt = con->prepareStatement(
            "UPDATE dishes SET is_delete=1 WHERE uuid=?"
        );
        pstmt->setString(1, id);
        return pstmt->executeUpdate() > 0;
    } catch (sql::SQLException& e) {
        std::cerr << "删除菜品失败喵！(" << e.what() << ")" << std::endl;
        return false;
    }
}

//━━━━━━━━━━━━━━ Order CRUD实现 ━━━━━━━━━━━━━━//
bool DatabaseClient::addOrder(const data::Order& order) {
    std::lock_guard<std::mutex> lock(mtx);
    try {
        auto pstmt = con->prepareStatement(
            "INSERT INTO orders(time, uuid, merchant_id, merchant_name, "
            "consumer_id, consumer_name, level, pay, status, sum) "
            "VALUES (?,?,?,?,?,?,?,?,?,?)"
        );
        
        pstmt->setString(1, order.time);
        pstmt->setString(2, order.uuid);
        pstmt->setString(3, order.merchant_id);
        pstmt->setString(4, order.merchant_name);
        pstmt->setString(5, order.consumer_id);
        pstmt->setString(6, order.consumer_name);
        pstmt->setInt(7, static_cast<int>(order.level));
        pstmt->setDouble(8, order.pay);
        pstmt->setInt(9, static_cast<int>(order.status));
        pstmt->setInt(10, order.sum);

        return pstmt->executeUpdate() > 0;
    } catch (sql::SQLException& e) {
        std::cerr << "添加订单失败喵！(" << e.what() << ")" << std::endl;
        return false;
    }
}

bool DatabaseClient::updateOrder(const data::Order& order) {
    std::lock_guard<std::mutex> lock(mtx);
    try {
        auto pstmt = con->prepareStatement(
            "UPDATE orders SET time=?, merchant_id=?, merchant_name=?, "
            "consumer_id=?, consumer_name=?, level=?, pay=?, status=?, sum=? "
            "WHERE uuid=?"
        );
        
        pstmt->setString(1, order.time);
        pstmt->setString(2, order.merchant_id);
        pstmt->setString(3, order.merchant_name);
        pstmt->setString(4, order.consumer_id);
        pstmt->setString(5, order.consumer_name);
        pstmt->setInt(6, static_cast<int>(order.level));
        pstmt->setDouble(7, order.pay);
        pstmt->setInt(8, static_cast<int>(order.status));
        pstmt->setInt(9, order.sum);
        pstmt->setString(10, order.uuid);

        return pstmt->executeUpdate() > 0;
    } catch (sql::SQLException& e) {
        std::cerr << "更新订单失败喵！(" << e.what() << ")" << std::endl;
        return false;
    }
}

bool DatabaseClient::updateOrder(const string& id, const data::Order::Status stat) {
    std::lock_guard<std::mutex> lock(mtx);
    try {
        auto pstmt = con->prepareStatement(
            "UPDATE orders SET status=? WHERE uuid=?"
        );
        pstmt->setInt(1, static_cast<int>(stat));
        pstmt->setString(2, id);
        return pstmt->executeUpdate() > 0;
    } catch (sql::SQLException& e) {
        std::cerr << "更新订单状态失败喵！(" << e.what() << ")" << std::endl;
        return false;
    }
}

data::Order DatabaseClient::searchOrderByID(const string& order_id) {
    std::lock_guard<std::mutex> lock(mtx);
    try {
        auto pstmt = con->prepareStatement(
            "SELECT * FROM orders WHERE uuid=?"
        );
        pstmt->setString(1, order_id);
        
        auto res = std::unique_ptr<sql::ResultSet>(pstmt->executeQuery());
        if(res->next()) {
            data::Order order;
            order.time = res->getString("time");
            order.uuid = res->getString("uuid");
            order.merchant_id = res->getString("merchant_id");
            order.merchant_name = res->getString("merchant_name");
            order.consumer_id = res->getString("consumer_id");
            order.consumer_name = res->getString("consumer_name");
            order.level = static_cast<data::Account::Level>(res->getInt("level"));
            order.pay = res->getDouble("pay");
            order.status = static_cast<data::Order::Status>(res->getInt("status"));
            order.sum = res->getInt("sum");
            return order;
        }
        return {};
    } catch (sql::SQLException& e) {
        std::cerr << "查询订单失败喵！(" << e.what() << ")" << std::endl;
        return {};
    }
}

vector<data::Order> DatabaseClient::getOrderListByMerchant(const string& merchant_id) {
    std::lock_guard<std::mutex> lock(mtx);
    vector<data::Order> orders;
    try {
        auto pstmt = con->prepareStatement(
            "SELECT * FROM orders WHERE merchant_id=?"
        );
        pstmt->setString(1, merchant_id);
        
        auto res = std::unique_ptr<sql::ResultSet>(pstmt->executeQuery());
        while(res->next()) {
            data::Order order;
            order.time = res->getString("time");
            order.uuid = res->getString("uuid");
            order.merchant_id = res->getString("merchant_id");
            order.merchant_name = res->getString("merchant_name");
            order.consumer_id = res->getString("consumer_id");
            order.consumer_name = res->getString("consumer_name");
            order.level = static_cast<data::Account::Level>(res->getInt("level"));
            order.pay = res->getDouble("pay");
            order.status = static_cast<data::Order::Status>(res->getInt("status"));
            order.sum = res->getInt("sum");
            orders.push_back(order);
        }
    } catch (sql::SQLException& e) {
        std::cerr << "获取商家订单失败喵！(" << e.what() << ")" << std::endl;
    }
    return orders;
}

vector<data::Order> DatabaseClient::getOrderListByMerchantWaiting(const string& merchant_id) {
    std::lock_guard<std::mutex> lock(mtx);
    vector<data::Order> orders;
    try {
        auto pstmt = con->prepareStatement(
            "SELECT * FROM orders WHERE merchant_id=? AND status=?"
        );
        pstmt->setString(1, merchant_id);
        pstmt->setInt(2, static_cast<int>(data::Order::WAITING));
        
        auto res = std::unique_ptr<sql::ResultSet>(pstmt->executeQuery());
        while(res->next()) {
            data::Order order;
            // 赋值代码同上...
            orders.push_back(order);
        }
    } catch (sql::SQLException& e) {
        std::cerr << "获取待处理订单失败喵！(" << e.what() << ")" << std::endl;
    }
    return orders;
}

vector<data::Order> DatabaseClient::getOrderListByConsumer(const string& consumer_id) {
    std::lock_guard<std::mutex> lock(mtx);
    vector<data::Order> orders;
    try {
        auto pstmt = con->prepareStatement(
            "SELECT * FROM orders WHERE consumer_id=?"
        );
        pstmt->setString(1, consumer_id);
        
        auto res = std::unique_ptr<sql::ResultSet>(pstmt->executeQuery());
        while(res->next()) {
            data::Order order;
            // 赋值代码同上...
            orders.push_back(order);
        }
    } catch (sql::SQLException& e) {
        std::cerr << "获取用户订单失败喵！(" << e.what() << ")" << std::endl;
    }
    return orders;
}

vector<data::Order> DatabaseClient::getAllOrderList() {
    std::lock_guard<std::mutex> lock(mtx);
    vector<data::Order> orders;
    try {
        auto res = std::unique_ptr<sql::ResultSet>(
            stmt->executeQuery("SELECT * FROM orders")
        );
        while(res->next()) {
            data::Order order;
            // 赋值代码同上...
            orders.push_back(order);
        }
    } catch (sql::SQLException& e) {
        std::cerr << "获取全部订单失败喵！(" << e.what() << ")" << std::endl;
    }
    return orders;
}

bool DatabaseClient::delOrderByID(const string& order_id) {
    std::lock_guard<std::mutex> lock(mtx);
    try {
        auto pstmt = con->prepareStatement(
            "DELETE FROM orders WHERE uuid=?"
        );
        pstmt->setString(1, order_id);
        return pstmt->executeUpdate() > 0;
    } catch (sql::SQLException& e) {
        std::cerr << "删除订单失败喵！(" << e.what() << ")" << std::endl;
        return false;
    }
}

int DatabaseClient::clearOverTimeOrder() {
    std::lock_guard<std::mutex> lock(mtx);
    try {
        // 将超时订单转移到历史表
        con->setAutoCommit(false);
        
        // 1. 复制到history表
        auto copyStmt = con->prepareStatement(
            "INSERT INTO history "
            "SELECT * FROM orders WHERE status=? AND time < NOW() - INTERVAL 30 MINUTE"
        );
        copyStmt->setInt(1, static_cast<int>(data::Order::WAITING));
        int affected = copyStmt->executeUpdate();
        
        // 2. 删除原订单
        auto delStmt = con->prepareStatement(
            "DELETE FROM orders WHERE status=? AND time < NOW() - INTERVAL 30 MINUTE"
        );
        delStmt->setInt(1, static_cast<int>(data::Order::WAITING));
        delStmt->executeUpdate();
        
        con->commit();
        con->setAutoCommit(true);
        return affected;
    } catch (sql::SQLException& e) {
        con->rollback();
        std::cerr << "清理超时订单失败喵！(" << e.what() << ")" << std::endl;
        return -1;
    }
}


//━━━━━━━━━━━━━━ OrderDish 相关操作 ━━━━━━━━━━━━━━//
bool DatabaseClient::addOrderDishesByID(const string& order_id, const vector<data::OrderDish>& list) {
    std::lock_guard<std::mutex> lock(mtx);
    try {
        con->setAutoCommit(false); // 启动量子纠缠态（事务）

        // 创建时空裂缝（预处理语句）
        auto pstmt = con->prepareStatement(
            "INSERT INTO orderDish(order_id, dish_id, merchant_id, name, dish_price, count) "
            "VALUES (?,?,?,?,?,?)"
        );

        vector<int> updateCounts; // 用来收集影响次元数

        for(const auto& dish : list) {
            // 设置量子参数
            pstmt->setString(1, order_id);
            pstmt->setString(2, dish.dish_id);
            pstmt->setString(3, dish.merchant_id);
            pstmt->setString(4, dish.name);
            pstmt->setDouble(5, dish.dish_price);
            pstmt->setInt(6, dish.count);

            // 执行单次时空跃迁（逐条执行）
            int count = pstmt->executeUpdate();
            updateCounts.push_back(count);
            
            // 重置时空裂缝准备下次跃迁
            pstmt->clearParameters();
        }

        con->commit(); // 确认所有次元跃迁
        con->setAutoCommit(true); // 恢复时空连续性

        // 验证所有次元跃迁结果
        for(auto count : updateCounts) {
            if(count <= 0) {
                std::cerr << "检测到量子态坍缩！部分插入失败喵～" << std::endl;
                return false;
            }
        }
        return true;

    } catch (sql::SQLException& e) {
        con->rollback(); // 启动时空回溯协议
        std::cerr << "次元跃迁异常喵！(错误代码:" << e.getErrorCode() 
                 << " 信息:" << e.what() << ")" << std::endl;
        return false;
    }
}

bool DatabaseClient::delOrderDishesByID(const string& order_id) {
    std::lock_guard<std::mutex> lock(mtx);
    try {
        auto pstmt = con->prepareStatement(
            "DELETE FROM orderDish WHERE order_id=?"
        );
        pstmt->setString(1, order_id);
        return pstmt->executeUpdate() > 0;
    } catch (sql::SQLException& e) {
        std::cerr << "删除订单菜品失败喵！(" << e.what() << ")" << std::endl;
        return false;
    }
}

vector<data::OrderDish> DatabaseClient::getOrderDishesByID(const string& order_id) {
    std::lock_guard<std::mutex> lock(mtx);
    vector<data::OrderDish> dishes;
    try {
        auto pstmt = con->prepareStatement(
            "SELECT * FROM orderDish WHERE order_id=?"
        );
        pstmt->setString(1, order_id);
        
        auto res = std::unique_ptr<sql::ResultSet>(pstmt->executeQuery());
        while(res->next()) {
            data::OrderDish dish;
            dish.order_id = res->getString("order_id");
            dish.dish_id = res->getString("dish_id");
            dish.merchant_id = res->getString("merchant_id");
            dish.name = res->getString("name");
            dish.dish_price = res->getDouble("dish_price");
            dish.count = res->getInt("count");
            dishes.push_back(dish);
        }
    } catch (sql::SQLException& e) {
        std::cerr << "查询订单菜品失败喵！(" << e.what() << ")" << std::endl;
    }
    return dishes;
}

//━━━━━━━━━━━━━━━━━ HistoryDish 相关操作 ━━━━━━━━━━━━━━━━//

// 原子化历史记录添加（事务保护）
bool DatabaseClient::addHistory(const data::Order& order) {
    std::lock_guard<std::mutex> lock(mtx);
    try {
        con->setAutoCommit(false); // 启动时间线锁定
        
        // 创建时空裂缝（预处理语句）
        sql::PreparedStatement* pstmt;
        pstmt = con->prepareStatement(
            "INSERT INTO history(time, uuid, merchant_id, merchant_name, consumer_id, "
            "consumer_name, level, pay, status, sum) VALUES (?,?,?,?,?,?,?,?,?,?)"
        );

        // 注入历史记录参数
        pstmt->setString(1, order.time);
        pstmt->setString(2, order.uuid);
        pstmt->setString(3, order.merchant_id);
        pstmt->setString(4, order.merchant_name);
        pstmt->setString(5, order.consumer_id);
        pstmt->setString(6, order.consumer_name);
        pstmt->setInt(7, static_cast<int>(order.level));
        pstmt->setDouble(8, order.pay);
        pstmt->setInt(9, static_cast<int>(order.status));
        pstmt->setInt(10, order.sum);

        // 执行时空刻印
        int affectedRows = pstmt->executeUpdate();
        con->commit(); // 确认历史记录
        
        // 清理时空残影
        delete pstmt;
        con->setAutoCommit(true);
        
        return affectedRows > 0;

    } catch (sql::SQLException& e) {
        con->rollback(); // 时间线回溯
        std::cerr << "历史记录刻印失败喵！(错误码:" << e.getErrorCode() 
                 << " 信息:" << e.what() << ")" << std::endl;
        return false;
    }
}

// 全量历史记录读取（量子纠缠态解析）
vector<data::Order> DatabaseClient::getAllHistoryList() {
    std::lock_guard<std::mutex> lock(mtx);
    vector<data::Order> historyList;
    
    try {
        sql::Statement* stmt;
        stmt = con->createStatement();
        
        sql::ResultSet* res;
        res = stmt->executeQuery("SELECT * FROM history");

        // 解析多重历史时间线
        while (res->next()) {
            data::Order order;
            
            // 基础字段映射
            order.time = res->getString("time");
            order.uuid = res->getString("uuid");
            order.merchant_id = res->getString("merchant_id");
            order.merchant_name = res->getString("merchant_name");
            order.consumer_id = res->getString("consumer_id");
            order.consumer_name = res->getString("consumer_name");
            
            // 枚举类型转换
            order.level = static_cast<data::Account::Level>(
                res->getInt("level"));
            order.status = static_cast<data::Order::Status>(
                res->getInt("status"));
            
            // 数值类型
            order.pay = res->getDouble("pay");
            order.sum = res->getInt("sum");

            historyList.push_back(order);
        }

        // 清理量子残影
        delete res;
        delete stmt;
        
    } catch (sql::SQLException& e) {
        std::cerr << "历史时间线读取异常喵！(" << e.what() << ")" << std::endl;
    }
    
    return historyList;
}

//━━━━━━━━━━━━━━━━━ HistoryDish 相关操作 ━━━━━━━━━━━━━━━━━━//

bool DatabaseClient::addHistoryDishesByID(const string& order_id, 
                                        const vector<data::OrderDish>& list) {
    std::lock_guard<std::mutex> lock(mtx);
    try {
        con->setAutoCommit(false); // 启动时空冻结
        
        // 创建时间胶囊（预处理语句）
        sql::PreparedStatement* pstmt;
        pstmt = con->prepareStatement(
            "INSERT INTO historyDish(order_id, dish_id, merchant_id, name, dish_price, count) "
            "VALUES (?,?,?,?,?,?)"
        );

        // 遍历时间碎片（逐条插入）
        for(const auto& dish : list) {
            pstmt->setString(1, order_id);
            pstmt->setString(2, dish.dish_id);
            pstmt->setString(3, dish.merchant_id);
            pstmt->setString(4, dish.name);
            pstmt->setDouble(5, dish.dish_price);
            pstmt->setInt(6, dish.count);

            // 执行时空刻印
            int affected = pstmt->executeUpdate();
            if(affected <= 0) {
                con->rollback(); // 时间线回溯
                delete pstmt;
                return false;
            }
            pstmt->clearParameters(); // 清理时空残影
        }

        con->commit(); // 确认历史轨迹
        delete pstmt;
        return true;

    } catch (sql::SQLException& e) {
        con->rollback();
        std::cerr << "历史菜品刻录失败喵！(错误码:" << e.getErrorCode() 
                 << " 信息:" << e.what() << ")" << std::endl;
        return false;
    }
}

vector<data::OrderDish> DatabaseClient::getHistoryDishesByID(const string& order_id) {
    std::lock_guard<std::mutex> lock(mtx);
    vector<data::OrderDish> dishes;

    try {
        sql::PreparedStatement* pstmt;
        pstmt = con->prepareStatement(
            "SELECT * FROM historyDish WHERE order_id = ?"
        );
        pstmt->setString(1, order_id);

        sql::ResultSet* res = pstmt->executeQuery();

        // 解析时空碎片
        while (res->next()) {
            data::OrderDish dish;
            
            // 基础字段映射
            dish.order_id = res->getString("order_id");
            dish.dish_id = res->getString("dish_id");
            dish.merchant_id = res->getString("merchant_id");
            dish.name = res->getString("name");
            
            // 数值类型转换
            dish.dish_price = res->getDouble("dish_price");
            dish.count = res->getInt("count");

            dishes.push_back(dish);
        }

        // 清理量子泡沫
        delete res;
        delete pstmt;
        
    } catch (sql::SQLException& e) {
        std::cerr << "历史菜品读取异常喵！(" << e.what() << ")" << std::endl;
    }
    
    return dishes;
}