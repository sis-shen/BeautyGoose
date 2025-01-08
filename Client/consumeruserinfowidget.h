#ifndef CONSUMERUSERINFOWIDGET_H
#define CONSUMERUSERINFOWIDGET_H

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
class ConsumerUserInfoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ConsumerUserInfoWidget(const btyGoose::data::Account*account);
    ConsumerNavWidget* leftNavW;
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

#endif // CONSUMERUSERINFOWIDGET_H
