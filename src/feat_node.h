#pragma once
#include"common/rootnode.hpp"
#include<json/json.h>
#include<megface-c/tracker_context.h>
#include"spdlog_wrap.h"
#include"mgf_context.h"
#include"context_manager.h"
class FeatNode:public RootNode{
public:
    FeatNode(const Json::Value& cfg);
    virtual ~FeatNode();
    MGFFeatureSettings setting;
    static std::shared_ptr<FeatNode> GenNode(const Json::Value& cfg);
    std::vector<mgf_feat_ptr>   m_vector_ctx;
    std::mutex					m_mutex_vector_ctx;
protected:
    virtual flow_data_ptr NodeProcess(flow_data_ptr input,void* ctx, node_info_ptr info);
    virtual void CreateThreadContext();//重写函数 建立对应的上下文关系
    virtual void DestroyThreadContext();//重写函数 销毁对应的上下文关系
    virtual void * GetThreadContext();//返回上下文
};