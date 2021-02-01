#pragma once
#pragma warning(push)
#pragma warning(disable:4251)
#ifndef __KAG_SDK_H__
#define __KAG_SDK_H__

#include <memory>
#include<vector>
#include <mutex>
#include <atomic>
#include "kafutils.h"
#include<opencv2/opencv.hpp>
#include<megface-c/megface.h>
#include<meglive-c/rgb_ir_fmp_context.h>
#include<meglive-c/fmp_context.h>
namespace kag {

typedef void * MGFImage;
typedef int MGFErrorCode;

typedef enum COLOR {
    bgr  = 0,
    gray = 1,
} COLOR;

template class KAGAPI std::shared_ptr<cv::Mat>;
class KAGAPI Image {
public:
    Image(const char *data, int width, int height, COLOR color = COLOR::bgr);
    Image(std::shared_ptr<cv::Mat> img, COLOR color = COLOR::bgr,double time = 0.0f);
    ~Image();
    MGFImage data() {
        return ctx;
    }
    std::shared_ptr<cv::Mat> getimg(){
        return this->img;
    }
    std::shared_ptr<cv::Mat> bgr;
    std::shared_ptr<cv::Mat> gray;
	std::shared_ptr<cv::Mat> img;
    double time;
private:
    MGFImage ctx;
};

template class KAGAPI std::shared_ptr<Image>;
template class KAGAPI std::vector<MGFTrackedFace>;
class KAGAPI CMGFTrackerResult {
private:
    bool isSort;
    static bool comp(const MGFTrackedFace &a, const MGFTrackedFace &b);
public:
    std::vector<MGFTrackedFace> items;
    size_t size;
	CMGFTrackerResult();
	~CMGFTrackerResult();
	CMGFTrackerResult(const MGFTrackerResult *p,std::shared_ptr<Image> img = nullptr);
	CMGFTrackerResult(std::shared_ptr<CMGFTrackerResult> t,int index = -1);
    std::shared_ptr<Image> img;
    void Sort();
};

template class KAGAPI std::vector<MGFDetectedFace>;
template class KAGAPI std::vector<MGFDetectedFace, std::allocator<MGFDetectedFace>>;
class KAGAPI CMGFDetectorResult{
private:
    static bool comp(const MGFDetectedFace &a, const MGFDetectedFace &b);
public:
    std::vector<MGFDetectedFace> items;
    size_t size;
    std::shared_ptr<Image> img;
    void Sort();
    CMGFDetectorResult();
    CMGFDetectorResult(const MGFDetectorResult* p, std::shared_ptr<Image> img = nullptr);
	CMGFDetectorResult(std::shared_ptr<CMGFDetectorResult> t,int index = -1);
    ~CMGFDetectorResult();
};

template class KAGAPI std::vector<MGFFeature>;
class KAGAPI CFeatureResult {
public:
    size_t size;
    std::vector<MGFFeature> items;
    CFeatureResult();
    CFeatureResult(const MGFFeature* p, size_t n);
    ~CFeatureResult();
};

class KAGAPI CMGLRGBIRResult{
public:
    size_t size;
    std::vector<MGLRGBIRResult> items;
    CMGLRGBIRResult();
    CMGLRGBIRResult(const MGLRGBIRResult* const p, size_t size);
    ~CMGLRGBIRResult();
};

class KAGAPI MGFIoU { 
public: 
    static float iou(const MGFRect * a, const MGFRect * b);
};

template class KAGAPI std::vector<MGFLandmarkAttribute>;
class KAGAPI CPoseBlur {
public:
    CPoseBlur();
    CPoseBlur(const MGFLandmarkAttribute* const p, int size);
    ~CPoseBlur();
    int size;
    std::vector<MGFLandmarkAttribute> items;
};

class KAGAPI CFMPResult {
public:
    CFMPResult();
    CFMPResult(const MGLFMPResult* const p, size_t size);
    ~CFMPResult();
    size_t size;
    std::vector<MGLFMPResult> items;
};

typedef struct Rect {
    int left;
    int top;
    int right;
    int bottom; } KAGAPI Rect;

typedef enum ResultType { 
    LIVENSS_RESULT     = 0, 
    TRACK_RESULT       = 1, 
    DETECT_RESULT      = 2,
    VERIFY_RESULT      = 10,
    THRESHOLD_RESULT   = 20,
} ResultType;

typedef struct LivenessResult {
    int   face;        // rgb face num;
    float attack;      // rgbir classify result;
    float quality;     // rgb quality;
    float similarity;  // rgb ir rect Iou;
    Rect  rgb;
    Rect  ir;
} KAGAPI LivenessResult;

typedef struct Status {
    float eye_left;
    float eye_right;
    float mouth;
} KAGAPI Status;

typedef struct Occlusion {
    float eye_left;
    float eye_right;
    float mouth;
} KAGAPI Occlusion;

typedef struct FacePose {
    float roll;  // Radius around nose
    float pitch; // Radius up and down
    float yaw;   // Radius left and right
} KAGAPI FacePose;

typedef struct DetectResult {
    int       face;       // face num;
    float     quality;
    FacePose  pose;
    Status    status;
    Occlusion occlusion;
    Rect      rect;
} KAGAPI DetectResult;

typedef struct VerifyResult {
    float score;
} KAGAPI VerifyResult;

typedef struct ThresholdResult {
    float hundredth;
    float thousandth;
    float ten_thousandth;
    float hundred_thousandth;
    float millionth;
} KAGAPI ThresholdResult;

typedef struct Result {
    MGFErrorCode   code;
    ResultType     type;
    union {
        LivenessResult  liveness;
        DetectResult    detect;
        VerifyResult    verify; 
        ThresholdResult threshold;
        // TrackResult    track;
    };
} KAGAPI Result;

class KAGAPI Event {
public:
    Event();
    virtual ~Event();
    virtual void onTrackerPosInfo(std::shared_ptr<CMGFTrackerResult> ts,std::shared_ptr<Image> img) const = 0;

