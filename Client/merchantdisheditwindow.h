#ifndef MERCHANTDISHEDITWINDOW_H
#define MERCHANTDISHEDITWINDOW_H
#include <QWidget>
#include <QDialog>
#include <QVBoxLayout>
#include "merchantdisheditwidget.h"
class MerchantDishEditWindow : public QDialog {
    Q_OBJECT
public:
    MerchantDishEditWindow(QWidget *parent = nullptr) : QDialog(parent) {
        // 设置窗口的基本属性
        setWindowTitle("订单详情窗口");
        setFixedSize(1080, 600);
        // 创建控件
        QWidget *centralWidget = new QWidget(this);
        QVBoxLayout *layout = new QVBoxLayout(centralWidget);
        mde = new MerchantDishEditWidget;
        layout->addWidget(mde);
        centralWidget->setLayout(layout);
    }

    MerchantDishEditWidget* mde;
};
#endif // MERCHANTDISHEDITWINDOW_H