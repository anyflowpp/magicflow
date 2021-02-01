#include"track_node.h"
#include"spdlog_wrap.h"
#include"mgf_context.h"
#include"context_manager.h"
#include"core/flow.h"

FLOW_REGISTER_NODE(flow_data,"track",TrackNode::GenNode);

std::shared_ptr<TrackNode> TrackNode::GenNode(const Json::Value& cfg){
    return std::make_shared<TrackNode>(cfg);
}

flow_data_ptr TrackNode::NodeProcess(flow_data_ptr input,void* ctx, node_info_ptr info){
    //TODO: 实现处理函数
    logd("");
    //TODO: 使用智能还context的类接受GetContext的内容, 防止不还context的情况
    auto cm = ContextManager::GetInstance().lock();
    // mgf_tracker_ptr tracker_ctx = cm->GetAutoContext(this->setting)->get_tracker();
    Json::Value cfg;
    mgf_tracker_ptr tracker_ctx = cm->GetAutoContext(mgf_tracker::genSetting(cfg))->get_tracker();
    MGFTrackerContext _ctx =  tracker_ctx->getContext();
    auto& data = input->data;
    for(auto it=data.begin();it!=data.end();++it){
        kag::Image mgfimg((*it)->img,kag::bgr);
        const MGFTrackerResult *result=nullptr;
        MGFErrorCode ret = MGF_track_frame(_ctx,mgfimg.data(),&result);
        if(ret!=MGF_OK){
            logw("mgf deal error!:{}",ret);
            result = nullptr;
        }
        auto trackResult = std::make_shared<kag::CMGFTrackerResult>(result);
        (*it)->track_result=trackResult;
    }
    // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    return input;
}

TrackNode::TrackNode(const Json::Value& cfg){
    if(cfg.isMember("nodecfg")){
        setNodeCfg(cfg["nodecfg"]);
    }
    if(!cfg.isMember("type")){
        throw std::logic_error("node type error");
    }
    if(cfg["type"].asString()!="track"){
        throw std::logic_error("node type error");
    }
}

TrackNode::~TrackNode(){
}