#ifndef MERCHANTORDERDETAILWINDOW_H
#define MERCHANTORDERDETAILWINDOW_H
#include <QWidget>
#include <QDialog>
#include <QVBoxLayout>
#include "merchantorderdetailwidget.h"
class MerchantOrderDetailWindow : public QDialog {
    Q_OBJECT
public:
    MerchantOrderDetailWindow(QWidget *parent = nullptr) : QDialog(parent) {
        // 设置窗口的基本属性
        setWindowTitle("订单详情窗口");
        setFixedSize(1080, 600);
        // 创建控件
        QWidget *centralWidget = new QWidget(this);
        QVBoxLayout *layout = new QVBoxLayout(centralWidget);
        mod = new MerchantOrderDetailWidget;
        layout->addWidget(mod);
        centralWidget->setLayout(layout);
    }

    MerchantOrderDetailWidget* mod;
};
#endif // MERCHANTORDERDETAILWINDOW_H
