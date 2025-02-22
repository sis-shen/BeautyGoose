#include "HTTPServer.hpp"

int main()
{
    btyGoose::HTTPServer::getInstance()->start();
    return 0;
}