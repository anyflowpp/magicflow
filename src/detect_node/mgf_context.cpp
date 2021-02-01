#include <iostream>
#include "mgf_context.h"
#include"spdlog_wrap.h"


static int max_cpuid = 8;
mgf_context::mgf_context():ctx(NULL),type(None){ }
mgf_context::~mgf_context() {
    if(ctx){
        MGF_release(ctx);
    }
}

mgf_detector::mgf_detector(const MGFDetectorSettings& setting,const std::string& model){
    this->type = context_type::Detect;
    MGFErrorCode ret = MGF_make_detector_context_from_file(model.c_str(), &setting, &ctx);
    if(ret!=MGF_OK){
        ctx=nullptr;
        throw std::logic_error("mgf detector make error");
    }
}
MGFDetectorSettings mgf_detector::genSetting(const Json::Value& cfg){
    MGFDetectorSettings ret;
    std::memset(&ret,0,sizeof(ret));
    // ret.device.dev_id = id++;
    static int s_id = 0;
    ret.device.dev_id = s_id%max_cpuid;
    s_id++;
    return ret;
}

MGFDetectorContext mgf_detector::getContext() {
    return (MGFDetectorContext)ctx;
}

MGFTrackerContext mgf_tracker::getContext(){
    return (MGFTrackerContext)ctx;
}

MGFTrackerSettings mgf_tracker::genSetting(const Json::Value& cfg){
    //TODO:完善setting的设置读取
    MGFTrackerSettings settings {
        MGF_SYNC_DETECTOR_TRACKER,
    };
    MGFDeviceOption opt{MGF_CPU, 0, 0};
    static int s_id = 0;
    opt.dev_id = s_id%max_cpuid;
    s_id++;
    settings.sync = MGFSyncDetectorTracker {
            opt,
            MGFTrackerDetectorOptions { MGFRect {0,0,0,0}, 80, MGF_UP, 0.9f },
            3,
            1,
            1,
            3,
    };
    return settings;
}
mgf_tracker::mgf_tracker(const MGFTrackerSettings& setting,const std::string& model):setting(setting){
    this->type=context_type::Track;
    auto ret = MGF_make_tracker_context_from_file(model.c_str(),&this->setting,&ctx);
    if(ret!=MGF_OK){
        loge("track load error:{}",ret);
        ctx = nullptr;
        throw std::runtime_error("megface load model error");
    }
}

MGFFeatureContext mgf_feat::getContext(){
    return (MGFFeatureContext)ctx;
}

MGFFeatureSettings mgf_feat::genSetting(const Json::Value& cfg){
    MGFFeatureSettings settings;
    std::memset(&settings,0,sizeof(settings));
    static int s_id = 0;
    settings.device.dev_id=s_id%max_cpuid;
    s_id++;
#ifdef ebg_v2_13_0_7
#else
    settings.device.fast_run=true;
#endif
    return settings;
}
mgf_feat::mgf_feat(const MGFFeatureSettings& setting,const std::string& model):setting(setting){
    this->type=context_type::Feat;
    auto ret = MGF_make_feature_context_from_file(model.c_str(),&this->setting,&ctx);
    if(ret!=MGF_OK){
        loge("LOAD ERROR:{}",ret);
        ctx = nullptr;
        throw std::runtime_error("MEGFACE LOAD MODEL ERROR");
    }
}

MGFLandmarkAttrContext mgf_attr::getContext(){
    return (MGFLandmarkAttrContext)ctx;
}

MGFLandmarkAttrSettings mgf_attr::genSetting(const Json::Value& cfg){
    MGFLandmarkAttrSettings settings;
    std::memset(&settings,0,sizeof(settings));
    static int s_id = 0;
    settings.device.dev_id=s_id%max_cpuid;
    s_id++;
    #ifdef ebg_v2_13_0_7
    #else
    settings.device.fast_run=true;
    #endif
    return settings;
}
mgf_attr::mgf_attr(const MGFLandmarkAttrSettings& setting,const std::string& model):setting(setting){
    this->type=context_type::Feat;
    auto ret = MGF_make_landmark_attr_context_from_file(model.c_str(),&this->setting,&ctx);
    if(ret!=MGF_OK){
        loge("LOAD ERROR:{}",ret);
        ctx = nullptr;
        throw std::runtime_error("MEGFACE LOAD MODEL ERROR");
    }
}

MGLFMPContext mgf_fmp::getContext(){
    return (MGLFMPContext)ctx;
}

MGLFMPSettings mgf_fmp::genSetting(const Json::Value& cfg){
    MGLFMPSettings settings;
    std::memset(&settings,0,sizeof(settings));
    static int s_id = 0;
    settings.device.dev_id=s_id%max_cpuid;
    s_id++;
    #ifdef ebg_v2_13_0_7
    #else
    settings.device.fast_run=true;
    #endif
    return settings;
}
mgf_fmp::mgf_fmp(const MGLFMPSettings & setting,const std::string& model):setting(setting){
    this->type=context_type::Feat;
    auto ret = MGL_make_fmp_context_from_file(model.c_str(),&this->setting,&ctx);
    if(ret!=MGF_OK){
        loge("LOAD ERROR:{}",ret);
        ctx = nullptr;
        throw std::runtime_error("MEGFACE LOAD MODEL ERROR");
    }
}

MGLRGBIRContext mgf_rgbir::getContext(){
    return (MGLRGBIRContext)ctx;
}

MGLRGBIRSettings mgf_rgbir::genSetting(const Json::Value& cfg){
    MGLRGBIRSettings settings;
    std::memset(&settings,0,sizeof(settings));
    static int s_id = 0;
    settings.device.dev_id=s_id%max_cpuid;
    s_id++;
    #ifdef ebg_v2_13_0_7
    #else
    settings.device.fast_run=true;
    #endif
    return settings;
}
mgf_rgbir::mgf_rgbir(const MGLRGBIRSettings & setting,const std::string& model):setting(setting){
    this->type=context_type::Feat;
    auto ret = MGL_make_rgb_ir_context_from_file(model.c_str(),&this->setting,&ctx);
    if(ret!=MGF_OK){
        loge("LOAD ERROR:{}",ret);
        ctx = nullptr;
        throw std::runtime_error("MEGFACE LOAD MODEL ERROR");
    }
}