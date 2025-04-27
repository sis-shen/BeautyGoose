#pragma once
#include <string>
#include <random>
#include <array>
#include <jsoncpp/json/json.h>
#include <chrono>
namespace btyGoose{

    namespace util{
        static uint64_t getSecTime()
        {
            return std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch()
            ).count();
        }

        static std::string makeId(const char pre = '#')
        {
            // 生成6字节随机数 (6*8=48bit)
            std::array<uint8_t, 6> bytes;
            std::random_device rd;
            std::mt19937 gen(rd());
            
            // 填充随机字节
            for (auto& byte : bytes) {
                byte = static_cast<uint8_t>(gen() & 0xFF);
            }

            // 转换字节为十六进制字符串
            constexpr char hexmap[] = "0123456789abcdef";
            std::string id(13, pre); // 预填充前缀+12字符
            for (int i = 0; i < 6; ++i) {
                id[1 + 2*i] = hexmap[(bytes[i] & 0xF0) >> 4]; // 高四位
                id[2 + 2*i] = hexmap[bytes[i] & 0x0F];        // 低四位
            }

            return id;
        }
    }

    /// <summary>
	/// 定义核心数据结构
	/// </summary>
    namespace data{
        //账户
        struct Account	
		{
			///账户类型
			enum Type
			{
				CONSUMER,	//消费者账户
				MERCHANT,	//商家账户
				ADMIN		//管理员账户
			};
			enum Level
			{
				MEMBER,		//普通用户
				VIP,		//VIP用户
				SUVIP		//超级VIP用户
			};

			std::string uuid = "";		//唯一id
			std::string name = "";		//账户名
			std::string password = "";	//密码
			std::string nickname = "";	//昵称
			std::string icon;	//图标
			Type type = CONSUMER;			//账户类型
			double balance = 0;		//账户余额
			std::string phone = "";		//电话号码
			Level level = MEMBER;		//优惠等级
        };
        struct Dish
        {
            std::string uuid = "";			//菜品id
            std::string merchant_id = "";	//商家uuid
            std::string merchant_name = "";	//商家uuid
            std::string name = "";			//菜品名称
            std::string icon_path = "";		//网络图片的url
            std::string description = "";	//菜品的描述
            double base_price = 0;		//基础价格
            double price_factor = 1;	//价格影响因素
            bool is_delete = false;		//是否被删除
        
        };

        struct Order
        {
            enum Status
            {
                UNPAYED,		//未支付状态
                WAITING,		//等待商家处理
                OVER_TIME,		//订单超时
                REJECTED,		//商家拒收
                SUCCESS,		//成功完成
                ERR,			//订单出错
                FATAL,			//订单出现致命错误
                CANCELED		//消费者取消订单
            };

            std::string merchant_id;		//商家id
            std::string merchant_name;		//商家账户名称
            std::string consumer_id;		//消费者id
            std::string consumer_name;		//消费者账户名称
            std::string time;				//订单时间
            Account::Level level;		//优惠等级
            double pay;					//订单价格
            std::string uuid;				//订单id
            Status status;				//订单状态
            int sum = 0;				//订单内总菜品数
        };

        struct OrderDish
        {
            std::string order_id = "";		//订单id
            std::string dish_id = "";		//菜品id
            std::string merchant_id = "";	//商家id
            std::string name = "";			//菜品id
            double dish_price = 0;		//当时的基础价格
            int count = 0;				//菜品数量

        };
        
    }//data

    //存放重载函数
    namespace json{


        static std::string toJson(data::Account acc)
        {
            Json::Value root;
        
            // 基本字段映射
            root["uuid"] = acc.uuid;
            root["name"] = acc.name;
            root["password"] = acc.password;
            root["nickname"] = acc.nickname;
            root["icon"] = acc.icon;
            root["type"] = static_cast<int>(acc.type);
            root["balance"] = acc.balance;
            root["phone"] = acc.phone;
            root["level"] = static_cast<int>(acc.level);
    
            // 配置漂亮的输出格式
            Json::StreamWriterBuilder writer;
            writer["indentation"] = "    ";  // 4空格缩进
            writer["commentStyle"] = "None"; // 不保留注释
            writer["enableYAMLCompatibility"] = false; 
            
            return Json::writeString(writer, root);
        }

        static data::Account createAccount(const std::string& jsonStr) {
            Json::Value root;
            JSONCPP_STRING errs;
            Json::CharReaderBuilder reader;
    
            // 创建字符流解析器
            std::unique_ptr<Json::CharReader> jsonReader(reader.newCharReader());
            if(!jsonReader->parse(jsonStr.data(), jsonStr.data()+jsonStr.size(), &root, &errs)) {
                throw std::runtime_error("JSON解析失败: " + errs);
            }
    
            data::Account acc;
    
            // 提取字段
            if(root.isMember("uuid")) acc.uuid = root["uuid"].asString();
            if(root.isMember("name")) acc.name = root["name"].asString();
            if(root.isMember("password")) acc.password = root["password"].asString();
            if(root.isMember("nickname")) acc.nickname = root["nickname"].asString();
            if(root.isMember("icon")) acc.icon = root["icon"].asString();
            if(root.isMember("type")) acc.type = static_cast<data::Account::Type>(root["type"].asInt());
            if(root.isMember("balance")) acc.balance = root["balance"].asDouble();
            if(root.isMember("phone")) acc.phone = root["phone"].asString();
            if(root.isMember("level")) acc.level = static_cast<data::Account::Level>(root["level"].asInt());
    
            return acc;
        }

        static std::string toJson(data::Dish dish)
        {
            Json::Value root;
            root["uuid"] = dish.uuid;
            root["merchant_id"] = dish.merchant_id;
            root["merchant_name"] = dish.merchant_name;
            root["name"] = dish.name;
            root["icon_path"] = dish.icon_path;
            root["description"] = dish.description;
            root["base_price"] = dish.base_price;
            root["price_factor"] = dish.price_factor;
            root["is_delete"] = dish.is_delete;
    
            Json::StreamWriterBuilder writer;
            writer["indentation"] = "    ";
            return Json::writeString(writer, root);
        }

        static data::Dish createDish(const std::string& jsonStr) {
            Json::Value root;
            JSONCPP_STRING errs;
            Json::CharReaderBuilder reader;
            
            std::unique_ptr<Json::CharReader> jsonReader(reader.newCharReader());
            if(!jsonReader->parse(jsonStr.data(), jsonStr.data()+jsonStr.size(), &root, &errs)) {
                throw std::runtime_error("Dish解析失败: " + errs);
            }
    
            data::Dish dish;
            if(root.isMember("uuid")) dish.uuid = root["uuid"].asString();
            if(root.isMember("merchant_id")) dish.merchant_id = root["merchant_id"].asString();
            if(root.isMember("merchant_name")) dish.merchant_name = root["merchant_name"].asString();
            if(root.isMember("name")) dish.name = root["name"].asString();
            if(root.isMember("icon_path")) dish.icon_path = root["icon_path"].asString();
            if(root.isMember("description")) dish.description = root["description"].asString();
            if(root.isMember("base_price")) dish.base_price = root["base_price"].asDouble();
            if(root.isMember("price_factor")) dish.price_factor = root["price_factor"].asDouble();
            if(root.isMember("is_delete")) dish.is_delete = root["is_delete"].asBool();
    
            return dish;
        }

            //====== Order结构体 =⭐️=//
            static std::string toJson(data::Order order) {
                Json::Value root;
                root["merchant_id"] = order.merchant_id;
                root["merchant_name"] = order.merchant_name;
                root["consumer_id"] = order.consumer_id;
                root["consumer_name"] = order.consumer_name;
                root["time"] = order.time;
                root["level"] = static_cast<int>(order.level);
                root["pay"] = order.pay;
                root["uuid"] = order.uuid;
                root["status"] = static_cast<int>(order.status);
                root["sum"] = order.sum;

                Json::StreamWriterBuilder writer;
                writer["indentation"] = "    ";
                return Json::writeString(writer, root);
            }

            static data::Order createOrder(const std::string& jsonStr) {
            Json::Value root;
            JSONCPP_STRING errs;
            Json::CharReaderBuilder reader;
            
            std::unique_ptr<Json::CharReader> jsonReader(reader.newCharReader());
            if(!jsonReader->parse(jsonStr.data(), jsonStr.data()+jsonStr.size(), &root, &errs)) {
                throw std::runtime_error("Order解析失败: " + errs);
            }

            data::Order order;
            if(root.isMember("merchant_id")) order.merchant_id = root["merchant_id"].asString();
            if(root.isMember("merchant_name")) order.merchant_name = root["merchant_name"].asString();
            if(root.isMember("consumer_id")) order.consumer_id = root["consumer_id"].asString();
            if(root.isMember("consumer_name")) order.consumer_name = root["consumer_name"].asString();
            if(root.isMember("time")) order.time = root["time"].asString();
            if(root.isMember("level")) order.level = static_cast<data::Account::Level>(root["level"].asInt());
            if(root.isMember("pay")) order.pay = root["pay"].asDouble();
            if(root.isMember("uuid")) order.uuid = root["uuid"].asString();
            if(root.isMember("status")) order.status = static_cast<data::Order::Status>(root["status"].asInt());
            if(root.isMember("sum")) order.sum = root["sum"].asInt();

            return order;
        }

    //====== OrderDish结构体 ======//
    static std::string toJson(data::OrderDish orderDish) {
        Json::Value root;
        root["order_id"] = orderDish.order_id;
        root["dish_id"] = orderDish.dish_id;
        root["merchant_id"] = orderDish.merchant_id;
        root["name"] = orderDish.name;
        root["dish_price"] = orderDish.dish_price;
        root["count"] = orderDish.count;

        Json::StreamWriterBuilder writer;
        writer["indentation"] = "    ";
        return Json::writeString(writer, root);
    }

    static data::OrderDish createOrderDish(const std::string& jsonStr) {
        Json::Value root;
        JSONCPP_STRING errs;
        Json::CharReaderBuilder reader;
        
        std::unique_ptr<Json::CharReader> jsonReader(reader.newCharReader());
        if(!jsonReader->parse(jsonStr.data(), jsonStr.data()+jsonStr.size(), &root, &errs)) {
            throw std::runtime_error("OrderDish解析失败: " + errs);
        }

        data::OrderDish od;
        if(root.isMember("order_id")) od.order_id = root["order_id"].asString();
        if(root.isMember("dish_id")) od.dish_id = root["dish_id"].asString();
        if(root.isMember("merchant_id")) od.merchant_id = root["merchant_id"].asString();
        if(root.isMember("name")) od.name = root["name"].asString();
        if(root.isMember("dish_price")) od.dish_price = root["dish_price"].asDouble();
        if(root.isMember("count")) od.count = root["count"].asInt();

        return od;
    }
    static std::string DishListToJsonArray(const std::vector<data::Dish>& dishList)
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
    static std::vector<data::Dish> DishListFromJsonArray(const std::string& jsonStr)
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

    static std::string OrderListToJsonArray(const std::vector<data::Order>&orderList)
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
    static std::vector<data::Order> OrderListFromJsonArray(const std::string& jsonStr)
    {
        std::vector<data::Order> orderList;
        Json::Value root;
        Json::Reader reader;

        if (reader.parse(jsonStr, root) && root.isArray()) {
            for (const auto& item : root) {
                Json::StreamWriterBuilder writer;
                data::Order order = json::createOrder(Json::writeString(writer, item));
                orderList.push_back(order);
            }
        }
        return orderList;
    }

    static std::string OrderDishListToJsonArray(const std::vector<data::OrderDish>&dishList)
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

    }//json
}