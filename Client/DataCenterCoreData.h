#ifndef DATACENTERCOREDATA_H
#define DATACENTERCOREDATA_H
#include <QString>
#include <unordered_map>
#include <QList>
#include <QHash>
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

class Cart
{
    QList<DishItem> list;
};

Cart
}
#endif // DATACENTERCOREDATA_H
