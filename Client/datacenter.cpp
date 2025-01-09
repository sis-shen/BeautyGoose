#include "datacenter.h"

DataCenter* DataCenter::datacenter = nullptr;

DataCenter::DataCenter()
{
    if(!loadConfig())
    {
        qDebug()<<configFileName<<"服务器地址配置错误";
        exit(1);
    }

    client = new network::NetClient(this,httpUrl,sockUrl);

    dish_list_table = new QHash<QString,QList<data::Dish>>;
    cart_list = new CartList;
    cart_list->cart_table = new QHash<QString,Cart::ptr>;
    // consumer_order_list = new ConsumerOrderList;
    // consumer_order_list->orderTable = new QHash<QString,ConsumerOrderItem::ptr>;
    order_table =  new QHash<QString,data::Order>;

    merchant_dish_table = new QHash<QString,data::Dish>;
    merchant_order_table = new QHash<QString,data::Order>;
}

QList<data::Dish> DataCenter::DishListFromJsonArray(const QString &jsonString)
{
    // qDebug()<<jsonString;
    QList<data::Dish> dishList;

    // 解析 JSON 字符串为 QJsonDocument
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8());

    // 如果文档有效且是一个 JSON 数组
    if (doc.isArray()) {
        QJsonArray jsonArray = doc.array();

        // 遍历数组中的每个元素
        for (const QJsonValue& value : jsonArray) {
            if (value.isObject()) {
                // 通过 Dish 类的 fromJson 方法将 QJsonObject 转换为 Dish 对象
                QJsonDocument doc(value.toObject());
                data::Dish dish;
                // qDebug()<<QString(doc.toJson()).toStdString();
                dish.loadFromJson(QString(doc.toJson()).toStdString());
                // qDebug()<<dish.merchant_name;
                dishList.append(dish);
            }
        }
    }

    return dishList;
}

QString DataCenter::DishListToJsonArray(const QList<data::Dish> &dishList)
{
    QJsonArray jsonArray;

    // 遍历 dishList，将每个 Dish 对象的 JSON 添加到 jsonArray 中
    for (const auto& dish : dishList)
    {
        // 将每个 Dish 对象转换为 JSON 字符串并加入到 JSON 数组中
        jsonArray.append(QJsonDocument::fromJson(dish.toJson().toUtf8()).object());
    }

    // 将 QJsonArray 转换为 JSON 字符串
    QJsonDocument doc(jsonArray);
    return QString(doc.toJson(QJsonDocument::Compact));
}

bool DataCenter::loadConfig()
{
    QFile file(configFileName);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);

        if (doc.isObject()) {
            QJsonObject jsonObject = doc.object();
            httpUrl = jsonObject["httpUrl"].toString();
            sockUrl = jsonObject["sockUrl"].toString();

            return true;
        }
        else
        {
            qDebug()<<"无法打开为json";
        }
    }
    else
    {
        qDebug()<<"文件打开错误";
    }
    return false;
}

bool DataCenter::getAuthcode(const QString &phoneNumber)
{
    return true;
}

void DataCenter::accountRegisterAsync(const QString &name, const QString &password, const QString &phone, const QString &nickname, const QString auth_code, int type)
{
    //加密password
    QByteArray hashPass = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    //调用接口
    client->accountRegister(name,hashPass,phone,nickname,auth_code,type);
}

void DataCenter::loginByNameAsync(const QString &name, const QString &password)
{
    //加密password
    QByteArray hashPass = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    client->accountLoginByName(name,hashPass);
}

void DataCenter::loginByPhoneAsync(const QString &phone, const QString &password)
{

}

void DataCenter::accountUpdateAsync()
{

}

void DataCenter::accountChangeNicknameAsync(const QString &nickname)
{

}

void DataCenter::consumerGetDishListAsync()
{
    client->consumerGetDishList();
    qDebug()<<"转到消费者界面";
}

void DataCenter::consumerGetDishInfoAsync(const QString &dish_id)
{
    client->consumerGetDishInfo(dish_id);
}

void DataCenter::consumerOrderGenerateAsync(const QString &merchant_id)
{
    auto it = cart_list->cart_table->find(merchant_id);
    if(it == cart_list->cart_table->end()) return;//购物车不存在

    client->consumerOrderGenerate(merchant_id);
}

void DataCenter::consumerGetOrderInfoAsync(const QString &order_id)
{
    datacenter->consumer_order_item;
}

int DataCenter::getCartDishNum(const QString &merchant_id, const QString &dish_id)
{
    // return cart_list->cart_table->value(merchant_id)->dish_table->value(dish_id)->cnt;
    auto cart_table = cart_list->cart_table;
    if(cart_table->find(merchant_id) == cart_table->end())
    {
        return 0;//尚未加入购物车
    }
    auto dish_table = cart_table->value(merchant_id)->dish_table;
    if(dish_table->find(dish_id) == dish_table->end())
    {
        return 0;//尚未加入购物车
    }
    return dish_table->value(dish_id)->cnt;
}

void DataCenter::merchantGetDishListAysnc()
{
    client->merchantGetDishList(account->uuid);
}

void DataCenter::merchantDishRegisterAsync(const QString &name, const QString &link, double price,double price_factor , const QString &introduction)
{
    client->merchantDishRegister(name,link,price,price_factor,introduction);
}

void DataCenter::merchantDishEditSaveAsync(const QString &dish_id, const QString &name, const QString &link, double price, double price_factor, const QString &introduction)
{
    client->merchantDishEditSave(dish_id,name,link,price,price_factor,introduction);
}

void DataCenter::merchantGetDishInfoAsync(const QString &dish_id)
{
    client->merchantGetDishInfo(dish_id);
}

void DataCenter::merchantDishEditDelAsync(const QString &dish_id)
{
    client->merchantDishEditDel(dish_id);
}

void DataCenter::merchantGetOrderListAsync()
{
    client->merchantGetOrderList(account->uuid);
}

QList<data::Order> DataCenter::OrderListFromJsonArray(const QString &jsonString)
{
    QList<btyGoose::data::Order> orderList;

    // 解析 JSON 字符串为 QJsonDocument
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8());

    // 如果文档有效且是一个 JSON 数组
    if (doc.isArray()) {
        QJsonArray jsonArray = doc.array();

        // 遍历数组中的每个元素
        for (const QJsonValue& value : jsonArray) {
            if (value.isObject()) {
                // 通过 Dish 类的 fromJson 方法将 QJsonObject 转换为 Dish 对象
                QJsonDocument doc(value.toObject());
                data::Order order;
                order.loadFromJson(QString(doc.toJson()).toStdString());
                orderList.append(order);
            }
        }
    }

    return orderList;
}




