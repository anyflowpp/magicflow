#pragma once
#include <boost/config.hpp> // for BOOST_SYMBOL_EXPORT
#include<boost/dll/alias.hpp>
#include "my_plugin_api.hpp"
#include<iostream>
#include<memory>

namespace my_namespace {

class my_plugin_sum : public my_plugin_api {
public:
    my_plugin_sum(); 
    std::string name() const ;
    float calculate(float x, float y) ;
    virtual ~my_plugin_sum() ;
    static std::shared_ptr<my_plugin_sum> create();
};

// Exporting `my_namespace::plugin` variable with alias name `plugin`
// (Has the same effect as `BOOST_DLL_ALIAS(my_namespace::plugin, plugin)`)
extern "C" BOOST_SYMBOL_EXPORT my_plugin_sum plugin;

BOOST_DLL_ALIAS(
    my_namespace::my_plugin_sum::create, // <-- this function is exported with...
    create_plugin                               // <-- ...this alias name
)

} // namespace my_namespace
