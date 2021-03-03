#pragma once
#include"core/node.h"
#include"flowtype.hpp"
#include<json/json.h>

class RootNode:public flow<flow_data>::Node{
public:
    void setNodeCfg(const Json::Value& cfg) {
        int buf_num=cfg.get("max_input_buf_number",2).asInt();
        SetInputBufNum(buf_num);
        if(cfg.get("run_mode",0).asInt()==0){
            SetThreadRunMode(thread_mode::shared);
        }else{
            SetThreadRunMode(thread_mode::holdon);
        }
        int thread_number= cfg.get("max_thread_number",2).asInt();
        SetThreadNum(thread_number);
    }
    RootNode(){}
protected:
    virtual flow_data_ptr NodeProcess(flow_data_ptr input,void* ctx, node_info_ptr info){return input;}
};

typedef std::shared_ptr<RootNode> RootNode_ptr;