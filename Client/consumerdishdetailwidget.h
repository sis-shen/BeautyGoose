#ifndef CONSUMERDISHDETAILWIDGET_H
#define CONSUMERDISHDETAILWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QIcon>
#include <QPushButton>
#include "CoreData.h"
#include "ColorConfig.h"
class ConsumerDishDetailWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ConsumerDishDetailWidget(const btyGoose::data::Dish* dish,int num);

    QString merchant_id="merchant_id";
    QString dish_id = "order_id";
    int num;//购物车中初识数量
    const btyGoose::data::Dish* _dish;

    // QIcon* icon;//TODO
    QLabel* icon_label;

    QWidget* upW;
    QWidget* downW;
    QWidget* ulW;
    QWidget* urW;

    QLabel* cnt;
    void setCnt(int n);
    void initUp();
    void initDown();
public slots:
    void addSlot();
    void popSlot();

signals:
    void cartDishAddSignal(QString merchant_id,QString order_id);
    void cartDishPopSignal(QString merchant_id,QString order_id);
};

#endif // CONSUMERDISHDETAILWIDGET_H
