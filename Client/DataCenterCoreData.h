#ifndef DATACENTERCOREDATA_H
#define DATACENTERCOREDATA_H
#include <QString>
#include <QList>
#include <QHash>
#include <QPointer>
namespace btyGoose
{
struct DishItem
{
    QString dish_id;
    QString dish_name;
    double dish_price;  //单价
    double pay;         //总价
    int cnt;            //数量
};

struct DishList
{
    QList<DishItem> list;
};

struct Cart
{
    using ptr= QSharedPointer<Cart>;
    QList<DishItem> list;
    QString merchant_id;
};

struct CartList
{
    QHash<QString,Cart::ptr> CartTable;
};

struct OrderItem
{
    using ptr = QSharedPointer<OrderItem>;
    QList<DishItem> list;
    double pay;     //总价格
    double sum;//菜品总数
    QString merchant_id;
};

struct OrderList
{
    QHash<QString,OrderItem::ptr> orderTabel;
};
}
#endif // DATACENTERCOREDATA_H
