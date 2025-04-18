#include "HTTPServer.hpp"

using namespace btyGoose;

HTTPServer* HTTPServer::_ins = nullptr;

using std::cout;
btyGoose::HTTPServer::HTTPServer()
{
    // 关键参数设置
    svr.set_read_timeout(5, 0);     // 5秒读超时 
    svr.set_write_timeout(5, 0);    // 5秒写超时
    svr.set_payload_max_length(1024 * 1024 * 10); // 10MB最大请求体

    // 启用keep-alive
    svr.set_keep_alive_max_count(1000); // 最大长连接数
    // 启动服务时指定线程数（建议CPU核数×2）
    svr.new_task_queue = [] { return new httplib::ThreadPool(4); };
}

void btyGoose::HTTPServer::setDB(std::shared_ptr<DatabaseClient>& ptr)
{
    // LOG_TRACE("HTTPServer: 开始初始化db");
    db = ptr;
}

void btyGoose::HTTPServer::setRedis(std::shared_ptr<RedisClient>&ptr)
{
    redis = ptr;
}

void btyGoose::HTTPServer::init(const std::string& _host,const uint32_t _port)
{
    host = _host;
    port = _port;
    initTestAPI();
    initAccountAPI();
    initConsumerAPI();
    initMerchantAPI();
    initAdminAPI();
    LOG_DEBUG("HTTPServer初始化完成");
}

void btyGoose::HTTPServer::start()
{
    LOG_DEBUG("开始启动DatabaseClient");
    db->start();
    LOG_DEBUG("HTTPServer开始监听{}:{}",host,port);
    cout <<svr.listen(host, port);
}

bool btyGoose::HTTPServer::checkRedisOrderListExist(const data::Order&order)
{
    bool has_consumer = redis->hasOrderListByUserId(order.consumer_id);
    bool has_merchant = redis->hasOrderListByUserId(order.merchant_id);

    if(has_consumer && has_merchant) return true;
    if(!has_consumer)
    {
        auto order_list = db->getOrderListByConsumer(order.consumer_id);
        redis->setOrderList(order_list);
        redis->setOrderListByIdDone(order.consumer_id);
    }
    if(!has_merchant)
    {
        auto dish_list = db->getOrderListByMerchant(order.merchant_id);
        redis->setOrderList(dish_list);
        redis->setOrderListByIdDone(order.merchant_id);
    }
    return false;
}

void btyGoose::HTTPServer::initTestAPI()
{
    svr.Get("/ping", [=](const httplib::Request& req, httplib::Response& res) {
        LOG_DEBUG("收到HTTP请求, method={},path={}",req.method,req.path);
        cout << "/ping\n";
        res.body = "pong";
        res.set_header("Content-Type", "application/text;charset=UTF-8");
        });
    
        svr.Get("/flush/redis",[=](const httplib::Request& req, httplib::Response& res){
            LOG_INFO("收到HTTP请求, method={},path={}",req.method,req.path);
            auto [begin,end] = req.params.equal_range("password");
            std::string password = begin->second;
            if(password == "IDOCONFIRM")
            {
                redis->flushall();
                res.body = "flush done";
            }
            else
            {
                res.body = "密码错误";
            }
            res.set_header("Content-Type", "application/text;charset=UTF-8");
        });
}

