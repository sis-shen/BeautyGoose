#ifndef ACCOUNTUSERINFOWIDGET_H
#define ACCOUNTUSERINFOWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QScrollBar>
#include <QPushButton>
#include <QLabel>
#include <QString>
#include <QSpacerItem>
#include <QList>
#include <QPointer>
#include <QLineEdit>
#include "Nav.h"
#include "CoreData.h"
class MerchantUserInfoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MerchantUserInfoWidget(const btyGoose::data::Account*account);
    MerchantNavWidget* leftNavW;
    QWidget* rightW;
    QLabel* icon;
    QLabel* nameLB;
    QLineEdit* nicknameLB;
    QLabel* phoneLB;
    void initRightW();

    const btyGoose::data::Account* _account;
public slots:
    void changeNicknameSlot();
signals:
    void changeNicknameSignal(QString nickname);
};

#endif // ACCOUNTUSERINFOWIDGET_H
