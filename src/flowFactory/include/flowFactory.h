#pragma once
#include<core/libmagicflow/include/magicflow.h>
#include"core/jsoncpp/include/json/json.h"
#include<vector>
#include<core/common/magic_func.h>
#include<boost/function.hpp>

class MAGIC_E_FUNC flowFactory{
public:
    ~flowFactory();
    static std::shared_ptr<flow> GenFlowFromJsonFile(std::string file);
    static flow::RootNode_ptr NodeGenner(const Json::Value &cfg);
    typedef std::shared_ptr<node_exec> (pluginapi_create_t)();
    typedef boost::function<flowFactory::pluginapi_create_t> boostfunc;
    static std::vector<boostfunc> creators;
};