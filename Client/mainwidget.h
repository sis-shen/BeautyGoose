#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include "registerwidget.h"
#include "datacenter.h"
#include <QMessageBox>
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWidget;
}
QT_END_NAMESPACE

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget *parent = nullptr);
    ~MainWidget();
private:
    //控件操作
    void clearAll();
private:
//Account模块
    void initRegisterConnection(RegisterWidget*);
public slots:
//Account模块
    void toRegisterSlot();
    void toNameLoginSlot();
    void toPhoneLoginSlot();
    void authCodeSlot();
    void registerSlot(RegisterWidget::Input input);

private:
    QString host = "127.0.0.1";
    int port = 80;
    Ui::MainWidget *ui;
    DataCenter dc;
};
#endif // MAINWIDGET_H
