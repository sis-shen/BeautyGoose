#ifndef MERCHANTDISHREGISTERWINDOW_H
#define MERCHANTDISHREGISTERWINDOW_H

#include <QWidget>
#include <QDialog>
#include <QVBoxLayout>
#include "merchantdishegisterwidget.h"
class MerchantDishRegisterWindow : public QDialog {
    Q_OBJECT
public:
    MerchantDishRegisterWindow(QWidget *parent = nullptr) : QDialog(parent) {
        // 设置窗口的基本属性
        setWindowTitle("菜品注册窗口");
        setFixedSize(1600, 900);
        // 创建控件
        QWidget *centralWidget = new QWidget(this);
        QVBoxLayout *layout = new QVBoxLayout(centralWidget);
        mrd = new MerchantDishRegisterWidget;
        layout->addWidget(mrd);
        centralWidget->setLayout(layout);
    }

    MerchantDishRegisterWidget* mrd;
};



#endif // MERCHANTDISHREGISTERWINDOW_H
