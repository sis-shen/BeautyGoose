#include "HTTPServer.hpp"

using namespace btyGoose;

HTTPServer* HTTPServer::_ins = nullptr;

using std::cout;
btyGoose::HTTPServer::HTTPServer()
{
    loadConfig();
    initTestAPI();
    initAccountAPI();
    initConsumerAPI();
    initMerchantAPI();
    initAdminAPI();
    cout<<"comlete init!\n";
}

void btyGoose::HTTPServer::start()
{
    cout <<svr.listen(host, port);
}

bool btyGoose::HTTPServer::loadConfig()
{
    std::ifstream file("serverConfig.json", std::ios::in);
    if (file.is_open()) {
        // 读取文件内容到字符串
        std::string data((std::istreambuf_iterator<char>(file)), 
                        std::istreambuf_iterator<char>());
        
        Json::Value root;
        Json::Reader reader;
        
        if (reader.parse(data, root)) {
            host = root["host"].asString();
            port = root["port"].asInt();
            return true;
        }
    }
    // 反序列化失败时生成默认配置
    saveConfig();
    return false;
}

void btyGoose::HTTPServer::saveConfig()
{
    std::ofstream file("serverConfig.json", std::ios::out);
    if (file.is_open()) {
        Json::Value root;
        root["host"] = host;
        root["port"] = port;

        Json::StreamWriterBuilder writer;
        file << Json::writeString(writer, root);
        file.close();
    } else {
        std::cerr << "无法打开服务器配置文件" << std::endl;
    }
}

void btyGoose::HTTPServer::initTestAPI()
{
    svr.Get("/ping", [=](const httplib::Request& req, httplib::Response& res) {
        cout << "/ping";
        res.body = "pong";
        res.set_header("Content-Type", "application/text;charset=UTF-8");
        });
}

