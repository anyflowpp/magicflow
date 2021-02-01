#include"context_manager.h"

ContextManager_ptr ContextManager::s_instance;

mgf_detector_ptr ContextManager::GetContext(const MGFDetectorSettings& setting){
    //TODO 通过settings 和模型对比返回可复用的context
    // TODO: 多线程操作vector 存在风险, 需做加锁处理
    std::lock_guard<std::mutex> locker(m_detect_lock);
    mgf_detector_ptr ret;
    if(m_detect_contexts.size()==0){
        ret = std::make_shared<mgf_detector>(setting,"data/detector.large.v3.pf.lyb.190119.conf");
        this->m_detect_contexts.push_back(std::static_pointer_cast<mgf_context>(ret));
        return ret;
    }
    ret = std::dynamic_pointer_cast<mgf_detector>(m_detect_contexts.front());
    m_detect_contexts.pop_front();
    return ret;
}

mgf_tracker_ptr ContextManager::GetContext(const MGFTrackerSettings& setting){
    //TODO: 通过setting对比有没有相同配置的context, 有的话直接返回, 没有就建立在返回
    //TODO 通过settings 和模型对比返回可复用的context
    std::lock_guard<std::mutex> locker(m_track_lock);
    mgf_tracker_ptr ret;
    if(m_track_contexts.size()==0){
        ret = std::make_shared<mgf_tracker>(setting,"data/tracker.mobile.v4.fast.conf");
        this->m_track_contexts.push_back(std::static_pointer_cast<mgf_context>(ret));
        return ret;
    }
    ret = std::dynamic_pointer_cast<mgf_tracker>(m_track_contexts.front());
    m_track_contexts.pop_front();
    return ret;
}

mgf_feat_ptr ContextManager::GetContext(const MGFFeatureSettings& setting){
    //TODO: 通过setting对比有没有相同配置的context, 有的话直接返回, 没有就建立在返回
    //TODO 通过settings 和模型对比返回可复用的context
    std::lock_guard<std::mutex> locker(m_feat_lock);
    mgf_feat_ptr ret;
    if(m_feat_contexts.size()==0){
        ret = std::make_shared<mgf_feat>(setting,"data/feat.faceid_boltzmann191108.conf");
        return ret;
    }
    ret = std::dynamic_pointer_cast<mgf_feat>(m_feat_contexts.front());
    m_feat_contexts.pop_front();
    return ret;
}

ContextManager_w_ptr ContextManager::GetInstance(){
    if(!ContextManager::s_instance){
        ContextManager::s_instance = std::make_shared<ContextManager>();
    }
    ContextManager_w_ptr ret = ContextManager::s_instance;
    return ret;
}
void ContextManager::Shutdown(){
    this->m_track_contexts.clear();
    this->m_detect_contexts.clear();
    MGF_shutdown();
}

ContextManager::context_ptr ContextManager::GetAutoContext(const MGFDetectorSettings& setting){
    mgf_detector_ptr ctx = GetContext(setting);
    auto ret = std::make_shared<ContextManager::_context_ptr>(this,std::dynamic_pointer_cast<mgf_context>(ctx));
    return ret;
}

ContextManager::context_ptr ContextManager::GetAutoContext(const MGFFeatureSettings& setting){
    mgf_feat_ptr ctx = GetContext(setting);
    auto ret = std::make_shared<ContextManager::_context_ptr>(this,std::dynamic_pointer_cast<mgf_context>(ctx));
    return ret;
}

ContextManager::context_ptr ContextManager::GetAutoContext(const MGFTrackerSettings& setting){
    auto ctx = GetContext(setting);
    auto ret = std::make_shared<ContextManager::_context_ptr>(this,std::dynamic_pointer_cast<mgf_context>(ctx));
    return ret;
}

ContextManager::_context_ptr::_context_ptr(ContextManager* mgr, mgf_context_ptr ptr):mgr(mgr),m_ptr(ptr){

}

ContextManager::_context_ptr::~_context_ptr(){
    // TODO: 不能这样转型, 会导致性能问题, 需要记录子类类型的变量,
    // 这这里判断变量枚举值, 直接操作 list.
    // TODO: 所有的contextlist操作都封装到manager里,
    //防止其他类操作 manager的locker.
    auto type = m_ptr->type;
    switch(type){
        case mgf_context::context_type::Track:
            {
                std::lock_guard<std::mutex> locker(mgr->m_track_lock);
                mgr->m_track_contexts.push_back(m_ptr);
                break;
            }
        case mgf_context::context_type::Detect:
            {
                std::lock_guard<std::mutex> locker(mgr->m_detect_lock);
                mgr->m_detect_contexts.push_back(m_ptr);
                break;
            }
        case mgf_context::context_type::Feat:
            {
                std::lock_guard<std::mutex> locker(mgr->m_feat_lock);
                mgr->m_feat_contexts.push_back(m_ptr);
                break;
            }
        default:
            break;
    }
}

mgf_detector_ptr ContextManager::_context_ptr::get_detector(){
    return std::static_pointer_cast<mgf_detector>(m_ptr);
}

mgf_tracker_ptr ContextManager::_context_ptr::get_tracker(){
    return std::static_pointer_cast<mgf_tracker>(m_ptr);
}

mgf_feat_ptr ContextManager::_context_ptr::get_feat(){
    return std::static_pointer_cast<mgf_feat>(m_ptr);
}