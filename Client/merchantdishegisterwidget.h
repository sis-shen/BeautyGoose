#ifndef MERCHANTREGISTERDISHWIDGET_H
#define MERCHANTREGISTERDISHWIDGET_H

#include <QWidget>
#include <QString>
#include <QLabel>
#include <QGridLayout>
#include <QRadioButton>
#include <QLineEdit>
#include <QPushButton>
#include <QPlainTextEdit>
class MerchantDishRegisterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MerchantDishRegisterWidget(QWidget *parent = nullptr);

    struct Input{
        QString link = "";
        QString name = "";
        double price = 0;
        double price_factor = 1.0;
        QString introduction = "";
    };

private:
    QString* dishPictureLink;
    QString* dishName;
    double* dishPrice;
    QString* dishIntroduction;

    QLineEdit *dishPictureLinkInput;
    QLineEdit *dishNameInput;
    QLineEdit *dishPriceInput;
    QPlainTextEdit *dishIntroductionInput;

public:
    QPushButton* dishRegisterBtn;

signals:
    void dishRegisterSignal(Input input);

public slots:
    void dishRegisterSlot();
};

#endif // MERCHANTREGISTERDISHWIDGET_H