void btyGoose::HTTPServer::initAccountAPI()
{
    // 用户名登录
    svr.Post("/account/login/username", [this](const httplib::Request& req, httplib::Response& res) {
        std::cout << "[INFO] /account/login/username received POST request\n";
        
        Json::Value reqJson, resJson;
        Json::Reader reader;
        
        // 解析JSON请求体
        if (!reader.parse(req.body, reqJson)) {
            std::cerr << "[ERROR] 无效的JSON格式: " << req.body << std::endl;
            resJson["success"] = false;
            resJson["message"] = "无效的JSON格式";
            res.body = Json::FastWriter().write(resJson);
            res.set_header("Content-Type", "application/json;charset=UTF-8");
            return;
        }

        try {
            data::Account record_acc = db.searchAccountByName(reqJson["name"].asString());
            
            if (!record_acc.name.empty() && reqJson["password"].asString() == record_acc.password) {
                // 登录成功响应
                resJson["success"] = true;
                resJson["message"] = "登录认证成功";
                resJson["account"] = json::toJson(record_acc); // 假设已实现toJson方法
            } else {
                // 登录失败响应
                resJson["success"] = false;
                resJson["message"] = "登录认证失败";
            }
            
            res.body = Json::FastWriter().write(resJson);
        }
        catch (const HTTPException& e) {
            std::cerr << "[ERROR] " << e.what() << std::endl;
            res.status = 500;
            resJson["success"] = false;
            resJson["message"] = e.what();
            res.body = Json::FastWriter().write(resJson);
        }
        
        std::cout << "[INFO] 用户名登录处理完成\n";
        res.set_header("Content-Type", "application/json;charset=UTF-8");
    });

    // 手机号登录
    svr.Post("/account/login/phone", [this](const httplib::Request& req, httplib::Response& res) {
        std::cout << "[INFO] /account/login/phone received POST request\n";
        
        Json::Value reqJson, resJson;
        Json::Reader reader;
        
        if (!reader.parse(req.body, reqJson)) {
            std::cerr << "[ERROR] 无效的JSON格式: " << req.body << std::endl;
            resJson["success"] = false;
            resJson["message"] = "无效的JSON格式";
            res.body = Json::FastWriter().write(resJson);
            res.set_header("Content-Type", "application/json;charset=UTF-8");
            return;
        }

        try {
            data::Account record_acc = db.searchAccountByPhone(reqJson["phone"].asString());
            
            if (!record_acc.phone.empty() && reqJson["password"].asString() == record_acc.password) {
                resJson["success"] = true;
                resJson["message"] = "手机登录成功";
                resJson["account"] = json::toJson(record_acc);
            } else {
                resJson["success"] = false;
                resJson["message"] = "手机认证失败";
            }
            
            res.body = Json::FastWriter().write(resJson);
        }
        catch (const HTTPException& e) {
            std::cerr << "[ERROR] " << e.what() << std::endl;
            res.status = 500;
            resJson["success"] = false;
            resJson["message"] = e.what();
            res.body = Json::FastWriter().write(resJson);
        }
        
        res.set_header("Content-Type", "application/json;charset=UTF-8");
    });

    // 账户注册
    svr.Post("/account/register", [this](const httplib::Request& req, httplib::Response& res) {
        std::cout << "[INFO] 开始处理用户注册\n";
        
        Json::Value reqJson, resJson;
        Json::Reader reader;
        res.set_header("Content-Type", "application/json;charset=UTF-8");

        if (!reader.parse(req.body, reqJson)) {
            std::cerr << "[ERROR] 注册请求解析失败: " << req.body << std::endl;
            resJson["success"] = false;
            resJson["message"] = "无效的JSON格式";
            res.body = Json::FastWriter().write(resJson);
            return;
        }

        try {
            data::Account acc;
            acc.name = reqJson["name"].asString();
            acc.password = reqJson["password"].asString();
            acc.phone = reqJson["phone"].asString();
            acc.nickname = reqJson["nickname"].asString();
            acc.type = static_cast<data::Account::Type>(reqJson["type"].asInt());
            acc.level = data::Account::Level::MEMBER;
            acc.uuid = util::makeId('A');

            // 验证UUID生成
            if (acc.uuid.empty()) {
                std::cerr << "[WARN] UUID生成失败\n";
                resJson["message"] = "UUID生成错误";
                resJson["success"] = false;
                res.body = Json::FastWriter().write(resJson);
                return;
            }

            // 验证码校验
            if (!AuthenticateAuthCode(acc.phone, reqJson["auth_code"].asString())) {
                std::cout << "[WARN] 验证码错误: " << acc.phone << std::endl;
                resJson["message"] = "验证码错误";
                resJson["success"] = false;
                res.body = Json::FastWriter().write(resJson);
                return;
            }

            // 用户名查重
            if (!db.searchAccountByName(acc.name).name.empty()) {
                std::cout << "[WARN] 用户名已存在: " << acc.name << std::endl;
                resJson["message"] = "用户名已存在";
                resJson["success"] = false;
                res.body = Json::FastWriter().write(resJson);
                return;
            }

            // 手机号查重
            if (!db.searchAccountByPhone(acc.phone).name.empty()) {
                std::cout << "[WARN] 手机号重复: " << acc.phone << std::endl;
                resJson["message"] = "手机号已被占用";
                resJson["success"] = false;
                res.body = Json::FastWriter().write(resJson);
                return;
            }

            // 创建账户
            if (!db.addAccount(acc)) {
                throw HTTPException("数据库写入失败");
            }

            std::cout << "[SUCCESS] 新用户注册成功: " << acc.name << std::endl;
            resJson["success"] = true;
            resJson["message"] = "注册成功";
            res.body = Json::FastWriter().write(resJson);
        }
        catch (const HTTPException& e) {
            std::cerr << "[ERROR] 注册异常: " << e.what() << std::endl;
            res.status = 500;
            resJson["success"] = false;
            resJson["message"] = e.what();
            res.body = Json::FastWriter().write(resJson);
        }
    });

    // 等级更新接口
    svr.Post("/account/update/level", [this](const httplib::Request& req, httplib::Response& res) {
        std::cout << "[INFO] 处理等级更新请求\n";
        
        Json::Value reqJson, resJson;
        Json::Reader reader;
        
        if (!reader.parse(req.body, reqJson)) {
            std::cerr << "[ERROR] 等级更新请求解析失败\n";
            resJson["success"] = false;
            resJson["message"] = "无效的JSON格式";
            res.body = Json::FastWriter().write(resJson);
            return;
        }

        try {
            data::Account acc = db.searchAccountByID(reqJson["id"].asString());
            std::string level = reqJson["level"].asString();
            
            std::cout << "正在更新用户等级: " << acc.name 
                     << " -> " << level << std::endl;

            if (level == "VIP") {
                acc.level = data::Account::Level::VIP;
            } else if (level == "SUVIP") {
                acc.level = data::Account::Level::SUVIP;
            } else {
                throw HTTPException("未知的等级类型: " + level);
            }

            db.updateAccount(acc);
            resJson["level"] = static_cast<int>(acc.level);
            res.body = Json::FastWriter().write(resJson);
        }
        catch (const HTTPException& e) {
            std::cerr << "[ERROR] 等级更新失败: " << e.what() << std::endl;
            res.status = 500;
            resJson["success"] = false;
            resJson["message"] = e.what();
            res.body = Json::FastWriter().write(resJson);
        }
        
        res.set_header("Content-Type", "application/json");
    });

    svr.Post("/account/update/nickname", [this](const httplib::Request& req, httplib::Response& res) {
        std::cout << "[INFO] 收到昵称更新请求\n";
        
        Json::Value reqJson, resJson;
        Json::Reader reader;
        res.set_header("Content-Type", "application/json");
    
        // 解析JSON请求
        if (!reader.parse(req.body, reqJson)) {
            std::cerr << "[ERROR] 无效的JSON格式: " << req.body << "\n";
            resJson["success"] = false;
            resJson["message"] = "无效的请求格式";
            res.body = Json::FastWriter().write(resJson);
            return;
        }
    
        try {
            // 参数校验
            if (!reqJson.isMember("id") || !reqJson.isMember("nickname")) {
                throw HTTPException("缺少必要参数");
            }
    
            // 获取用户信息
            std::string userId = reqJson["id"].asString();
            std::string newNickname = reqJson["nickname"].asString();
            
            std::cout << "[DEBUG] 正在更新用户昵称: " << userId 
                     << " -> " << newNickname << "\n";
    
            // 查询并更新账户
            data::Account acc = db.searchAccountByID(userId);
            acc.nickname = newNickname;
            
            if (!db.updateAccount(acc)) {
                throw HTTPException("数据库更新失败");
            }
    
            // 验证更新结果
            data::Account updatedAcc = db.searchAccountByID(userId);
            resJson["nickname"] = updatedAcc.nickname;
            resJson["success"] = true;
            res.body = Json::FastWriter().write(resJson);
        }
        catch (const HTTPException& e) {
            std::cerr << "[ERROR] 昵称更新失败: " << e.what() << "\n";
            res.status = 500;
            resJson["success"] = false;
            resJson["message"] = e.what();
            res.body = Json::FastWriter().write(resJson);
        }
    
        std::cout << "[INFO] 昵称更新处理完成\n";
    });
}

