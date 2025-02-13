#ifndef THIRDPAYWINDOW_H
#define THIRDPAYWINDOW_H
#include <QWidget>
#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
class ThirdPayWindow : public QDialog {
    Q_OBJECT
public:
    ThirdPayWindow(QWidget *parent = nullptr) : QDialog(parent) {
        // 设置窗口的基本属性
        setWindowTitle("订单详情窗口");
        setFixedSize(900, 600);
        QVBoxLayout* layout = new QVBoxLayout;
        this->setLayout(layout);
        // 创建控件
        code_icon = new QLabel;
        code_icon->setPixmap(QIcon(QPixmap("://qsrc/QRCode.png")).pixmap(400,400));
        code_icon->setFixedSize(400,400);
        payNum =new QLabel("应支付:00元");
        payBtn = new QPushButton("支付");
        layout->addWidget(code_icon);
        layout->addWidget(payNum);
        layout->addWidget(payBtn);

        connect(payBtn,&QPushButton::clicked,this,&ThirdPayWindow::payConfirmlySlot);
    }
    QString order_id = "order_id";
    QPushButton* payBtn;
    QLabel* code_icon;
    QLabel*payNum;
public slots:
    void payConfirmlySlot(){emit payConfirmlySignal(order_id);}
signals:
    void payConfirmlySignal(QString order_id);
};
#endif // THIRDPAYWINDOW_H