    void  refresh() const;
    float threshold() const;
protected:
    float m_threshold;
};

template struct KAGAPI std::atomic<bool>;
class KAGAPI std::mutex;

#define SDK_FUNC_PRE {\
    if(m_init_thread && m_init_thread->joinable()){\
        m_init_thread->join();\
    }\
}

class KAGAPI SDK {
typedef std::mutex Mutex;
typedef std::lock_guard<Mutex> Guard;
public:
    SDK(const char * model_dir = ".", int min_face= 50, Event *event = NULL);
    virtual ~SDK();

    std::shared_ptr<CMGFTrackerResult> track(std::shared_ptr<cv::Mat> rgb,double time=0.0f);
    std::shared_ptr<CMGFDetectorResult> detect_ir(std::shared_ptr<cv::Mat> mat,double time=0.0f);
    std::shared_ptr<CMGFDetectorResult> detect(std::shared_ptr<cv::Mat> image);
    MGFErrorCode extract_src(std::shared_ptr<Image> image, const MGFLandmarkMap *landmark);
    std::shared_ptr<CFeatureResult> extract(const std::shared_ptr<CMGFDetectorResult> &dr, int index = -1);
    std::shared_ptr<CFeatureResult> extract(const std::shared_ptr<CMGFTrackerResult>& tr, int index = -1);
    int featureLen();    
    MGFComparisonAlgorithm get_comparison_algorithm();
    Result threshold();
    std::shared_ptr<CPoseBlur> pose_blur(std::shared_ptr<CMGFTrackerResult> image, int index = -1);
    std::shared_ptr<CPoseBlur> pose_blur(std::shared_ptr<CMGFDetectorResult> image, int index = -1);
    MGFErrorCode compare_src(const MGFFeature *a, const MGFFeature *b, float* score);
    MGFErrorCode compare_cur(const MGFFeature *a, const MGFFeature *b, float* score);
    std::shared_ptr<CFMPResult> fmp_cur(std::shared_ptr<CMGFTrackerResult> image, int index = -1);
    std::shared_ptr<CPoseBlur> landmark_attr(std::shared_ptr<CMGFDetectorResult> image, int index = -1);
    std::shared_ptr<CMGLRGBIRResult> rgbir (std::shared_ptr<CMGFTrackerResult> trackret, std::shared_ptr<CMGFDetectorResult> trackretIR, int trackIndex, int trackretIRIndex);

private:
    Detector     *m_detect;
    Mutex        m_mtx_detect;
    Detector     *m_detectir;
    Mutex        m_mtx_detectIR;
    Feature      *m_feature_cur;
    Mutex        m_mtx_feature_cur;
    Feature      *m_feature_src;
    Mutex        m_mtx_feature_src;
    Tracker      *m_track;
    Mutex        m_mtx_track;
    FMP          *m_fmp;
    Mutex        m_mtx_fmp;
    Rgbir        *m_rgbir;
    Mutex        m_mtx_rgbir;
    LandmarkAttr *m_lmk_attr;
    Mutex        m_mtx_lmk_attr;
    LandmarkAttr *m_pose_blur;
    Mutex        m_mtx_pose_blur;
    std::shared_ptr<std::thread>  m_init_thread;
    Event             *event;
    std::mutex        mtx;
    void _delayInit(const std::string model_dir, int min_face);
};

KAGAPI const char * version();

} // namespace kag

#endif // __KAG_SDK_H__

#pragma warning(pop)