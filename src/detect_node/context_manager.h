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
        //TODO : ��������ʵ�ַ���context_ptr ������ʱ���Զ�����manager ��ֹһ�½���contxt��
        //��дnode��ʱ������return.
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
    //TODO: �ṩmegface��context������, ���е�contextӦ�ö��ǹ��õ�, ���flow֮����Ի���, 
    //����ҲҪ�ж������ǲ���һ��, context����ʱҪ����cpu�������������ֻ�ʹ��

    //TODO: contextmgr Ӧ�ṩ����ģʽ,��ʹ���غͶ��Լ���ģʽ,
    // ���Լ��� : ����Ӧ��������ʹ�õ�context(��Ӧһ�����͵�һ��).����ʱ��
    //     ��������, �� node��ʹ��, node��ʹ������, ���� thread_pool������, thread_pool����cpu��������.
    // ��ʱ���� : ����Ӧ������Ҫʹ�õ�����context(��Ӧ��Ҫÿ�ֽ������ٸ�������node�Ļ���buf�ж�ȡ).

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