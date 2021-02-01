#pragma once
#include"mgf_context.h"
#include"json/json.h"
#include<list>
#include<mutex>

class ContextManager;
typedef std::weak_ptr<ContextManager> ContextManager_w_ptr;
typedef std::shared_ptr<ContextManager> ContextManager_ptr;

class ContextManager{

    class _context_ptr{
        //TODO : 这里类里实现返回context_ptr 析构的时候自动换回manager 防止一致建立contxt和
        //编写node的时候忘记return.
        public:
            _context_ptr(ContextManager* manager,mgf_context_ptr ptr);
            mgf_detector_ptr get_detector();
            mgf_tracker_ptr get_tracker();
            mgf_feat_ptr get_feat();
            virtual ~_context_ptr();
        private:
            mgf_context_ptr m_ptr;
            ContextManager* mgr;
    };
    typedef std::shared_ptr<_context_ptr> context_ptr;
    //TODO: 提供megface的context管理功能, 所有的context应该都是公用的, 多个flow之间可以互用, 
    //但是也要判断设置是不是一样, context生成时要根据cpu个数依次增加轮换使用

    //TODO: contextmgr 应提供两种模式,即使加载和惰性加载模式,
    // 惰性加载 : 至少应建立最少使用的context(对应一种类型的一个).不够时再
    //     继续建立, 公 node们使用, node的使用上限, 会受 thread_pool的限制, thread_pool会收cpu个数限制.
    // 即时加载 : 至少应建立需要使用的所有context(对应需要每种建立多少个在配置node的缓存buf中读取).

public:
    static ContextManager_w_ptr GetInstance();
    mgf_tracker_ptr GetContext(const MGFTrackerSettings& setting);
    mgf_detector_ptr GetContext(const MGFDetectorSettings& setting);
    mgf_feat_ptr GetContext(const MGFFeatureSettings& setting);

    context_ptr GetAutoContext(const MGFDetectorSettings& setting);
    context_ptr GetAutoContext(const MGFTrackerSettings& setting);
    context_ptr GetAutoContext(const MGFFeatureSettings& setting);
    void Shutdown();
private:
    // std::vector<mgf_tracker_ptr> m_track_contexts;
    // std::vector<mgf_detector_ptr> m_detect_contexts;
    std::list<mgf_context_ptr> m_track_contexts;
    std::mutex m_track_lock;
    std::list<mgf_context_ptr> m_detect_contexts;
    std::mutex m_detect_lock;
    std::list<mgf_context_ptr> m_feat_contexts;
    std::mutex m_feat_lock;
    static ContextManager_ptr s_instance;
};