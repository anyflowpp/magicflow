#include"feat_node.h"
#include"core/flow.h"
#include<sstream>

FLOW_REGISTER_NODE(flow_data,"feat",FeatNode::GenNode);

std::shared_ptr<FeatNode> FeatNode::GenNode(const Json::Value& cfg){
    return std::make_shared<FeatNode>(cfg);
}

FeatNode::FeatNode(const Json::Value& cfg){
    if(cfg.isMember("nodecfg")){
        setNodeCfg(cfg["nodecfg"]);
    }
    if(!cfg.isMember("type")){
        throw std::logic_error("node type error");
    }
    if(cfg["type"].asString()!="feat"){
        throw std::logic_error("node type error");
    }
	CreateThreadContext();//重写函数 建立对应的上下文关系
}

FeatNode::~FeatNode(){
	DestroyThreadContext();//重写函数 销毁对应的上下文关系
}
void * FeatNode::GetThreadContext(){
    mgf_feat_ptr mgf =nullptr ;
    std::lock_guard<std::mutex> lck(m_mutex_vector_ctx);
	static int index = 0;
	mgf = m_vector_ctx[index];
	index++;
    return  mgf->getContext();
}
void FeatNode::CreateThreadContext(){
    for(int i=0;i<m_max_thread_number;i++){
        Json::Value cfg;
        auto s1 = mgf_feat::genSetting(cfg);
        mgf_feat_ptr _ctx = std::make_shared<mgf_feat>(s1,"data/feat.faceid_boltzmann191108.conf");        
        std::lock_guard<std::mutex> lck (m_mutex_vector_ctx);
		m_vector_ctx.push_back(_ctx);
    } 
}
void FeatNode::DestroyThreadContext(){
    std::lock_guard<std::mutex> lck (m_mutex_vector_ctx);
	m_vector_ctx.clear();
}
flow_data_ptr FeatNode::NodeProcess(flow_data_ptr input , void * ctx, node_info_ptr info ){
    auto &data = input->data;
    for(auto it = data.begin();it!=data.end();++it){
        auto &track_result =(*it)->track_result; 
        auto &detect_result = (*it)->detect_result;
        if(!track_result && !detect_result){
            logw("no data can be deal");
            continue;
        }
        kag::Image mgfimg((*it)->img,kag::bgr);
        if(track_result && track_result->size>0){
            const MGFFeature *result=nullptr;
            auto& item = track_result->items[0];
            MGFImage mgfi = mgfimg.data();
            MGFErrorCode ret = MGF_extract(ctx,&mgfi,&item.landmark,
                                        1, &result);
            if(ret!=MGF_OK){
                logw("mgf deal error!:{}",ret);
                result = nullptr;
            }
            auto cResult = std::make_shared<kag::CFeatureResult>(result,1);
            (*it)->feat_result= cResult;
        }else if(detect_result && detect_result->size>0){
            const MGFFeature *result=nullptr;
            auto& item = detect_result->items[0];
            MGFImage mgfi = mgfimg.data();
            MGFErrorCode ret = MGF_extract(ctx,&mgfi,&item.landmark,
                                        1, &result);
            if(ret!=MGF_OK){
                logw("mgf deal error!:{}",ret);
                result = nullptr;
            }
            auto cResult = std::make_shared<kag::CFeatureResult>(result,1);
            (*it)->feat_result= cResult;
        }else{
            logw("not fond face");
        }
    }
    return input;
}
