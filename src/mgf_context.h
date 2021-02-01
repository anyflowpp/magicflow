#pragma once
#ifndef __KAG_KAFFACE_H__
#define __KAG_KAFFACE_H__

//#include "megface-c/license.h"
#include "megface-c/megface.h"
#include "meglive-c/fmp_context.h"
#include "megface-c/pose_context.h"
#include "meglive-c/rgb_ir_fmp_context.h"
#include "megface-c/landmark_attr_context.h"
#include "megface-c/pose_blur_context.h"
#include "kafutils.h"
#include "megface-c/dognose_context.h"
#include<memory>
#include<json/json.h>
#include<megface-c/feature_context.h>

class mgf_context {
public:
    typedef enum{
        None,
        Track,
        Detect,
        Feat,
    }context_type;

    mgf_context();
    virtual ~mgf_context();
    context_type type;
protected:
    void* ctx;
};
typedef std::shared_ptr<mgf_context> mgf_context_ptr;

class mgf_detector: public mgf_context{
public:
    // mgf_detector(const MGFDetectorSettings &setting);
    //TODO: 实现构造
    mgf_detector(const MGFDetectorSettings& setting,const std::string& model);
    MGFDetectorContext getContext();
    static MGFDetectorSettings genSetting(const Json::Value& cfg);
    MGFDetectorSettings setting;
protected:
    MGFDetectorOptions options;
};
typedef std::shared_ptr<mgf_detector> mgf_detector_ptr;

class mgf_tracker: public mgf_context{
public:
    mgf_tracker(const MGFTrackerSettings& setting,const std::string& model);
    MGFTrackerContext getContext();
    MGFTrackerSettings setting;
    static MGFTrackerSettings genSetting(const Json::Value& cfg);
protected:
    MGFTrackerOptions options;
};
typedef std::shared_ptr<mgf_tracker> mgf_tracker_ptr;

class mgf_feat: public mgf_context{
public:
    mgf_feat(const MGFFeatureSettings& setting,const std::string& model);
    MGFFeatureContext getContext();
    MGFFeatureSettings setting;
    static MGFFeatureSettings genSetting(const Json::Value& cfg);
protected:
};
typedef std::shared_ptr<mgf_feat> mgf_feat_ptr;

class mgf_attr: public mgf_context{
public:
    mgf_attr(const MGFLandmarkAttrSettings& setting,const std::string& model);
    MGFLandmarkAttrContext getContext();
    MGFLandmarkAttrSettings setting;
    static MGFLandmarkAttrSettings genSetting(const Json::Value& cfg);
protected:
};
typedef std::shared_ptr<mgf_attr> mgf_attr_ptr;

class mgf_fmp: public mgf_context{
public:
    mgf_fmp(const MGLFMPSettings &setting,const std::string& model);
    MGLFMPContext getContext();
    MGLFMPSettings setting;
    static MGLFMPSettings genSetting(const Json::Value& cfg);
protected:
};
typedef std::shared_ptr<mgf_fmp> mgf_fmp_ptr;

class mgf_rgbir: public mgf_context{
public:
    mgf_rgbir(const MGLRGBIRSettings &setting,const std::string& model);
    MGLRGBIRContext getContext();
    MGLRGBIRSettings setting;
    static MGLRGBIRSettings genSetting(const Json::Value& cfg);
protected:
};
typedef std::shared_ptr<mgf_rgbir> mgf_rgbir_ptr;
#endif // __KAG_KAFFACE_H__
