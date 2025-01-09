#ifndef MERCHANTDISHEDITWIDGET_H
#define MERCHANTDISHEDITWIDGET_H

#include <QWidget>
#include <QString>
#include <QLabel>
#include <QGridLayout>
#include <QRadioButton>
#include <QLineEdit>
#include <QPushButton>
#include <QPlainTextEdit>
#include "CoreData.h"
class MerchantDishEditWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MerchantDishEditWidget(const btyGoose::data::Dish* dish);
    struct Input{
        QString dish_id = "";
        QString link = "";
        QString name = "";
        double price = 0;
        double price_factor = 1.0;
        QString introduction = "";
    };
    QString dish_id;
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
    QPushButton* dishSaveBtn;
    QPushButton* dishDeleteBtn;

public slots:
    void saveSlot();
    void delSlot();
signals:
    void merchantDishSaveSignal(Input input);
    void merchantDishDelSignal(QString dish_id);
};

#endif // MERCHANTDISHEDITWIDGET_H
