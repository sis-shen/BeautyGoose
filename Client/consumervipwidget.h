#ifndef CONSUMERVIPWIDGET_H
#define CONSUMERVIPWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QPointer>
#include <QList>
#include <QString>
#include "Nav.h"
class ConsumerVIPWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ConsumerVIPWidget(QWidget *parent = nullptr);
    ConsumerNavWidget* leftNavW;
    QWidget* rightW;
    void initRight();
public slots:
    void VIPSlot();
    void SUVIPSlot();
signals:
    void accountUpgradeSignal(QString level);
};

#endif // CONSUMERVIPWIDGET_H
