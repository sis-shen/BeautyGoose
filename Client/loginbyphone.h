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
    explicit LoginByPhoneWidget(QWidget *parent = nullptr);

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
    QTextEdit* phoneInput;
    QTextEdit* auth_codeInput;
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
