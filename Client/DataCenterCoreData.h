#ifndef DATACENTERCOREDATA_H
#define DATACENTERCOREDATA_H
#include <QString>
#include <QList>
#include <QHash>
#include <QPointer>
#include "CoreData.h"
namespace btyGoose
{
struct CartDishItem
{
    using ptr = QSharedPointer<CartDishItem>;
    QString dish_id;
    QString dish_name;
    QString icon_path;
    double dish_price;  //单价
    double pay;         //总价
    int cnt;            //数量
};


struct Cart
{
    using ptr= QSharedPointer<Cart>;
    QHash<QString,CartDishItem::ptr>* dish_table;
    QString merchant_id;
    QString merchant_name;
    double pay;
    int cnt;
};

struct CartList
{
    QHash<QString,Cart::ptr>* cart_table = nullptr;
};

struct ConsumerOrderItem
{
    using ptr = QSharedPointer<ConsumerOrderItem>;
    QSharedPointer<QList<CartDishItem>> list = nullptr;
    QSharedPointer<data::Order> order = nullptr;
};

// struct ConsumerOrderList
// {
//     QHash<QString,ConsumerOrderItem::ptr>* orderTable;//订单id->订单
// };
}
#endif // DATACENTERCOREDATA_H
