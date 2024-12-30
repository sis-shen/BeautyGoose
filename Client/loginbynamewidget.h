#ifndef LOGINBYNAMEWIDGET_H
#define LOGINBYNAMEWIDGET_H

#include <QWidget>
#include <QString>
#include <QLabel>
#include <QGridLayout>
#include <QRadioButton>
#include <QLineEdit>
#include <QPushButton>

class LoginByNameWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LoginByNameWidget(QWidget *parent = nullptr);

    struct Input{
        QString type;
        QString name;
        QString password;
    };

private:
    QRadioButton *consumerRadio;
    QRadioButton *merchantRadio;
    QRadioButton *adminRadio;

    QLineEdit *nameInput;
    QLineEdit *passwordInput;

public:
    QPushButton *loginBtn;
    QPushButton *toPhoneBtn;
    QPushButton* toRegisterBtn;

    bool check();

signals:
    void toPhoneLoginSignal();
    void toRegisterSignal();
    void loginSignal(Input input);

public slots:
    void loginSlot();
    //先暂时设置两个槽接受信号
    void toPhoneBtnClickedSlot();
    void toRegisterBtnClickedSlot();
};

#endif // LOGINBYNAMEWIDGET_H
