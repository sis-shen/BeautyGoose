#ifndef REGISTERWIDGET_H
#define REGISTERWIDGET_H

#include <QWidget>
#include <QRadioButton>
#include <QTextEdit>
#include <QPushButton>
#include <QLineEdit>
class RegisterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RegisterWidget(QWidget *parent = nullptr);

    struct Input
    {
        QString type;
        QString name;
        QString nickname;
        QString password;
        QString phone;
        QString auth_code;
    };

private:
    QRadioButton* consumerRadio;
    QRadioButton* merchantRadio;
    QLineEdit* nameInput;
    QLineEdit* nicknameInput;
    QLineEdit* passwordInput;
    QLineEdit* passwordRepeatInput;
    QLineEdit* phoneInput;
    QLineEdit* auth_codeInput;

    QPushButton* registerBtn;
    QPushButton* getCodeBtn;
    QPushButton* toNameLoginBtn;
    QPushButton* toPhoneLoginBtn;

    bool check();
public slots:
    void registerSlot();
    void getCodeSlot();
    void toNameSlot();
    void toPhoneSlot();
signals:
    void toNameLoginSignal();
    void toPhoneLoginSignal();
    void authCodeSignal();
    void registerSiganl(Input input);
};

#endif // REGISTERWIDGET_H
