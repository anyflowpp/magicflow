#include"detect_node.h"
#include"spdlog_wrap.h"
#include"mgf_context.h"
#include"context_manager.h"
#include"core/flow.h"
#include<mutex>

using namespace kag;

std::shared_ptr<DetectNode> DetectNode::GenNode(const Json::Value& cfg){
    return std::make_shared<DetectNode>(cfg);
}

flow_data_ptr DetectNode::NodeProcess(flow_data_ptr input,void* ctx, node_info_ptr info){
    logd("");
        static int i = 0;
        static std::mutex mtx;
        if(i<100){
            i++;
            cv::Mat oneimg = cv::imread("rgb.face.jpg");
            auto id = flow_data::GenDataWithImg(std::make_shared<cv::Mat>(oneimg));
            auto fd = std::make_shared<Node_Info>();
            // genInput(id,fd);
        }
    auto cm = ContextManager::GetInstance().lock();
    // mgf_detector_ptr detector_ctx = cm->GetContext(this->setting);
    Json::Value cfg;
    mgf_detector_ptr detector_ctx = cm->GetAutoContext(mgf_detector::genSetting(cfg))->get_detector();
    // mgf_detector_ptr detector_ctx = cm->GetAutoContext(this->setting)->get_detector();
    auto _ctx =  detector_ctx->getContext();
    auto &data = input->data;
    for(auto it = data.begin();it!=data.end();++it){
        if(!(*it)->img){
            continue;
        }
        kag::Image mgfimg((*it)->img,kag::bgr);
        const MGFDetectorResult *result=nullptr;
        MGFErrorCode ret = MGF_detect(_ctx,mgfimg.data(),nullptr,&result);
        if(ret!=MGF_OK){
            logw("mgf deal error!:{}",ret);
            result = nullptr;
        }
        auto cResult = std::make_shared<kag::CMGFDetectorResult>(result);
        (*it)->detect_result = cResult;
    }
    // std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return input;
}

DetectNode::DetectNode(const Json::Value& cfg){
    if(cfg.isMember("nodecfg")){
        setNodeCfg(cfg["nodecfg"]);
    }
    if(!cfg.isMember("type")){
        throw std::logic_error("node type error");
    }
    if(cfg["type"].asString()!="detect"){
        throw std::logic_error("node type error");
    }
}

DetectNode::~DetectNode(){
}
flow_data_ptr DetectNode::NodeProcessBack(flow_data_ptr input,node_info_ptr info){
    logi("");
    return input;
}