#include"../include/flowFactory.h"
#include<boost/dll/import.hpp>
#include<boost/function.hpp>
#include<iostream>
#include<fstream>
#include"spdlog_wrap.h"

std::vector<flowFactory::boostfunc> flowFactory::creators;

std::shared_ptr<flow> flowFactory::GenFlowFromJsonFile(std::string file){
    Json::Reader jread;
    Json::Value root;
    try{
        std::fstream fin(file,std::ios::in);
        jread.parse(fin,root);
    }catch(Json::Exception& e){
        loge("json parse error:{}",e.what());
    }

    auto retflow = std::make_shared<flow>(root,flowFactory::NodeGenner);
    return retflow;
}

flow::RootNode_ptr flowFactory::NodeGenner(const Json::Value &cfg){
    std::string nodetype = cfg["type"].asString();

    boost::function<pluginapi_create_t> creator;
    boost::dll::fs::path shared_library_path  = "./" + nodetype;
    creator = boost::dll::import_alias<pluginapi_create_t>(             // type of imported symbol must be explicitly specified
        shared_library_path,                                            // path to library
        "create_node",                                                // symbol to import
        boost::dll::load_mode::append_decorations                              // do append extensions and prefixes
    );

    flowFactory::creators.push_back(creator);

    auto mynode_ptr = creator();
    return std::make_shared<Node>(mynode_ptr);
}