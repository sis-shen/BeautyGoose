#include <QtCore/QCoreApplication>
#include "HTTPServer.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    btyGoose::HTTPServer svr;
    return a.exec();
}