bool btyGoose::HTTPServer::AuthenticateAuthCode(const string& phone, const string& auth_code)
{
    ///////////////////////////
    // TODO
    // ///////////////////
    //这里就不具体实现了
    if (auth_code == "888888")
    {
        return true;
    }
    else
    {
        return false;
    }
}


void btyGoose::HTTPServer::initConsumerAPI()
{
    svr.Post("/consumer/order/detail/dishlist", [this](const httplib::Request& req, httplib::Response& res) {
        std::cout << "/consumer/order/detail/dishlist get a post!\n";
        std::string jsonStr = req.body;
        Json::Value jsonObj;
        Json::Reader reader;
        if (!reader.parse(jsonStr, jsonObj)) {
            std::cerr << "Invalid Json: " << jsonStr << "\n";
            jsonObj = Json::objectValue;
        }
        Json::Value resJson;
        try {
            if (jsonObj.empty()) {
                throw HTTPException("Json Serialization failed");
            }

            std::string order_id = jsonObj["order_id"].asString();
            std::vector<data::OrderDish> dishList = db.getOrderDishesByID(order_id);
            std::cout << "order:" << order_id << " find dishes " << dishList.size() << "\n";
            std::string dishListJson = OrderDishListToJsonArray(dishList);
            resJson["dish_list"] = dishListJson;
            Json::StreamWriterBuilder writer;
            res.body = Json::writeString(writer, resJson);
            res.set_header("Content-Type", "application/json;charset=UTF-8");
        }
        catch (const HTTPException& e) {
            res.status = 500;
            resJson["success"] = false;
            resJson["message"] = e.what();
            Json::StreamWriterBuilder writer;
            res.body = Json::writeString(writer, resJson);
            res.set_header("Content-Type", "application/json;charset=UTF-8");
            std::cerr << e.what() << "\n";
        }
    });

    svr.Post("/consumer/dish/list", [this](const httplib::Request& req, httplib::Response& res) {
        std::cout << "/consumer/dish/list get a post!\n";
        std::vector<data::Dish> dishList = db.getAllDishList();
        std::string dishListJson = DishListToJsonArray(dishList);
        Json::Value resJson;
        resJson["dish_list"] = dishListJson;
        Json::StreamWriterBuilder writer;
        res.body = Json::writeString(writer, resJson);
        res.set_header("Content-Type", "application/json;charset=UTF-8");
    });

    svr.Post("/consumer/dish/dishInfo", [this](const httplib::Request& req, httplib::Response& res) {
        std::cout << "/consumer/dish/dishInfo get a post!\n";
        std::string jsonStr = req.body;
        Json::Value jsonObj;
        Json::Reader reader;
        if (!reader.parse(jsonStr, jsonObj)) {
            std::cerr << "Invalid Json: " << jsonStr << "\n";
            jsonObj = Json::objectValue;
        }
        Json::Value resJson;
        try {
            if (jsonObj.empty()) {
                throw HTTPException("Json Serialization failed");
            }

            std::string dish_id = jsonObj["dish_id"].asString();
            data::Dish dish = db.searchDishByID(dish_id);
            resJson["dish"] = json::toJson(dish);
            Json::StreamWriterBuilder writer;
            res.body = Json::writeString(writer, resJson);
            res.set_header("Content-Type", "application/json;charset=UTF-8");
        }
        catch (const HTTPException& e) {
            res.status = 500;
            resJson["success"] = false;
            resJson["message"] = e.what();
            Json::StreamWriterBuilder writer;
            res.body = Json::writeString(writer, resJson);
            res.set_header("Content-Type", "application/json;charset=UTF-8");
            std::cerr << e.what() << "\n";
        }
    });

    // 订单生成
    svr.Post("/consumer/order/generate", [this](const httplib::Request& req, httplib::Response& res) {
        std::cout << "/consumer/order/generate get a post!\n";
        std::string jsonStr = req.body;
        Json::Value jsonObj;
        Json::Reader reader;
        if (!reader.parse(jsonStr, jsonObj)) {
            std::cerr << "Invalid Json: " << jsonStr << "\n";
            jsonObj = Json::objectValue;
        }
        Json::Value resJson;
        try {
            if (jsonObj.empty()) {
                throw HTTPException("Json Serialization failed");
            }

            data::Order order;
            order.uuid = util::makeId('O');
            order.merchant_id = jsonObj["merchant_id"].asString();
            order.merchant_name = jsonObj["merchant_name"].asString();
            order.consumer_id = jsonObj["consumer_id"].asString();
            order.consumer_name = jsonObj["consumer_name"].asString();
            order.level = static_cast<data::Account::Level>(jsonObj["level"].asInt());
            order.pay = jsonObj["pay"].asDouble();
            order.sum = jsonObj["count"].asInt();
            order.time = std::to_string(util::getSecTime());
            order.status = data::Order::Status::UNPAYED;

            std::cout << "生成一个order, merchant id:" << order.merchant_id << "\n";

            bool ok = db.addOrder(order);
            assert(ok);
            std::vector<data::OrderDish> dish_list;
            Json::Value jsonArr = jsonObj["dish_arr"];
            for (const auto& value : jsonArr) {
                std::cout << "生成一个OrderDish\n";
                data::OrderDish dish;
                dish.order_id = order.uuid;
                dish.merchant_id = order.merchant_id;
                dish.name = value["dish_name"].asString();
                dish.dish_price = value["dish_price"].asDouble();
                dish.count = value["count"].asInt();
                dish.dish_id = value["dish_id"].asString();
                dish_list.push_back(dish);
            }
            ok = db.addOrderDishesByID(order.uuid, dish_list);
            assert(ok);

            resJson["order"] = json::toJson(order);
            Json::StreamWriterBuilder writer;
            res.body = Json::writeString(writer, resJson);
            res.set_header("Content-Type", "application/json;charset=UTF-8");
        }
        catch (const HTTPException& e) {
            res.status = 500;
            resJson["success"] = false;
            resJson["message"] = e.what();
            Json::StreamWriterBuilder writer;
            res.body = Json::writeString(writer, resJson);
            res.set_header("Content-Type", "application/json;charset=UTF-8");
            std::cerr << e.what() << "\n";
        }
    });

    svr.Post("/consumer/order/list", [this](const httplib::Request& req, httplib::Response& res) {
        std::cout << "/consumer/order/list get a post!\n";
        std::string jsonStr = req.body;
        Json::Value jsonObj;
        Json::Reader reader;
        if (!reader.parse(jsonStr, jsonObj)) {
            std::cerr << "Invalid Json: " << jsonStr << "\n";
            jsonObj = Json::objectValue;
        }
        Json::Value resJson;
        try {
            if (jsonObj.empty()) {
                throw HTTPException("Json Serialization failed");
            }

            std::string consumer_id = jsonObj["consumer_id"].asString();
            std::vector<data::Order> orderList = db.getOrderListByConsumer(consumer_id);
            std::cout << "consumer:" << consumer_id << " find orders " << orderList.size() << "\n";
            std::string dishListJson = OrderListToJsonArray(orderList);
            resJson["order_list"] = dishListJson;
            Json::StreamWriterBuilder writer;
            res.body = Json::writeString(writer, resJson);
            res.set_header("Content-Type", "application/json;charset=UTF-8");
        }
        catch (const HTTPException& e) {
            res.status = 500;
            resJson["success"] = false;
            resJson["message"] = e.what();
            Json::StreamWriterBuilder writer;
            res.body = Json::writeString(writer, resJson);
            res.set_header("Content-Type", "application/json;charset=UTF-8");
            std::cerr << e.what() << "\n";
        }
    });

    svr.Post("/consumer/order/pay/confirm", [this](const httplib::Request& req, httplib::Response& res) {
        std::cout << "/consumer/order/pay/confirm get a post!\n";
        std::string jsonStr = req.body;
        Json::Value jsonObj;
        Json::Reader reader;
        if (!reader.parse(jsonStr, jsonObj)) {
            std::cerr << "Invalid Json: " << jsonStr << "\n";
            jsonObj = Json::objectValue;
        }
        Json::Value resJson;
        try {
            if (jsonObj.empty()) {
                throw HTTPException("Json Serialization failed");
            }
            data::Order order = db.searchOrderByID(jsonObj["order_id"].asString());
            order.status = data::Order::Status::WAITING;
            db.updateOrder(order);
            Json::StreamWriterBuilder writer;
            res.body = Json::writeString(writer, resJson);
            res.set_header("Content-Type", "application/json");
        }
        catch (const HTTPException& e) {
            res.status = 500;
            resJson["success"] = false;
            resJson["message"] = e.what();
            Json::StreamWriterBuilder writer;
            res.body = Json::writeString(writer, resJson);
            res.set_header("Content-Type", "application/json");
            std::cerr << e.what() << "\n";
        }
    });

    svr.Post("/consumer/order/cancel", [this](const httplib::Request& req, httplib::Response& res) {
        std::cout << "/consumer/order/cancel get a post!\n";
        std::string jsonStr = req.body;
        Json::Value jsonObj;
        Json::Reader reader;
        if (!reader.parse(jsonStr, jsonObj)) {
            std::cerr << "Invalid Json: " << jsonStr << "\n";
            jsonObj = Json::objectValue;
        }
        Json::Value resJson;
        try {
            if (jsonObj.empty()) {
                throw HTTPException("Json Serialization failed");
            }

            data::Order order = db.searchOrderByID(jsonObj["order_id"].asString());
            std::string reason;
            if (order.status == data::Order::Status::UNPAYED) {
                order.status = data::Order::Status::CANCELED;
                db.updateOrder(order);
                reason = "订单取消成功";
            } else if (order.status == data::Order::Status::WAITING) {
                reason = "订单无法取消：已支付";
            } else {
                reason = "订单无法支付,订单状态码:" + std::to_string(static_cast<int>(order.status));
            }

            resJson["reason"] = reason;
            Json::StreamWriterBuilder writer;
            res.body = Json::writeString(writer, resJson);
            res.set_header("Content-Type", "application/json;charset=UTF-8");
        }
        catch (const HTTPException& e) {
            res.status = 500;
            resJson["success"] = false;
            resJson["message"] = e.what();
            Json::StreamWriterBuilder writer;
            res.body = Json::writeString(writer, resJson);
            res.set_header("Content-Type", "application/json;charset=UTF-8");
            std::cerr << e.what() << "\n";
        }
    });
}
void btyGoose::HTTPServer::initMerchantAPI()
{
    svr.Post("/merchant/dish/list", [this](const httplib::Request& req, httplib::Response& res) {
        std::cout << "/merchant/dish/list get a post!\n";
        std::string jsonStr = req.body;
        Json::Value jsonObj;
        Json::Reader reader;
        if (!reader.parse(jsonStr, jsonObj)) {
            std::cerr << "Invalid Json: " << jsonStr << "\n";
            jsonObj = Json::objectValue;
        }
        Json::Value resJson;
        try {
            if (jsonObj.empty()) {
                throw HTTPException("Json Serialization failed");
            }

            std::string merchant_id = jsonObj["merchant_id"].asString();
            std::vector<data::Dish> dishList = db.getDishListByMerchant(merchant_id);
            std::cout << "merchant:" << merchant_id << " find dishes " << dishList.size() << "\n";
            std::string dishListJson = DishListToJsonArray(dishList);
            resJson["dish_list"] = dishListJson;
            Json::StreamWriterBuilder writer;
            res.body = Json::writeString(writer, resJson);
            res.set_header("Content-Type", "application/json;charset=UTF-8");
        }
        catch (const HTTPException& e) {
            res.status = 500;
            resJson["success"] = false;
            resJson["message"] = e.what();
            Json::StreamWriterBuilder writer;
            res.body = Json::writeString(writer, resJson);
            res.set_header("Content-Type", "application/json;charset=UTF-8");
            std::cerr << e.what() << "\n";
        }
    });

    // 菜品注册
    svr.Post("/merchant/dish/register", [this](const httplib::Request& req, httplib::Response& res) {
        std::cout << "/merchant/dish/register get a post!\n";
        std::cout << "菜品开始注册\n";
        std::string jsonStr = req.body;
        Json::Value jsonObj;
        Json::Reader reader;
        if (!reader.parse(jsonStr, jsonObj)) {
            std::cerr << "Invalid Json: " << jsonStr << "\n";
            jsonObj = Json::objectValue;
        }
        Json::Value resJson;
        res.set_header("Content-Type", "application/json;charset=UTF-8");
        try {
            if (jsonObj.empty()) {
                throw HTTPException("Json Serialization failed");
            }

            data::Dish dish;
            dish.uuid = util::makeId('D');
            dish.name = jsonObj["name"].asString();
            dish.merchant_id = jsonObj["merchant_id"].asString();
            dish.merchant_name = jsonObj["merchant_name"].asString();
            dish.icon_path = jsonObj["icon_path"].asString();
            dish.description = jsonObj["introduction"].asString();
            dish.base_price = jsonObj["price"].asDouble();
            dish.price_factor = jsonObj["price_factor"].asDouble();
            dish.is_delete = false;

            if (dish.uuid.empty()) {
                res.status = 200;
                resJson["message"] = "uuid生成错误!";
                resJson["success"] = false;
                Json::StreamWriterBuilder writer;
                res.body = Json::writeString(writer, resJson);
                return;
            }

            if (db.searchDishByID(dish.uuid).name.empty()) {
                if (!db.addDish(dish)) {
                    throw HTTPException("databse add dish failed");
                }
                std::cout << "成功向数据库插入新菜品 " << dish.name << "\n";
                resJson["success"] = true;
                resJson["message"] = "菜品创建成功";
                Json::StreamWriterBuilder writer;
                res.body = Json::writeString(writer, resJson);
            } else {
                res.status = 200;
                resJson["message"] = "uuid生成错误!";
                resJson["success"] = false;
                Json::StreamWriterBuilder writer;
                res.body = Json::writeString(writer, resJson);
                return;
            }
        }
        catch (const HTTPException& e) {
            res.status = 500;
            resJson["success"] = false;
            resJson["message"] = e.what();
            Json::StreamWriterBuilder writer;
            res.body = Json::writeString(writer, resJson);
            res.set_header("Content-Type", "application/json;charset=UTF-8");
            std::cerr << e.what() << "\n";
        }
    });

       // 菜品信息查询
       svr.Post("/merchant/dish/info", [this](const httplib::Request& req, httplib::Response& res) {
        std::cout << "/merchant/dish/info收到请求\n";
        std::string jsonStr = req.body;
        Json::Value jsonObj;
        Json::Reader reader;
        
        if (!reader.parse(jsonStr, jsonObj)) {
            std::cerr << "JSON解析错误: " << "\n";
            jsonObj = Json::objectValue;
        }

        Json::Value resJson;
        try {
            if (!jsonObj.isMember("dish_id")) {
                throw HTTPException("缺少菜品ID");
            }

            std::string dish_id = jsonObj["dish_id"].asString();
            data::Dish dish = db.searchDishByID(dish_id);
            
            if (dish.uuid.empty()) {
                throw HTTPException("菜品不存在");
            }
            
            resJson["dish"] = json::toJson(dish);
            res.body = Json::writeString(Json::StreamWriterBuilder(), resJson);
            res.set_header("Content-Type", "application/json");
        }
        catch (const HTTPException& e) {
            res.status = 404;
            resJson["success"] = false;
            resJson["message"] = e.what();
            res.body = Json::writeString(Json::StreamWriterBuilder(), resJson);
            std::cerr << "查询错误: " << e.what() << "\n";
        }
    });

// 菜品更新
svr.Post("/merchant/dish/update", [this](const httplib::Request& req, httplib::Response& res) {
    std::cout << "/merchant/dish/update收到POST请求\n";
    std::cout << "开始更新菜品...\n";

    std::string jsonStr = req.body;
    Json::Value jsonObj;
    Json::Reader reader;
    if (!reader.parse(jsonStr, jsonObj)) {
        std::cerr << "无效的JSON数据: " << jsonStr << "\n";
        jsonObj = Json::objectValue;
    }

    Json::Value resJson;
    res.set_header("Content-Type", "application/json;charset=UTF-8");
    
    try {
        if (jsonObj.empty() || !jsonObj.isMember("dish_id")) {
            throw HTTPException("缺少必要字段");
        }

        std::string dish_id = jsonObj["dish_id"].asString();
        data::Dish dish = db.searchDishByID(dish_id);
        std::cout << "找到菜品ID: " << dish.uuid << "\n";

        dish.name = jsonObj["name"].asString();
        dish.merchant_id = jsonObj["merchant_id"].asString();
        dish.merchant_name = jsonObj["merchant_name"].asString();
        dish.icon_path = jsonObj["icon_path"].asString();
        dish.description = jsonObj["introduction"].asString();
        dish.base_price = jsonObj["price"].asDouble();
        dish.price_factor = jsonObj["price_factor"].asDouble();

        if (!dish.uuid.empty()) {
            if (!db.updateDish(dish)) {
                throw HTTPException("数据库更新失败");
            }
            std::cout << "成功更新菜品: " << dish.name << "\n";
            resJson["success"] = true;
            resJson["message"] = "菜品更新成功";
            res.body = Json::writeString(Json::StreamWriterBuilder(), resJson);
        } else {
            res.status = 404;
            resJson["message"] = "找不到菜品!";
            resJson["success"] = false;
            res.body = Json::writeString(Json::StreamWriterBuilder(), resJson);
        }
    }
    catch (const HTTPException& e) {
        res.status = 500;
        resJson["success"] = false;
        resJson["message"] = e.what();
        res.body = Json::writeString(Json::StreamWriterBuilder(), resJson);
        std::cerr << "数据库错误: " << e.what() << "\n";
    }
});

// 菜品删除
svr.Post("/merchant/dish/del", [this](const httplib::Request& req, httplib::Response& res) {
    std::cout << "/merchant/dish/del收到删除请求\n";
    
    std::string jsonStr = req.body;
    Json::Value jsonObj;
    Json::Reader reader;
    if (!reader.parse(jsonStr, jsonObj)) {
        std::cerr << "无效的JSON数据: " << jsonStr << "\n";
        jsonObj = Json::objectValue;
    }

    Json::Value resJson;
    res.set_header("Content-Type", "application/json;charset=UTF-8");
    
    try {
        if (jsonObj.empty() || !jsonObj.isMember("dish_id")) {
            throw HTTPException("缺少菜品ID");
        }

        std::string dish_id = jsonObj["dish_id"].asString();
        db.delDishByID(dish_id);
        
        resJson["success"] = true;
        resJson["message"] = "删除成功!";
        res.body = Json::writeString(Json::StreamWriterBuilder(), resJson);
    }
    catch (const HTTPException& e) {
        res.status = 500;
        resJson["success"] = false;
        resJson["message"] = e.what();
        res.body = Json::writeString(Json::StreamWriterBuilder(), resJson);
        std::cerr << "删除错误: " << e.what() << "\n";
    }
    });

    // 商家订单列表
svr.Post("/merchant/order/list", [this](const httplib::Request& req, httplib::Response& res) {
    std::cout << "收到商家订单列表请求\n";
    
    std::string jsonStr = req.body;
    Json::Value jsonObj;
    Json::Reader reader;
    if (!reader.parse(jsonStr, jsonObj)) {
        std::cerr << "无效的JSON数据: " << jsonStr << "\n";
        jsonObj = Json::objectValue;
    }

    Json::Value resJson;
    try {
        if (jsonObj.empty() || !jsonObj.isMember("merchant_id")) {
            throw HTTPException("缺少商家ID");
        }

        std::string merchant_id = jsonObj["merchant_id"].asString();
        std::vector<data::Order> orderList = db.getOrderListByMerchant(merchant_id);
        std::cout << "商家:" << merchant_id << " 找到订单 " << orderList.size() << "个\n";
        
        std::string orderListJson = OrderListToJsonArray(orderList);
        resJson["order_list"] = orderListJson;
        
        Json::StreamWriterBuilder writer;
        res.body = Json::writeString(writer, resJson);
        res.set_header("Content-Type", "application/json;charset=UTF-8");
    }
    catch (const HTTPException& e) {
        res.status = 500;
        resJson["success"] = false;
        resJson["message"] = e.what();
        res.body = Json::writeString(Json::StreamWriterBuilder(), resJson);
        std::cerr << "订单列表错误: " << e.what() << "\n";
    }
});

// 订单菜品详情
svr.Post("/merchant/order/detail/dishlist", [this](const httplib::Request& req, httplib::Response& res) {
    std::cout << "获取订单菜品详情\n";
    
    std::string jsonStr = req.body;
    Json::Value jsonObj;
    Json::Reader reader;
    if (!reader.parse(jsonStr, jsonObj)) {
        std::cerr << "无效的JSON数据: " << jsonStr << "\n";
        jsonObj = Json::objectValue;
    }

    Json::Value resJson;
    try {
        if (jsonObj.empty() || !jsonObj.isMember("order_id")) {
            throw HTTPException("缺少订单ID");
        }

        std::string order_id = jsonObj["order_id"].asString();
        std::vector<data::OrderDish> dishList = db.getOrderDishesByID(order_id);
        std::cout << "订单:" << order_id << " 包含菜品 " << dishList.size() << "个\n";
        
        std::string dishListJson = OrderDishListToJsonArray(dishList);
        resJson["dish_list"] = dishListJson;
        
        res.body = Json::writeString(Json::StreamWriterBuilder(), resJson);
        res.set_header("Content-Type", "application/json;charset=UTF-8");
    }
    catch (const HTTPException& e) {
        res.status = 500;
        resJson["success"] = false;
        resJson["message"] = e.what();
        res.body = Json::writeString(Json::StreamWriterBuilder(), resJson);
        std::cerr << "菜品详情错误: " << e.what() << "\n";
    }
});

// 接受订单
svr.Post("/merchant/order/accept", [this](const httplib::Request& req, httplib::Response& res) {
    std::cout << "处理订单接受请求\n";
    
    std::string jsonStr = req.body;
    Json::Value jsonObj;
    Json::Reader reader;
    if (!reader.parse(jsonStr, jsonObj)) {
        std::cerr << "无效的JSON数据: " << jsonStr << "\n";
        jsonObj = Json::objectValue;
    }

    Json::Value resJson;
    try {
        if (jsonObj.empty() || !jsonObj.isMember("order_id")) {
            throw HTTPException("缺少订单ID");
        }

        std::string order_id = jsonObj["order_id"].asString();
        data::Order order = db.searchOrderByID(order_id);
        order.status = data::Order::Status::SUCCESS;
        
        if (!db.updateOrder(order)) {
            throw HTTPException("数据库更新失败");
        }
        db.addHistory(order);
        db.addHistoryDishesByID(order.uuid, db.getOrderDishesByID(order.uuid));

        resJson["success"] = true;
        resJson["message"] = "订单接受成功";
        res.body = Json::writeString(Json::StreamWriterBuilder(), resJson);
        res.set_header("Content-Type", "application/json;charset=UTF-8");
    }
    catch (const HTTPException& e) {
        res.status = 500;
        resJson["success"] = false;
        resJson["message"] = e.what();
        res.body = Json::writeString(Json::StreamWriterBuilder(), resJson);
        std::cerr << "接受订单错误: " << e.what() << "\n";
    }
});

// 拒绝订单
svr.Post("/merchant/order/reject", [this](const httplib::Request& req, httplib::Response& res) {
    std::cout << "处理订单拒绝请求\n";
    
    std::string jsonStr = req.body;
    Json::Value jsonObj;
    Json::Reader reader;
    if (!reader.parse(jsonStr, jsonObj)) {
        std::cerr << "无效的JSON数据: " << jsonStr << "\n";
        jsonObj = Json::objectValue;
    }

    Json::Value resJson;
    try {
        if (jsonObj.empty() || !jsonObj.isMember("order_id")) {
            throw HTTPException("缺少订单ID");
        }

        std::string order_id = jsonObj["order_id"].asString();
        data::Order order = db.searchOrderByID(order_id);
        order.status = data::Order::Status::REJECTED;
        
        if (!db.updateOrder(order)) {
            throw HTTPException("数据库更新失败");
        }
        db.addHistory(order);
        db.addHistoryDishesByID(order.uuid, db.getOrderDishesByID(order.uuid));

        resJson["success"] = true;
        resJson["message"] = "订单已拒绝";
        res.body = Json::writeString(Json::StreamWriterBuilder(), resJson);
        res.set_header("Content-Type", "application/json;charset=UTF-8");
    }
    catch (const HTTPException& e) {
        res.status = 500;
        resJson["success"] = false;
        resJson["message"] = e.what();
        res.body = Json::writeString(Json::StreamWriterBuilder(), resJson);
        std::cerr << "接受订单错误: " << e.what() << "\n";
    }
});





}
void btyGoose::HTTPServer::initAdminAPI()
{
    svr.Post("/admin/order/list", [this](const httplib::Request& req, httplib::Response& res) {
        cout << "/admin/order/list get a post!";

        Json::Value resJson;
        try {
            std::vector<data::Order> orderList = db.getAllOrderList();
            cout << "admin:" << "find orders" << orderList.size();
            std::string orderListJson = OrderListToJsonArray(orderList);
            resJson["order_list"] = orderListJson;
            
            Json::StreamWriterBuilder writer;
            res.body = Json::writeString(writer, resJson);
            res.set_header("Content-Type", "application/json;charset=UTF-8");
        }
        catch (const HTTPException& e)
        {
            res.status = 500;
            resJson["success"] = false;
            resJson["message"] = e.what();
            
            Json::StreamWriterBuilder writer;
            res.body = Json::writeString(writer, resJson);
            res.set_header("Content-Type", "application/json;charset=UTF-8");
            std::cerr << e.what() << "\n";
        }
    });
}

