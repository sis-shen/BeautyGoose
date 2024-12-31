#ifndef CONSUMERDISHDETAILWIDGET_H
#define CONSUMERDISHDETAILWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QIcon>
#include <QPushButton>
class ConsumerDishDetailWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ConsumerDishDetailWidget(QWidget *parent = nullptr);

    QString merchant_id="merchant_id";
    QString dish_id = "order_id";

    // QIcon* icon;//TODO
    QLabel* iconTMP;

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
