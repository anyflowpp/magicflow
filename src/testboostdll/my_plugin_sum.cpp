#include"my_plugin_sum.h"
#include"spdlog_wrap.h"

using namespace my_namespace; 

my_plugin_sum::my_plugin_sum() {
    std::cout << "Constructing my_plugin_sum" << std::endl;
}

std::string my_plugin_sum::name() const {
    return "sum";
}

float my_plugin_sum::calculate(float x, float y) {
    logw("ddd");
    return x + y;
}

std::shared_ptr<my_plugin_sum> my_plugin_sum::create() {
    std::shared_ptr<my_plugin_sum> ret(new my_plugin_sum);
    return ret;
}

my_plugin_sum::~my_plugin_sum() {
    std::cout << "Destructing my_plugin_sum ;o)" << std::endl;
}

namespace my_namespace{
my_plugin_sum plugin;
}