void btyGoose::HTTPServer::initAccountAPI()
{
    // ==============用户名登录================ //
    svr.Post("/account/login/username", [this](const httplib::Request& req, httplib::Response& res) {
        LOG_DEBUG("收到HTTP请求, method={},path={}",req.method,req.path);
        
        Json::Value reqJson, resJson;
        Json::Reader reader;
        
        // 解析JSON请求体
        if (!reader.parse(req.body, reqJson)) {
            LOG_ERROR("[用户名登录] 无效的JSON格式: {}",req.body);
            resJson["success"] = false;
            resJson["message"] = "无效的JSON格式";
            res.body = Json::FastWriter().write(resJson);
            res.set_header("Content-Type", "application/json;charset=UTF-8");
            return;
        }
        try {
            data::Account record_acc;
            std::string name = reqJson["name"].asString();
            record_acc = redis->getAccountByName(name);
            if(record_acc.uuid.empty())
            {
                //缓存不命中
                record_acc = db->searchAccountByName(name);
                if(!record_acc.uuid.empty())
                {
                    redis->setAccount(record_acc);
                }
            }
            
            if (!record_acc.name.empty() && reqJson["password"].asString() == record_acc.password) {
                // 登录成功响应
                resJson["success"] = true;
                resJson["message"] = "登录认证成功";
                resJson["account"] = json::toJson(record_acc); // 假设已实现toJson方法
                LOG_INFO("登录成功,id:{}",record_acc.uuid);
            } else {
                // 登录失败响应
                resJson["success"] = false;
                resJson["message"] = "登录认证失败";
                LOG_INFO("登录失败,id:{}",record_acc.uuid);

            }
            
            res.body = Json::FastWriter().write(resJson);
        }
        catch (const HTTPException& e) {
            LOG_ERROR("账户登录出错,信息:{}",e.what());
            res.status = 500;
            resJson["success"] = false;
            resJson["message"] = e.what();
            res.body = Json::FastWriter().write(resJson);
        }
        
        res.set_header("Content-Type", "application/json;charset=UTF-8");
    });

    // =============== 手机号登录 =========== //
    svr.Post("/account/login/phone", [this](const httplib::Request& req, httplib::Response& res) {
        LOG_DEBUG("收到HTTP请求, method={},path={}",req.method,req.path);
        
        Json::Value reqJson, resJson;
        Json::Reader reader;
        
        if (!reader.parse(req.body, reqJson)) {
            LOG_ERROR("[手机号登录]无效的JSON格式:{}",req.body);
            resJson["success"] = false;
            resJson["message"] = "无效的JSON格式";
            res.body = Json::FastWriter().write(resJson);
            res.set_header("Content-Type", "application/json;charset=UTF-8");
            return;
        }

        try {
            data::Account record_acc;
            std::string phone = reqJson["phone"].asString();
            record_acc = redis->getAccountByPhone(phone);
            if(record_acc.uuid.empty())
            {
                //缓存不命中
                record_acc =  db->searchAccountByPhone(phone);
                if(!record_acc.uuid.empty())
                {
                    redis->setAccount(record_acc);
                }
            }


            if (!record_acc.phone.empty() && reqJson["password"].asString() == record_acc.password) {
                resJson["success"] = true;
                resJson["message"] = "手机登录成功";
                resJson["account"] = json::toJson(record_acc);
                LOG_INFO("手机号登录成功,id:{}",record_acc.uuid);
            } else {
                resJson["success"] = false;
                resJson["message"] = "手机认证失败";
                LOG_INFO("手机号登录认证失败,id:{}",record_acc.uuid);
            }
            
            res.body = Json::FastWriter().write(resJson);
        }
        catch (const HTTPException& e) {
            LOG_ERROR("[手机号登录出错],信息:{}",e.what());
            std::cerr << "[ERROR] " << e.what() << std::endl;
            res.status = 500;
            resJson["success"] = false;
            resJson["message"] = e.what();
            res.body = Json::FastWriter().write(resJson);
        }
        
        res.set_header("Content-Type", "application/json;charset=UTF-8");
    });

    //  ============= 账户注册 =============
    svr.Post("/account/register", [this](const httplib::Request& req, httplib::Response& res) {
        LOG_DEBUG("收到HTTP请求, method={},path={}",req.method,req.path);
        
        Json::Value reqJson, resJson;
        Json::Reader reader;
        res.set_header("Content-Type", "application/json;charset=UTF-8");

        if (!reader.parse(req.body, reqJson)) {
            LOG_ERROR("[账户注册]无效的JSON格式:{}",req.body);
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
                LOG_ERROR("[账户注册]UUID生成失败:空的uuid");
                resJson["message"] = "UUID生成错误";
                resJson["success"] = false;
                res.body = Json::FastWriter().write(resJson);
                return;
            }

            // 验证码校验
            if (!AuthenticateAuthCode(acc.phone, reqJson["auth_code"].asString())) {
                LOG_INFO("[账户注册]验证码错误,注册失败");
                resJson["message"] = "注册失败,验证码错误";
                resJson["success"] = false;
                res.body = Json::FastWriter().write(resJson);
                return;
            }

            // 用户名查重
            if (!db->searchAccountByName(acc.name).name.empty()) {
                LOG_INFO("[账户注册]失败：用户名已存在");
                resJson["message"] = "注册失败,用户名已存在:"+ acc.name;
                resJson["success"] = false;
                res.body = Json::FastWriter().write(resJson);
                return;
            }

            // 手机号查重
            if (!db->searchAccountByPhone(acc.phone).name.empty()) {
                LOG_INFO("[账户注册]失败：手机号已存在");
                resJson["message"] = "注册失败,手机号已被占用:"+acc.phone;
                resJson["success"] = false;
                res.body = Json::FastWriter().write(resJson);
                return;
            }

            // 创建账户
            if (!db->addAccount(acc)) {
                throw HTTPException("数据库写入失败");
            }

            redis->setAccount(acc);

            LOG_INFO("[账户注册]成功,id:",acc.uuid);
            resJson["success"] = true;
            resJson["message"] = "注册成功";
            res.body = Json::FastWriter().write(resJson);
        }
        catch (const HTTPException& e) {
            LOG_ERROR("[账户注册]错误，信息:{}",e.what());
            res.status = 500;
            resJson["success"] = false;
            resJson["message"] = e.what();
            res.body = Json::FastWriter().write(resJson);
        }
    });

    // =============== 等级更新接口 ============
    svr.Post("/account/update/level", [this](const httplib::Request& req, httplib::Response& res) {
        LOG_DEBUG("收到HTTP请求, method={},path={}",req.method,req.path);
        
        Json::Value reqJson, resJson;
        Json::Reader reader;
        
        if (!reader.parse(req.body, reqJson)) {
            LOG_ERROR("[账户等级更新]无效的JSON格式:{}",req.body);

            resJson["success"] = false;
            resJson["message"] = "无效的JSON格式";
            res.body = Json::FastWriter().write(resJson);
            return;
        }

        try {
            data::Account acc;
            std::string id = reqJson["id"].asString();
            acc = redis->getAccountById(id);
            if(acc.uuid.empty())
                acc = db->searchAccountByID(id);
            std::string level = reqJson["level"].asString();
            
            LOG_INFO("[用户等级更新]id:{},{}->{}",acc.uuid,acc.level,level);

            if (level == "VIP") {
                acc.level = data::Account::Level::VIP;
            } else if (level == "SUVIP") {
                acc.level = data::Account::Level::SUVIP;
            } else {
                throw HTTPException("未知的等级类型: " + level);
            }

            db->updateAccount(acc);
            redis->setAccount(acc);
            resJson["level"] = static_cast<int>(acc.level);
            res.body = Json::FastWriter().write(resJson);
            LOG_INFO("[用户等级更新]成功");
        }
        catch (const HTTPException& e) {
            LOG_ERROR("[用户等级更新]出错，信息:{}",e.what());
            res.status = 500;
            resJson["success"] = false;
            resJson["message"] = e.what();
            res.body = Json::FastWriter().write(resJson);
        }
        
        res.set_header("Content-Type", "application/json");
    });

    //============= 更新用户昵称 ============== //
    svr.Post("/account/update/nickname", [this](const httplib::Request& req, httplib::Response& res) {
        LOG_DEBUG("收到HTTP请求, method={},path={}",req.method,req.path);
        
        Json::Value reqJson, resJson;
        Json::Reader reader;
        res.set_header("Content-Type", "application/json");
    
        // 解析JSON请求
        if (!reader.parse(req.body, reqJson)) {
            LOG_ERROR("[账户更新昵称]无效的JSON格式:{}",req.body);

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
            
            LOG_INFO("[用户昵称更新]id:{},?->{}",userId,newNickname);
    
            // 查询并更新账户
            data::Account acc;
            acc = redis->getAccountById(userId);
            if(acc.uuid.empty())
                acc = db->searchAccountByID(userId);
            acc.nickname = newNickname;
            
            if (!db->updateAccount(acc)) {
                throw HTTPException("数据库更新失败");
            }
            
            // 验证更新结果
            data::Account updatedAcc = db->searchAccountByID(userId);
            redis->setAccount(updatedAcc);
            resJson["nickname"] = updatedAcc.nickname;
            resJson["success"] = true;
            res.body = Json::FastWriter().write(resJson);
            LOG_INFO("[用户昵称更新]成功，id:{}",updatedAcc.uuid);
        }
        catch (const HTTPException& e) {
            LOG_ERROR("[昵称更新]出错，信息:{}",e.what());
            res.status = 500;
            resJson["success"] = false;
            resJson["message"] = e.what();
            res.body = Json::FastWriter().write(resJson);
        }
    
    });
}

