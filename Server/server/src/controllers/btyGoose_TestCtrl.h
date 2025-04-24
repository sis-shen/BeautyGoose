#pragma once

#include <drogon/HttpController.h>
#include "SharedResources.h"

using namespace drogon;

namespace btyGoose
{
class TestCtrl : public drogon::HttpController<TestCtrl>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(TestCtrl::ping,"/ping",Get);
    ADD_METHOD_TO(TestCtrl::flushAll,"/flush/redis",Get);
    // ADD_METHOD_TO(TestCtrl::your_method_name, "/absolute/path/{1}/{2}/list", Get); // path is /absolute/path/{arg1}/{arg2}/list

    METHOD_LIST_END
    void ping(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
    {
      auto resp = drogon::HttpResponse::newHttpResponse();
      resp->setStatusCode(k200OK);
      resp->setContentTypeCode(CT_TEXT_HTML);
      resp->setBody("pong");
      callback(resp);    
    }
    void flushAll(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
    {


      auto resp = drogon::HttpResponse::newHttpResponse();
      resp->setStatusCode(k200OK);
      resp->setContentTypeCode(CT_TEXT_HTML);
      resp->setBody("flush redis success");
      SR::ins().redis()->flushall();
      callback(resp);    
    }
};
}
