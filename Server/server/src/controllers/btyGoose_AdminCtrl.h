#pragma once

#include <drogon/HttpController.h>
#include "SharedResources.h"
using namespace drogon;

namespace btyGoose
{
class AdminCtrl : public drogon::HttpController<AdminCtrl>
{
  public:
    METHOD_LIST_BEGIN
    // use METHOD_ADD to add your custom processing function here;
    // METHOD_ADD(AdminCtrl::get, "/{2}/{1}", Get); // path is /btyGoose/AdminCtrl/{arg2}/{arg1}
    // METHOD_ADD(AdminCtrl::your_method_name, "/{1}/{2}/list", Get); // path is /btyGoose/AdminCtrl/{arg1}/{arg2}/list
    // ADD_METHOD_TO(AdminCtrl::your_method_name, "/absolute/path/{1}/{2}/list", Get); // path is /absolute/path/{arg1}/{arg2}/list

    METHOD_LIST_END
    
};
}
