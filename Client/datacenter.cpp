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

    dish_list_table = new DishListTable;
    cart_list = new CartList;
    cart_list->cart_table = new QHash<QString,Cart::ptr>;
    consumer_order_list = new ConsumerOrderList;
    consumer_order_list->orderTable = new QHash<QString,ConsumerOrderItem::ptr>;
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

void DataCenter::consumerGetDishListAsync()
{

}

void DataCenter::consumerGetDishInfoAsync(const QString &dihs_id)
{

}

void DataCenter::consumerOrderGenerate(const QString &merchant_id)
{
    auto it = cart_list->cart_table->find(merchant_id);
    if(it == cart_list->cart_table->end()) return;//购物车不存在

}




