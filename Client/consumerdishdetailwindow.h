#ifndef CONSUMERDISHDETAILWINDOW_H
#define CONSUMERDISHDETAILWINDOW_H
#include <QDialog>
#include <QVBoxLayout>
#include "consumerdishdetailwidget.h"
#include "CoreData.h"
class ConsumerDishDetailWindow : public QDialog {
    Q_OBJECT
public:
    ConsumerDishDetailWindow(const btyGoose::data::Dish* dish,int n) {
        // 设置窗口的基本属性
        setWindowTitle("Custom Sub Window");
        setFixedSize(1080, 600);
        // 创建控件
        QWidget *centralWidget = new QWidget(this);
        QVBoxLayout *layout = new QVBoxLayout(centralWidget);
        cdd = new ConsumerDishDetailWidget(dish,n);
        layout->addWidget(cdd);
        centralWidget->setLayout(layout);
    }

    ConsumerDishDetailWidget* cdd;
// public slots:
//     void onClose()
//     {
//         emit closeEvent();
//     }
// signals:
//     void closeEvent();
};
#endif // CONSUMERDISHDETAILWINDOW_H