bool btyGoose::HTTPServer::AuthenticateAuthCode(const string& phone, const string& auth_code)
{
    ///////////////////////////
    // TODO
    // ///////////////////
    //这里就不具体实现了
    // if (auth_code == "888888")
    // {
    //     return true;
    // }
    // else
    // {
    //     return false;
    // }
    return true;
}


void btyGoose::HTTPServer::initConsumerAPI()
{
    //=========== 消费者获取订单的菜品列表 ===========
    svr.Post("/consumer/order/detail/dishlist", [this](const httplib::Request& req, httplib::Response& res) {
        LOG_DEBUG("收到HTTP请求, method={},path={}",req.method,req.path);

        std::string jsonStr = req.body;
        Json::Value jsonObj;
        Json::Reader reader;
        if (!reader.parse(jsonStr, jsonObj)) {
            LOG_ERROR("[消费者获取订单菜品列表]无效的JSON格式:{}",req.body);

            jsonObj = Json::objectValue;
        }
        Json::Value resJson;
        try {
            if (jsonObj.empty()) {
                throw HTTPException("Json Serialization failed");
            }

            std::string order_id = jsonObj["order_id"].asString();
            std::string dish_list_json = redis->getOrderDishListJson(order_id);
            if(dish_list_json.empty())
            {
                std::vector<data::OrderDish> dishList = db->getOrderDishesByID(order_id);
                // std::cout << "order:" << order_id << " find dishes " << dishList.size() << "\n";
                std::string dishListJson = OrderDishListToJsonArray(dishList);
                resJson["dish_list"] = dishListJson;
                Json::StreamWriterBuilder writer;
                res.body = Json::writeString(writer, resJson);
                redis->setOrderDishListJson(order_id,dishListJson);
            }
            else 
            {
                resJson["dish_list"] = dish_list_json;
                Json::StreamWriterBuilder writer;
                res.body = Json::writeString(writer, resJson);
            }
            res.set_header("Content-Type", "application/json;charset=UTF-8");
            LOG_INFO("[消费者获取订单菜品列表]成功,order_id:{}",order_id);
        }
        catch (const HTTPException& e) {
            res.status = 500;
            resJson["success"] = false;
            resJson["message"] = e.what();
            Json::StreamWriterBuilder writer;
            res.body = Json::writeString(writer, resJson);
            res.set_header("Content-Type", "application/json;charset=UTF-8");
            LOG_ERROR("[消费者获取菜品列表]出错，信息:{}",e.what());
        }
    });

    // ================= 消费者获取菜品列表 ============= //
    svr.Post("/consumer/dish/list", [this](const httplib::Request& req, httplib::Response& res) {
        LOG_DEBUG("收到HTTP请求, method={},path={}",req.method,req.path);
        
        std::vector<data::Dish> dishList = db->getAllDishList();
        std::string dishListJson = DishListToJsonArray(dishList);
        Json::Value resJson;
        resJson["dish_list"] = dishListJson;
        Json::StreamWriterBuilder writer;
        res.body = Json::writeString(writer, resJson);
        res.set_header("Content-Type", "application/json;charset=UTF-8");
        LOG_INFO("[消费者获取菜品列表]成功,总数:{}",dishList.size());
    });

    // ============== 消费者获取菜品详情 ===================//
    svr.Post("/consumer/dish/dishInfo", [this](const httplib::Request& req, httplib::Response& res) {
        LOG_DEBUG("收到HTTP请求, method={},path={}",req.method,req.path);
        
        std::string jsonStr = req.body;
        Json::Value jsonObj;
        Json::Reader reader;
        if (!reader.parse(jsonStr, jsonObj)) {
            LOG_ERROR("[消费者获取菜品详情]无效的JSON格式:{}",req.body);
            jsonObj = Json::objectValue;
        }
        Json::Value resJson;
        try {
            if (jsonObj.empty()) {
                throw HTTPException("Json Serialization failed");
            }

            std::string dish_id = jsonObj["dish_id"].asString();
            data::Dish dish;
            dish = redis->getDishById(dish_id);
            if(dish.uuid.empty())
            {
                dish = db->searchDishByID(dish_id);
                if(!dish.uuid.empty())
                {
                    redis->setDish(dish);
                }
            }

            if(!dish.uuid.empty())
            {
                resJson["dish"] = json::toJson(dish);
                Json::StreamWriterBuilder writer;
                res.body = Json::writeString(writer, resJson);
                res.set_header("Content-Type", "application/json;charset=UTF-8");
                LOG_INFO("[消费者获取菜品详情]成功,dish id:{}",dish_id);
                LOG_TRACE("{}",res.body);
            }
            else
            {
                res.status = 404;
                resJson["success"] = false;
                resJson["message"] = "菜品不存在";
                Json::StreamWriterBuilder writer;
                res.body = Json::writeString(writer, resJson);
                res.set_header("Content-Type", "application/json;charset=UTF-8");
                LOG_WARN("[消费者获取菜品详情]出错，菜品不存在");
            }
        }
        catch (const HTTPException& e) {
            res.status = 500;
            resJson["success"] = false;
            resJson["message"] = e.what();
            Json::StreamWriterBuilder writer;
            res.body = Json::writeString(writer, resJson);
            res.set_header("Content-Type", "application/json;charset=UTF-8");
            LOG_ERROR("[消费者获取菜品详情]出错，信息:{}",e.what());
        }
    });

    // =========== 订单生成 ============== //
    svr.Post("/consumer/order/generate", [this](const httplib::Request& req, httplib::Response& res) {
        LOG_DEBUG("收到HTTP请求, method={},path={}",req.method,req.path);
        
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

            LOG_DEBUG("[生成订单]order id:{}",order.uuid);
            
            //先插入订单
            bool ok = db->addOrder(order);
            if(!ok)
            {
                LOG_ERROR("订单生成失败,id: {},来自消费者:{}",order.uuid,order.consumer_id);
            }
            //如果缓存不存在，则重新全部加载
            if(checkRedisOrderListExist(order))
            {
                redis->setOrder(order);
            }
            std::vector<data::OrderDish> dish_list;
            Json::Value jsonArr = jsonObj["dish_arr"];
            for (const auto& value : jsonArr) {
                data::OrderDish dish;
                dish.order_id = order.uuid;
                dish.merchant_id = order.merchant_id;
                dish.name = value["dish_name"].asString();
                dish.dish_price = value["dish_price"].asDouble();
                dish.count = value["count"].asInt();
                dish.dish_id = value["dish_id"].asString();
                dish_list.push_back(dish);
                LOG_TRACE("生成一个OrderDish, dish id:{}",dish.dish_id);
            }
            ok = db->addOrderDishesByID(order.uuid, dish_list);
            if(!ok)
                LOG_ERROR("订单菜品插入失败,order_id: {},来自消费者:{}",order.uuid,order.consumer_id);
            Json::StreamWriterBuilder dish_writer;
            redis->setOrderDishListJson(order.uuid,OrderDishListToJsonArray(dish_list)); 

            resJson["order"] = json::toJson(order);
            Json::StreamWriterBuilder writer;
            res.body = Json::writeString(writer, resJson);
            res.set_header("Content-Type", "application/json;charset=UTF-8");
            LOG_INFO("[订单生成]成功,order_id:{}",order.uuid);
        }
        catch (const HTTPException& e) {
            res.status = 500;
            resJson["success"] = false;
            resJson["message"] = e.what();
            Json::StreamWriterBuilder writer;
            res.body = Json::writeString(writer, resJson);
            res.set_header("Content-Type", "application/json;charset=UTF-8");
            LOG_ERROR("[订单生成]出错，信息：{}",e.what());
        }
    });

    // ===================消费者获取订单列表 ============= // 
    svr.Post("/consumer/order/list", [this](const httplib::Request& req, httplib::Response& res) {
        LOG_DEBUG("收到HTTP请求, method={},path={}",req.method,req.path);
        
        std::string jsonStr = req.body;
        Json::Value jsonObj;
        Json::Reader reader;
        if (!reader.parse(jsonStr, jsonObj)) {
            LOG_ERROR("[消费者获取订单列表]无效的JSON格式:{}",req.body);
            jsonObj = Json::objectValue;
        }
        Json::Value resJson;
        try {
            if (jsonObj.empty()) {
                throw HTTPException("Json Serialization failed");
            }

            std::string consumer_id = jsonObj["consumer_id"].asString();
            std::vector<data::Order> orderList;
            if(redis->hasOrderListByUserId(consumer_id))
            {
                orderList = redis->getOrderListByConsumer(consumer_id);
            }
            //缓存不命中或者缓存为空
            if(orderList.empty())
            {
                orderList = db->getOrderListByConsumer(consumer_id);
                redis->setOrderList(orderList);
                redis->setOrderListByIdDone(consumer_id);
            }
            // std::cout << "consumer:" << consumer_id << " find orders " << orderList.size() << "\n";
            std::string dishListJson = OrderListToJsonArray(orderList);
            resJson["order_list"] = dishListJson;
            Json::StreamWriterBuilder writer;
            res.body = Json::writeString(writer, resJson);
            res.set_header("Content-Type", "application/json;charset=UTF-8");
            LOG_INFO("[消费者获取订单列表]成功，总数:{}",orderList.size());
        }
        catch (const HTTPException& e) {
            res.status = 500;
            resJson["success"] = false;
            resJson["message"] = e.what();
            Json::StreamWriterBuilder writer;
            res.body = Json::writeString(writer, resJson);
            res.set_header("Content-Type", "application/json;charset=UTF-8");
            LOG_ERROR("[消费者获取订单列表]出错，信息:{}",e.what());
        }
    });

    svr.Post("/consumer/order/pay/confirm", [this](const httplib::Request& req, httplib::Response& res) {
        LOG_DEBUG("收到HTTP请求, method={},path={}",req.method,req.path);
        
        std::string jsonStr = req.body;
        Json::Value jsonObj;
        Json::Reader reader;
        if (!reader.parse(jsonStr, jsonObj)) {
            LOG_ERROR("[消费者确认支付订单]无效的JSON格式:{}",req.body);
            jsonObj = Json::objectValue;
        }
        Json::Value resJson;
        try {
            if (jsonObj.empty()) {
                throw HTTPException("Json Serialization failed");
            }
            data::Order order;
            std::string order_id = jsonObj["order_id"].asString();

            order = redis->getOrderById(order_id);
            if(order.uuid.empty())
            {
                order = db->searchOrderByID(order_id);
            }
            order.status = data::Order::Status::WAITING;
            db->updateOrder(order);
            redis->setOrder(order);
            Json::StreamWriterBuilder writer;
            res.body = Json::writeString(writer, resJson);
            res.set_header("Content-Type", "application/json");
            LOG_INFO("[消费者确认支付订单]成功,order id",jsonObj["order_id"].asString());
        }
        catch (const HTTPException& e) {
            res.status = 500;
            resJson["success"] = false;
            resJson["message"] = e.what();
            Json::StreamWriterBuilder writer;
            res.body = Json::writeString(writer, resJson);
            res.set_header("Content-Type", "application/json");
            LOG_ERROR("[消费者确认支付订单]出错，信息:{}",e.what());
        }
    });

    svr.Post("/consumer/order/cancel", [this](const httplib::Request& req, httplib::Response& res) {
        LOG_DEBUG("收到HTTP请求, method={},path={}",req.method,req.path);
        
        std::string jsonStr = req.body;
        Json::Value jsonObj;
        Json::Reader reader;
        if (!reader.parse(jsonStr, jsonObj)) {
            LOG_ERROR("[消费者取消订单]无效的JSON格式:{}",req.body);

            std::cerr << "Invalid Json: " << jsonStr << "\n";
            jsonObj = Json::objectValue;
        }
        Json::Value resJson;
        try {
            if (jsonObj.empty()) {
                throw HTTPException("Json Serialization failed");
            }
            data::Order order;
            std::string order_id = jsonObj["order_id"].asString();
            order = redis->getOrderById(order_id);
            if(order.uuid.empty())
            {
                order = db->searchOrderByID(order_id);
            }
            std::string reason;
            if (order.status == data::Order::Status::UNPAYED) {
                order.status = data::Order::Status::CANCELED;
                db->updateOrder(order);
                reason = "订单取消成功";
            } else if (order.status == data::Order::Status::WAITING) {
                reason = "订单无法取消：已支付";
            } else {
                reason = "订单无法支付,订单状态码:" + std::to_string(static_cast<int>(order.status));
            }
            redis->setOrder(order);

            resJson["reason"] = reason;
            Json::StreamWriterBuilder writer;
            res.body = Json::writeString(writer, resJson);
            res.set_header("Content-Type", "application/json;charset=UTF-8");
            LOG_INFO("[消费者取消订单]结果:{}",reason);
        }
        catch (const HTTPException& e) {
            res.status = 500;
            resJson["success"] = false;
            resJson["message"] = e.what();
            Json::StreamWriterBuilder writer;
            res.body = Json::writeString(writer, resJson);
            res.set_header("Content-Type", "application/json;charset=UTF-8");
            LOG_ERROR("[消费者取消订单]出错，信息:{}",e.what());
        }
    });
}
void btyGoose::HTTPServer::initMerchantAPI()
{
    svr.Post("/merchant/dish/list", [this](const httplib::Request& req, httplib::Response& res) {
        LOG_DEBUG("收到HTTP请求, method={},path={}",req.method,req.path);
        
        std::string jsonStr = req.body;
        Json::Value jsonObj;
        Json::Reader reader;
        if (!reader.parse(jsonStr, jsonObj)) {
            LOG_ERROR("[商家获取菜品列表]无效的JSON格式:{}",req.body);
            // std::cerr << "Invalid Json: " << jsonStr << "\n";
            jsonObj = Json::objectValue;
        }
        Json::Value resJson;
        try {
            if (jsonObj.empty()) {
                throw HTTPException("Json Serialization failed");
            }
            
            std::string merchant_id = jsonObj["merchant_id"].asString();
            std::string dishListJson = redis->getDishListJsonByMerchant(merchant_id); 
            LOG_TRACE("Redis获取到的菜品列表json: {}",dishListJson);
            if(dishListJson.empty())
            {
                std::vector<data::Dish> dishList = db->getDishListByMerchant(merchant_id);
                dishListJson = DishListToJsonArray(dishList);
                LOG_TRACE("数据库查询到菜品数量:{} , 获得的菜品json: {}",dishList.size(),dishListJson);
                redis->setDishListJsonByMerchant(merchant_id,dishListJson);
            }
            resJson["dish_list"] = dishListJson;
            Json::StreamWriterBuilder writer;
            res.body = Json::writeString(writer, resJson);
            res.set_header("Content-Type", "application/json;charset=UTF-8");
            LOG_INFO("[商家获取菜品列表]成功");
        }
        catch (const HTTPException& e) {
            res.status = 500;
            resJson["success"] = false;
            resJson["message"] = e.what();
            Json::StreamWriterBuilder writer;
            res.body = Json::writeString(writer, resJson);
            res.set_header("Content-Type", "application/json;charset=UTF-8");
            LOG_ERROR("[商家获取菜品列表]出错，信息:{}",e.what());
        }
    });

    // 菜品注册
    svr.Post("/merchant/dish/register", [this](const httplib::Request& req, httplib::Response& res) {
        LOG_DEBUG("收到HTTP请求, method={},path={}",req.method,req.path);
        
        std::string jsonStr = req.body;
        Json::Value jsonObj;
        Json::Reader reader;
        if (!reader.parse(jsonStr, jsonObj)) {
            LOG_ERROR("[商家注册菜品]无效的JSON格式:{}",req.body);

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
                LOG_ERROR("[商家注册菜品] uuid生成错误");
                return;
            }

            if (db->searchDishByID(dish.uuid).name.empty()) {
                if (!db->addDish(dish)) {
                    throw HTTPException("databse add dish failed");
                }
                redis->setDish(dish);
                redis->delDishListJsonByMerchant(dish.merchant_id);//缓存失效
                // std::cout << "成功向数据库插入新菜品 " << dish.name << "\n";
                resJson["success"] = true;
                resJson["message"] = "菜品创建成功";
                Json::StreamWriterBuilder writer;
                res.body = Json::writeString(writer, resJson);
                LOG_INFO("[商家注册菜品]成功,dish id:{}",dish.uuid);
            } else {
                res.status = 200;
                resJson["message"] = "uuid生成重复!";
                resJson["success"] = false;
                Json::StreamWriterBuilder writer;
                res.body = Json::writeString(writer, resJson);
                LOG_ERROR("[商家注册菜品] uuid生成重复");
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
            LOG_ERROR("[商家注册菜品]出错，信息:{}",e.what());
        }
    });

       // 菜品信息查询 debug版
    svr.Post("/debug/merchant/dish/info", [this](const httplib::Request& req, httplib::Response& res) {
        LOG_DEBUG("收到HTTP请求, method={},path={}",req.method,req.path);
        ScopedTimer timer("菜品详情查询");
        std::string jsonStr = req.body;
        Json::Value jsonObj;
        Json::Reader reader;
        
        if (!reader.parse(jsonStr, jsonObj)) {
            LOG_ERROR("[商家查询菜品详情] uuid生成错误");
            jsonObj = Json::objectValue;
        }
        ///////////////////////
        LOG_TRACE("Json解析完成,    耗时 {}μs",timer.staged());
        //////////////////////////
        Json::Value resJson;
        try {
            if (!jsonObj.isMember("dish_id")) {
                throw HTTPException("缺少菜品ID");
            }

            std::string dish_id = jsonObj["dish_id"].asString();
            data::Dish dish;
            //////////////////
            timer.staged();
            /////////////////
            dish = redis->getDishById(dish_id);
            //////////////////
            LOG_TRACE("完成Redis的get查询,  耗时 {}μs",timer.staged());
            /////////////////
            if(dish.uuid.empty())
            {
                timer.staged();
                dish = db->searchDishByID(dish_id);
                LOG_TRACE("完成数据库查询数据，耗时 {}μs",timer.staged());
                if (dish.uuid.empty()) {
                    throw HTTPException("菜品不存在");
                }
                timer.staged();
                redis->setDish(dish);
                LOG_TRACE("完成Redis设置缓存，耗时 {}μs",timer.staged());
            }
            timer.staged();
            resJson["dish"] = json::toJson(dish);
            res.body = Json::writeString(Json::StreamWriterBuilder(), resJson);
            LOG_TRACE("完成Json序列化和报文写入,耗时 {}μs",timer.staged());
            res.set_header("Content-Type", "application/json");
            LOG_INFO("[商家查询菜品详情]成功");
        }
        catch (const HTTPException& e) {
            res.status = 404;
            resJson["success"] = false;
            resJson["message"] = e.what();
            res.body = Json::writeString(Json::StreamWriterBuilder(), resJson);
            LOG_ERROR("[商家查询菜品详情]出错,信息:{}",e.what());
        }
    });

           // 菜品信息查询
        svr.Post("/merchant/dish/info", [this](const httplib::Request& req, httplib::Response& res) {
        LOG_DEBUG("收到HTTP请求, method={},path={}",req.method,req.path);
        std::string jsonStr = req.body;
        Json::Value jsonObj;
        Json::Reader reader;
        
        if (!reader.parse(jsonStr, jsonObj)) {
            LOG_ERROR("[商家查询菜品详情] uuid生成错误");
            jsonObj = Json::objectValue;
        }

        Json::Value resJson;
        try {
            if (!jsonObj.isMember("dish_id")) {
                throw HTTPException("缺少菜品ID");
            }

            std::string dish_id = jsonObj["dish_id"].asString();
            data::Dish dish;
            dish = redis->getDishById(dish_id);
            if(dish.uuid.empty())
            {
                dish = db->searchDishByID(dish_id);
                if (dish.uuid.empty()) {
                    throw HTTPException("菜品不存在");
                }
                redis->setDish(dish);
            }
            
            resJson["dish"] = json::toJson(dish);
            res.body = Json::writeString(Json::StreamWriterBuilder(), resJson);
            res.set_header("Content-Type", "application/json");
            LOG_INFO("[商家查询菜品详情]成功");
        }
        catch (const HTTPException& e) {
            res.status = 404;
            resJson["success"] = false;
            resJson["message"] = e.what();
            res.body = Json::writeString(Json::StreamWriterBuilder(), resJson);
            LOG_ERROR("[商家查询菜品详情]出错,信息:{}",e.what());
        }
    });
    
    // 菜品更新
    svr.Post("/merchant/dish/update", [this](const httplib::Request& req, httplib::Response& res) {
    LOG_DEBUG("收到HTTP请求, method={},path={}",req.method,req.path);

    std::string jsonStr = req.body;
    Json::Value jsonObj;
    Json::Reader reader;
    if (!reader.parse(jsonStr, jsonObj)) {
        LOG_ERROR("[商家更新菜品]无效的JSON格式:{}",req.body);
        jsonObj = Json::objectValue;
    }

    Json::Value resJson;
    res.set_header("Content-Type", "application/json;charset=UTF-8");
    
    try {
        if (jsonObj.empty() || !jsonObj.isMember("dish_id")) {
            throw HTTPException("缺少必要字段 dish_id");
        }

        std::string dish_id = jsonObj["dish_id"].asString();
        data::Dish dish;
        dish = redis->getDishById(dish_id);
        if(dish.uuid.empty())
        {
            dish = db->searchDishByID(dish_id);
        }
        // std::cout << "找到菜品ID: " << dish.uuid << "\n";

        dish.name = jsonObj["name"].asString();
        dish.merchant_id = jsonObj["merchant_id"].asString();
        dish.merchant_name = jsonObj["merchant_name"].asString();
        dish.icon_path = jsonObj["icon_path"].asString();
        dish.description = jsonObj["introduction"].asString();
        dish.base_price = jsonObj["price"].asDouble();
        dish.price_factor = jsonObj["price_factor"].asDouble();

        if (!dish.uuid.empty()) {
            if (!db->updateDish(dish)) {
                throw HTTPException("数据库更新失败");
            }
            redis->setDish(dish);
            redis->delDishListJsonByMerchant(dish.merchant_id);
            // std::cout << "成功更新菜品: " << dish.name << "\n";
            resJson["success"] = true;
            resJson["message"] = "菜品更新成功";
            res.body = Json::writeString(Json::StreamWriterBuilder(), resJson);
            LOG_INFO("[商家更新菜品]成功,dish id:{}",dish.uuid);
        } else {
            res.status = 404;
            resJson["message"] = "找不到菜品!";
            resJson["success"] = false;
            res.body = Json::writeString(Json::StreamWriterBuilder(), resJson);
            LOG_INFO("[商家更新菜品]失败：找不到菜品");
        }
    }
    catch (const HTTPException& e) {
        res.status = 500;
        resJson["success"] = false;
        resJson["message"] = e.what();
        res.body = Json::writeString(Json::StreamWriterBuilder(), resJson);
        LOG_ERROR("[商家更新菜品]出错，信息:{}",e.what());
    }
});

// 菜品删除
svr.Post("/merchant/dish/del", [this](const httplib::Request& req, httplib::Response& res) {
    LOG_DEBUG("收到HTTP请求, method={},path={}",req.method,req.path);
    
    std::string jsonStr = req.body;
    Json::Value jsonObj;
    Json::Reader reader;
    if (!reader.parse(jsonStr, jsonObj)) {
        LOG_ERROR("[商家删除菜品]无效的JSON格式:{}",req.body);

        jsonObj = Json::objectValue;
    }

    Json::Value resJson;
    res.set_header("Content-Type", "application/json;charset=UTF-8");
    
    try {
        if (jsonObj.empty() || !jsonObj.isMember("dish_id")) {
            throw HTTPException("缺少菜品ID");
        }

        std::string dish_id = jsonObj["dish_id"].asString();
        db->delDishByID(dish_id);
        redis->delDishById(dish_id);
        
        resJson["success"] = true;
        resJson["message"] = "删除成功!";
        res.body = Json::writeString(Json::StreamWriterBuilder(), resJson);
        LOG_INFO("[商家删除菜品]成功");
    }
    catch (const HTTPException& e) {
        res.status = 500;
        resJson["success"] = false;
        resJson["message"] = e.what();
        res.body = Json::writeString(Json::StreamWriterBuilder(), resJson);
        LOG_ERROR("[商家删除菜品]错误,信息:{}",e.what());
    }
    });

    // 商家订单列表
svr.Post("/merchant/order/list", [this](const httplib::Request& req, httplib::Response& res) {
    LOG_DEBUG("收到HTTP请求, method={},path={}",req.method,req.path);
    
    std::string jsonStr = req.body;
    Json::Value jsonObj;
    Json::Reader reader;
    if (!reader.parse(jsonStr, jsonObj)) {
        LOG_ERROR("[商家获取订单列表]无效的JSON格式:{}",req.body);

        jsonObj = Json::objectValue;
    }

    Json::Value resJson;
    try {
        if (jsonObj.empty() || !jsonObj.isMember("merchant_id")) {
            throw HTTPException("缺少商家ID");
        }

        std::string merchant_id = jsonObj["merchant_id"].asString();
        std::vector<data::Order> orderList;
        if(redis->hasOrderListByUserId(merchant_id))
        {
            orderList = redis->getOrderListByMerchant(merchant_id);
        }
        if(orderList.empty())
        {
            orderList = db->getOrderListByMerchant(merchant_id);
            redis->setOrderList(orderList);
            redis->setOrderListByIdDone(merchant_id);
        }
        
        std::string orderListJson = OrderListToJsonArray(orderList);
        resJson["order_list"] = orderListJson;
        
        Json::StreamWriterBuilder writer;
        res.body = Json::writeString(writer, resJson);
        res.set_header("Content-Type", "application/json;charset=UTF-8");
        LOG_INFO("[商家获取订单列表]成功，数量:{}",orderList.size());
    }
    catch (const HTTPException& e) {
        res.status = 500;
        resJson["success"] = false;
        resJson["message"] = e.what();
        res.body = Json::writeString(Json::StreamWriterBuilder(), resJson);
        LOG_ERROR("[商家获取订单列表]出错，信息:{}",e.what());
    }
});

// 订单菜品详情
svr.Post("/merchant/order/detail/dishlist", [this](const httplib::Request& req, httplib::Response& res) {
    LOG_DEBUG("收到HTTP请求, method={},path={}",req.method,req.path);
    
    std::string jsonStr = req.body;
    Json::Value jsonObj;
    Json::Reader reader;
    if (!reader.parse(jsonStr, jsonObj)) {
        LOG_ERROR("[商家获取订单菜品列表]无效的JSON格式:{}",req.body);
        jsonObj = Json::objectValue;
    }

    Json::Value resJson;
    try {
        if (jsonObj.empty() || !jsonObj.isMember("order_id")) {
            throw HTTPException("缺少订单ID");
        }

        std::string order_id = jsonObj["order_id"].asString();
        std::string dishListJson;
        dishListJson = redis->getOrderDishListJson(order_id);
        if(dishListJson.empty())
        {
            std::vector<data::OrderDish> dishList = db->getOrderDishesByID(order_id);
            dishListJson = OrderDishListToJsonArray(dishList);
            redis->setOrderDishListJson(order_id,dishListJson);
        }
        
        resJson["dish_list"] = dishListJson;
        
        res.body = Json::writeString(Json::StreamWriterBuilder(), resJson);
        res.set_header("Content-Type", "application/json;charset=UTF-8");
        LOG_INFO("[商家获取订单菜品列表]成功");
    }
    catch (const HTTPException& e) {
        res.status = 500;
        resJson["success"] = false;
        resJson["message"] = e.what();
        res.body = Json::writeString(Json::StreamWriterBuilder(), resJson);
        LOG_ERROR("[商家获取订单菜品列表]出错,信息:{}",e.what());
    }
});

// 接受订单
svr.Post("/merchant/order/accept", [this](const httplib::Request& req, httplib::Response& res) {
    LOG_DEBUG("收到HTTP请求, method={},path={}",req.method,req.path);
    
    std::string jsonStr = req.body;
    Json::Value jsonObj;
    Json::Reader reader;
    if (!reader.parse(jsonStr, jsonObj)) {
        LOG_ERROR("[商家接受订单]无效的JSON格式:{}",req.body);
        jsonObj = Json::objectValue;
    }

    Json::Value resJson;
    try {
        if (jsonObj.empty() || !jsonObj.isMember("order_id")) {
            throw HTTPException("缺少订单ID");
        }

        std::string order_id = jsonObj["order_id"].asString();
        data::Order order;
        order = redis->getOrderById(order_id);
        if(order.uuid.empty())
        {
            order = db->searchOrderByID(order_id);
        }
        order.status = data::Order::Status::SUCCESS;
        
        if (!db->updateOrder(order)) {
            throw HTTPException("数据库更新失败");
        }
        redis->setOrder(order);
        
        db->addHistory(order);
        db->addHistoryDishesByID(order.uuid, db->getOrderDishesByID(order.uuid));

        resJson["success"] = true;
        resJson["message"] = "订单接受成功";
        res.body = Json::writeString(Json::StreamWriterBuilder(), resJson);
        res.set_header("Content-Type", "application/json;charset=UTF-8");
        LOG_INFO("[商家接受订单]成功,order id:{}",order.uuid);
    }
    catch (const HTTPException& e) {
        res.status = 500;
        resJson["success"] = false;
        resJson["message"] = e.what();
        res.body = Json::writeString(Json::StreamWriterBuilder(), resJson);
        LOG_ERROR("[商家接受订单]出错,信息:{}",e.what());
    }
});

// 拒绝订单
svr.Post("/merchant/order/reject", [this](const httplib::Request& req, httplib::Response& res) {
    LOG_DEBUG("收到HTTP请求, method={},path={}",req.method,req.path);
    
    std::string jsonStr = req.body;
    Json::Value jsonObj;
    Json::Reader reader;
    if (!reader.parse(jsonStr, jsonObj)) {
        LOG_ERROR("[商家拒绝订单]无效的JSON格式:{}",req.body);
        jsonObj = Json::objectValue;
    }

    Json::Value resJson;
    try {
        if (jsonObj.empty() || !jsonObj.isMember("order_id")) {
            throw HTTPException("缺少订单ID");
        }

        std::string order_id = jsonObj["order_id"].asString();
        data::Order order;
        order = redis->getOrderById(order_id);
        if(order.uuid.empty())
        {
            order = db->searchOrderByID(order_id);
        }
        order.status = data::Order::Status::REJECTED;
        
        if (!db->updateOrder(order)) {
            throw HTTPException("数据库更新失败");
        }
        redis->delOrderById(order_id);
        db->addHistory(order);
        db->addHistoryDishesByID(order.uuid, db->getOrderDishesByID(order.uuid));

        resJson["success"] = true;
        resJson["message"] = "订单成功拒绝";
        res.body = Json::writeString(Json::StreamWriterBuilder(), resJson);
        res.set_header("Content-Type", "application/json;charset=UTF-8");
        LOG_INFO("[商家拒绝订单]成功,id:{}",order.uuid);
    }
    catch (const HTTPException& e) {
        res.status = 500;
        resJson["success"] = false;
        resJson["message"] = e.what();
        res.body = Json::writeString(Json::StreamWriterBuilder(), resJson);
        LOG_ERROR("[商家拒绝订单]出错，信息:{}",e.what());
    }
});


}
void btyGoose::HTTPServer::initAdminAPI()
{
    svr.Post("/admin/order/list", [this](const httplib::Request& req, httplib::Response& res) {
        LOG_DEBUG("收到HTTP请求, method={},path={}",req.method,req.path);

        Json::Value resJson;
        try {
            std::vector<data::Order> orderList = db->getAllOrderList();
            cout << "admin:" << "find orders" << orderList.size();
            std::string orderListJson = OrderListToJsonArray(orderList);
            resJson["order_list"] = orderListJson;
            
            Json::StreamWriterBuilder writer;
            res.body = Json::writeString(writer, resJson);
            res.set_header("Content-Type", "application/json;charset=UTF-8");
            LOG_INFO("[管理员获取历史订单]成功，数量:{}",orderList.size());
        }
        catch (const HTTPException& e)
        {
            res.status = 500;
            resJson["success"] = false;
            resJson["message"] = e.what();
            
            Json::StreamWriterBuilder writer;
            res.body = Json::writeString(writer, resJson);
            res.set_header("Content-Type", "application/json;charset=UTF-8");
            LOG_ERROR("[管理员获取历史订单]出错，信息:{}",e.what());
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