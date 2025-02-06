#ifndef LOGINBYPHONE_H
#define LOGINBYPHONE_H
#include "QRadioButton"
#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
class LoginByPhoneWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LoginByPhoneWidget(QString phone,QString password);
    struct Input
    {
        QString type;
        QString phone;
        QString password;
    };

private:
    QRadioButton* consumerRadio;
    QRadioButton* merchantRadio;
    QRadioButton *adminRadio;
    QLineEdit* phoneInput;
    QLineEdit* passwordInput;

    QPushButton* loginBtn;
    QPushButton* toNameLoginBtn;
    QPushButton* toRegisterBtn;

    bool check();
public slots:
    void loginSlot();
    void toNameBtnClickedSlot();
    void toRegisterBtnClickedSlot();

signals:
    void toNameLoginSignal();
    void toRegisterSignal();
    void loginSignal(Input input);
};

#endif // LOGINBYPHONE_H
