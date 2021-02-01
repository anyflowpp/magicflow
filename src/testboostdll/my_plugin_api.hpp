#pragma once
#include <boost/config.hpp>
#include <string>
#include<common/rootnode.hpp>

// class __declspec(dllexport) my_plugin_api {
class BOOST_SYMBOL_VISIBLE my_plugin_api :public RootNode {
public:
   virtual std::string name() const = 0;
   virtual float calculate(float x, float y) = 0;

   virtual ~my_plugin_api() {};
};