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
    QList<DishItem>* list = nullptr;
};

struct Cart
{
    using ptr= QSharedPointer<Cart>;
    QList<DishItem>* list;
    QString merchant_id;
};

struct CartList
{
    QHash<QString,Cart::ptr>* CartTable = nullptr;
};

struct ConsumerOrderItem
{
    using ptr = QSharedPointer<ConsumerOrderItem>;
    QList<DishItem>* list = nullptr;
    double pay;     //总价格
    double sum;//菜品总数
    QString merchant_id;
    QString conumser_id;
};

struct ConsumerOrderList
{
    QHash<QString,ConsumerOrderItem::ptr>* orderTable;
};
}
#endif // DATACENTERCOREDATA_H
