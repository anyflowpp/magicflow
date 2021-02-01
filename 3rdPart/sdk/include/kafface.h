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

template<class C, class R>
class T {
public:
    T():ctx(NULL), result(NULL) { }
    virtual ~T() {
        if (NULL != ctx) {
            MGF_release(ctx);
        }
    }
    const R * current() const {
        return result;
    }
protected:
    C ctx;
    const R *result;
};

class Detector: public T<MGFDetectorContext, MGFDetectorResult> {
public:
    MGFErrorCode load(const char *model, int min_face);
    MGFErrorCode detect(MGFImage image);
protected:
    MGFDetectorOptions options;
};

class Tracker: public T<MGFTrackerContext, MGFTrackerResult> {
public:
    MGFErrorCode load(const char *model);
    MGFErrorCode track(MGFImage image); 
};

class kagQuality: public T<MGFQualityContext, MGFQuality> {
public:
    MGFErrorCode load(const char *model);
    MGFErrorCode predict(MGFImage image, const MGFLandmarkMap *landmark, int batch_size = 1);
};

class Pose: public T<MGFPoseContext, MGFPose> {
public:
    MGFErrorCode load(const char *model);
    MGFErrorCode predict(MGFImage image, const MGFLandmarkMap *landmark, int batch_size = 1);
};

class FMP: public T<MGLFMPContext, MGLFMPResult> {
public:
    MGFErrorCode load(const char *model);
    MGFErrorCode classify(MGFImage image, const MGFLandmarkMap *landmark, int batch_size = 1);
};

class Rgbir: public T<MGLRGBIRContext, MGLRGBIRResult> {
public:
    MGFErrorCode load(const char *model);
    MGFErrorCode classify(MGFImage rgb, const MGFLandmarkMap *rgb_lmks, MGFImage ir, const MGFLandmarkMap *ir_lmks, int batch_size = 1);
};

class LandmarkAttr: public T<MGFLandmarkAttrContext, MGFLandmarkAttribute> {
public:
    MGFErrorCode load(const char *model);
    MGFErrorCode predict(MGFImage image, const MGFLandmarkMap *landmark, int batch_size = 1);
};

class Feature: public T<MGFFeatureContext, MGFFeature> {
public:
    MGFErrorCode load(const char *model);
    MGFErrorCode extract(MGFImage image, const MGFLandmarkMap *landmark, int batch_size = 1);
    MGFErrorCode compare(const MGFFeature *a, const MGFFeature *b);
    MGFErrorCode threshold();
    MGFComparisonAlgorithm get_comparison_algorithm(); 
    MGFFeatureScoreThresholds feature_threshold() const {
        return threshold_value;
    }

    float score() const{
        return score_value;
    }
    size_t size;
protected:
    float score_value;
    MGFFeatureScoreThresholds threshold_value;
};

class PoseBlur: public T<MGFPoseBlurContext, MGFPoseBlur> {
public:
    MGFErrorCode load(const char *model);
    MGFErrorCode predict(MGFImage image, const MGFScoreRect* landmark, int batch_size = 1);
};
#endif // __KAG_KAFFACE_H__
