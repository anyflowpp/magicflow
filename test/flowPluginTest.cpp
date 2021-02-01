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
#include "common/rootnode.hpp"
#include"feat_node.h"
#include<boost/function.hpp>

typedef flow<flow_data> mgfflow;
typedef std::shared_ptr<mgfflow> mgfflow_ptr;
static inline void callback(flow_data_ptr output, node_info_ptr info){
    auto el = output->data[0];
    static int i = 0;
    i++;
    logi("alloutput count:{}",i);
    if(el->track_result) logi("track face num:{}",el->track_result->items.size());
    if(el->detect_result) logi("detect face num:{}",el->detect_result->items.size());
    if(el->feat_result){ logi("every feat length :{}",el->feat_result->items[0].size);}
}

typedef std::shared_ptr<RootNode> (pluginapi_create_t)(const Json::Value& cfg);
typedef boost::function<pluginapi_create_t> crator_type;
static crator_type creator;
static std::map<std::string, crator_type> gennodes_funcs;
void GetCreater(){
    boost::dll::fs::path lib_path("detect");
    creator = boost::dll::import_alias<pluginapi_create_t>(             // type of imported symbol must be explicitly specified
        lib_path,                                            // path to library
        "create_plugin",                                                // symbol to import
        boost::dll::load_mode::append_decorations                              // do append extensions and prefixes
    );
    gennodes_funcs.insert({"detect",creator});
}

RootNode_ptr outGenner(const Json::Value& jnode){
    RootNode_ptr ret;
    std::string nodetype = jnode["type"].asString();
    if(gennodes_funcs.find(nodetype)!=gennodes_funcs.end()){
        ret = gennodes_funcs[nodetype](jnode);
    }else{
        logw("type:{} not fount",nodetype);
    }
    return ret;
}

RootNode_ptr lamdacreater(const Json::Value& cfg){
    return creator(cfg);
}

TEST(flowplugin,t1){
    setLogLevel(spdlog::level::debug);
    auto &g = flow_thread_pool::GetInstance();
    int cpus = g.getCpuCount();
    logi("cpu count:{}",cpus);
    Json::Value root;
    Json::Reader jsonread;
    std::fstream fin("./flow1.json",std::ios::in);
    jsonread.parse(fin,root);

    Json::Value detectcfg;
    std::fstream fin1("./flow1.json",std::ios::in);
    jsonread.parse(fin1,detectcfg);
    auto detectf = detectcfg["flow"].begin();
    GetCreater();
    gennodes_funcs.insert({"feat",std::bind(&FeatNode::GenNode,std::placeholders::_1)});
    // FLOW_REGISTER_NODE(flow_data,"detect",lamdacreater);
    auto flow = mgfflow::GetInstance(root,outGenner);
    // flow->m_nodes.push_back(detectNode);
    flow->SetCfg(flowcfg(flowcfg::async,flowcfg::sync));
    flow->SetCallBack(callback);
    cv::Mat oneimg = cv::imread("rgb.face.jpg");
    for(int i = 0;i<1000;i++){
        logi("i = {}",i);
        auto rgb = std::make_shared<cv::Mat>(oneimg);
		node_info_ptr info = std::make_shared<Node_Info>();
        flow_data_ptr input = flow_data::GenDataWithImg(rgb);
        flow->SetInput(input,info);
        if(i%1000==0){
        //    auto out =  flow->GetOutput();
        }
    }
};