#ifndef MERCHANTDISHWETAILWINDOW_H
#define MERCHANTDISHWETAILWINDOW_H
#include <QDialog>
#include <QVBoxLayout>
#include "merchantdishdetailwidget.h"
#include "CoreData.h"
class MerchantDishDetailWindow : public QDialog {
    Q_OBJECT
public:
    MerchantDishDetailWindow(const btyGoose::data::Dish* dish) : QDialog(nullptr) {
        // 设置窗口的基本属性
        setWindowTitle("菜品详情");
        setFixedSize(1080, 600);
        // 创建控件
        QWidget *centralWidget = new QWidget(this);
        QVBoxLayout *layout = new QVBoxLayout(centralWidget);
        mdd = new MerchantDishDetailWidget(dish);
        layout->addWidget(mdd);
        centralWidget->setLayout(layout);
    }

    MerchantDishDetailWidget* mdd;
};

#endif // MERCHANTDISHWETAILWINDOW_H
