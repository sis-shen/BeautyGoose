#ifndef MERCHANTORDERDETAILWINDOW_H
#define MERCHANTORDERDETAILWINDOW_H
#include <QWidget>
#include <QDialog>
#include <QVBoxLayout>
#include "merchantorderdetailwidget.h"
#include "CoreData.h"
class MerchantOrderDetailWindow : public QDialog {
    Q_OBJECT
public:
    MerchantOrderDetailWindow(const btyGoose::data::Order& order,const QList<btyGoose::data::OrderDish>* dish_list) : QDialog(nullptr) {
        // 设置窗口的基本属性
        setWindowTitle("订单详情窗口");
        setFixedSize(1080, 600);
        // 创建控件
        QWidget *centralWidget = new QWidget(this);
        QVBoxLayout *layout = new QVBoxLayout(centralWidget);
        mod = new MerchantOrderDetailWidget(order,dish_list);
        layout->addWidget(mod);
        centralWidget->setLayout(layout);
    }

    MerchantOrderDetailWidget* mod;
};
#endif // MERCHANTORDERDETAILWINDOW_H
