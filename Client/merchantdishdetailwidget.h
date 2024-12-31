#ifndef MERCHANTDISHDETAILWIDGET_H
#define MERCHANTDISHDETAILWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QIcon>
#include <QPushButton>
class MerchantDishDetailWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MerchantDishDetailWidget(QWidget *parent = nullptr);
    QString dish_id = "order_id";

    // QIcon* icon;//TODO
    QLabel* iconTMP;

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
