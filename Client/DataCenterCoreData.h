#ifndef DATACENTERCOREDATA_H
#define DATACENTERCOREDATA_H
#include <QString>
#include <QList>
#include <QHash>
#include <QPointer>
namespace btyGoose
{
struct CartDishItem
{
    using ptr = QSharedPointer<CartDishItem>;
    QString dish_id;
    QString dish_name;
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
    QList<CartDishItem>* list = nullptr;
    double pay;     //总价格
    double sum;//菜品总数
    QString merchant_id;
    QString merchant_name;
    QString conumser_id;
};

struct ConsumerOrderList
{
    QHash<QString,ConsumerOrderItem::ptr>* orderTable;
};
}
#endif // DATACENTERCOREDATA_H
