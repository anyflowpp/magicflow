#include<stdlib.h>
#include<functional>
#include<string.h>
#include<thread>
#include<chrono>
#include<functional>
#include<gtest/gtest.h>
#include<boost/dll/import.hpp>
#include<boost/dll/import_mangled.hpp>
#include"spdlog_wrap.h"
#include "testboostdll/my_plugin_api.hpp"
#include<boost/function.hpp>

TEST(boostdlltest,t1){
    setLogLevel(spdlog::level::debug);
    namespace dll = boost::dll;
    dll::shared_library alib;
    boost::dll::fs::path lib_path("D:\\megvii\\threadflow\\Megvii_threadflow\\testboostdll");
    // boost::shared_ptr<my_plugin_api> plugin;            // variable to hold a pointer to plugin variable
    std::cout << "Loading the plugin" << std::endl;
    // alib.load(lib_path,dll::load_mode::append_decorations);
    bool loaded = alib.is_loaded();
    // auto value = dll::experimental::import_mangled
    // auto plug1 = alib.get<my_namespace::my_plugin_sum>("plugin");
    auto plugin = dll::import<my_plugin_api>(          // type of imported symbol is located between `<` and `>`
        lib_path,                     // path to the library and library name
        "plugin",                                       // name of the symbol to import
        dll::load_mode::append_decorations
    );

    std::cout << "plugin->calculate(1.5, 1.5) call:  " << plugin->calculate(1.5, 1.511) << std::endl;

    typedef std::shared_ptr<my_plugin_api> (pluginapi_create_t)();
    boost::function<pluginapi_create_t> creator;

    creator = boost::dll::import_alias<pluginapi_create_t>(             // type of imported symbol must be explicitly specified
        lib_path,                                            // path to library
        "create_plugin",                                                // symbol to import
        dll::load_mode::append_decorations                              // do append extensions and prefixes
    );

    std::shared_ptr<my_plugin_api> plugin1 = creator();
    std::cout << "plugin->calculate(1.5, 1.5) call:  " << plugin1->calculate(1.5, 1.5) << std::endl;
    std::cout << "plugin->calculate(1.5, 1.5) second call:  " << plugin1->calculate(1.5, 1.5) << std::endl;
    std::cout << "Plugin Name:  " << plugin1->name().c_str() << std::endl;

};
