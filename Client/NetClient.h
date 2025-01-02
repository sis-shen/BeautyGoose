#ifndef NETCLIENT_H
#define NETCLIENT_H

#include <QObject>

namespace network
{
class NetClient : public QObject
{
    Q_OBJECT
public:
    explicit NetClient(QObject *parent = nullptr);

signals:
};
}
#endif // NETCLIENT_H
