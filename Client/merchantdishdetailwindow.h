#ifndef MERCHANTDISHWETAILWINDOW_H
#define MERCHANTDISHWETAILWINDOW_H
#include <QDialog>
#include <QVBoxLayout>
#include "merchantdishdetailwidget.h"
class MerchantDishDetailWindow : public QDialog {
    Q_OBJECT
public:
    MerchantDishDetailWindow(QWidget *parent = nullptr) : QDialog(parent) {
        // 设置窗口的基本属性
        setWindowTitle("Custom Sub Window");
        setFixedSize(1080, 600);
        // 创建控件
        QWidget *centralWidget = new QWidget(this);
        QVBoxLayout *layout = new QVBoxLayout(centralWidget);
        mdd = new MerchantDishDetailWidget;
        layout->addWidget(mdd);
        centralWidget->setLayout(layout);
    }

    MerchantDishDetailWidget* mdd;
};

#endif // MERCHANTDISHWETAILWINDOW_H