bool btyGoose::HTTPServer::getPay(double pay)
{
    return true;
}
//工具函数
string btyGoose::HTTPServer::DishListToJsonArray(const vector<data::Dish>& dishList)
{
    Json::Value jsonArray(Json::arrayValue);
    Json::Reader reader;

    for (const auto& dish : dishList)
    {
        Json::Value dishJson;
        if (reader.parse(json::toJson(dish), dishJson)) {
            jsonArray.append(dishJson);
        }
    }

    Json::StreamWriterBuilder writer;
    writer["indentation"] = ""; // 紧凑模式
    return Json::writeString(writer, jsonArray);
}
vector<data::Dish> btyGoose::HTTPServer::DishListFromJsonArray(const string& jsonStr)
{
    std::vector<data::Dish> dishList;
    Json::Value root;
    Json::Reader reader;

    if (reader.parse(jsonStr, root) && root.isArray()) {
        for (const auto& item : root) {

            Json::StreamWriterBuilder writer;
            data::Dish dish = json::createDish(Json::writeString(writer, item));
            dishList.push_back(dish);
        }
    }
    return dishList;
}

string btyGoose::HTTPServer::OrderListToJsonArray(const vector<data::Order>&orderList)
{
    Json::Value jsonArray(Json::arrayValue);
    
    for (const auto& order : orderList) {
        Json::Value orderJson;
        Json::Reader().parse(json::toJson(order), orderJson);
        jsonArray.append(orderJson);
    }

    Json::StreamWriterBuilder writer;
    writer["indentation"] = "";
    return Json::writeString(writer, jsonArray);
}
vector<data::Order> btyGoose::HTTPServer::OrderListFromJsonArray(const string& jsonStr)
{
    std::vector<data::Order> orderList;
    Json::Value root;
    Json::Reader reader;

    if (reader.parse(jsonStr, root) && root.isArray()) {
        for (const auto& item : root) {
            ;
            Json::StreamWriterBuilder writer;
            data::Order order = json::createOrder(Json::writeString(writer, item));
            orderList.push_back(order);
        }
    }
    return orderList;
}

string btyGoose::HTTPServer::OrderDishListToJsonArray(const vector<data::OrderDish>&dishList)
{
    Json::Value jsonArray(Json::arrayValue);
    
    for (const auto& dish : dishList) {
        Json::Value dishJson;
        Json::Reader().parse(json::toJson(dish), dishJson);
        jsonArray.append(dishJson);
    }

    Json::StreamWriterBuilder writer;
    writer["indentation"] = "";
    return Json::writeString(writer, jsonArray);
}