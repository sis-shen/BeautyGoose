#ifndef MERCHANTDISHDETAILWIDGET_H
#define MERCHANTDISHDETAILWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QIcon>
#include <QPushButton>
#include "CoreData.h"
#include "ColorConfig.h"
class MerchantDishDetailWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MerchantDishDetailWidget(const btyGoose::data::Dish*dish);
    QString dish_id = "order_id";
    const btyGoose::data::Dish* dish;
    // QIcon* icon;//TODO
    QLabel* icon_label;

    QWidget* upW;
    QWidget* downW;
    QWidget* ulW;
    QWidget* urW;

    void setCnt(int n);
    void initUp();
    void initDown();
public slots:
    void editSlot();
signals:
    void dishEditSignal(QString dish_id);

};

#endif // MERCHANTDISHDETAILWIDGET_H
