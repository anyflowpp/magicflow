#pragma once
#pragma warning(push)
#pragma warning(disable:4251)
#ifndef __KAG_SDK_H__
#define __KAG_SDK_H__

#include <memory>
#include<vector>
#include <mutex>
#include <atomic>
#include<opencv2/opencv.hpp>
#include<megface-c/megface.h>
#include<meglive-c/rgb_ir_fmp_context.h>
#include<meglive-c/fmp_context.h>
#include"spdlog_wrap.h"
#define KAGAPI  
namespace kag {

inline int area_of_rect(const MGFRect * r) {
    return (r->right - r->left) * (r->bottom - r->top);
}

inline int overlap_area(const MGFRect * a, const MGFRect * b) {
    int max_left = std::max(a->left, b->left);
    int max_top  = std::max(a->top, b->top);
    int min_right = std::min(a->right, b->right);
    int min_bottom = std::min(a->bottom, b->bottom);

    if (max_left >= min_right || max_top >= min_bottom) {
        return 0;
    } else {
        return (min_right - max_left) * (min_bottom - max_top);
    }
}

typedef enum COLOR {
    bgr  = 0,
    gray = 1,
} COLOR;

class KAGAPI Image {
public:
    Image(const char *data, int width, int height, COLOR color = COLOR::bgr){
        auto code = COLOR::bgr == color ? 
            MGF_MAKE_BGR_IMAGE (data, height, width, &ctx) : 
            MGF_MAKE_GRAY_IMAGE(data, height, width, &ctx);
        if (MGF_OK != code) {
            loge("error: make {} image failed.:{}", COLOR::bgr == color ? "bgr" : "gray",code);
            ctx = NULL;
        }
    }

    Image(std::shared_ptr<cv::Mat> img, COLOR color = COLOR::bgr,double time = 0.0f){
        if(!img || img->cols==0 || img->rows==0) {
            return;
        }
        // 必须clone一下,要不这个会根据现场照片实时变化
        this->img = std::make_shared<cv::Mat>(img->clone());
        MGFErrorCode code = MGF_UNKNOWN_ERROR;
        auto bgr = std::make_shared<cv::Mat>(*img);
        if(4 == img->channels()) {
            cv::cvtColor(*img, *bgr, CV_BGRA2BGR);
        }
        if(color == COLOR::bgr){
            code = MGF_MAKE_BGR_IMAGE (bgr->data, bgr->rows, bgr->cols, &ctx); 
            this->bgr = bgr;
        }else if(color == COLOR::gray) {
            auto gray = std::make_shared<cv::Mat>();
            cv::cvtColor(*bgr, *gray, CV_BGR2GRAY);
            code = MGF_MAKE_GRAY_IMAGE(gray->data, gray->rows, gray->cols, &ctx);
            this->gray = gray;
        }
        if (MGF_OK != code) {
            loge("error: make {} image failed.:{}", COLOR::bgr == color ? "bgr" : "gray",code);
            ctx = NULL;
        }
    }
    ~Image(){
        if (NULL != ctx) {
            MGF_release(ctx);
        }
    }
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
typedef std::shared_ptr<Image> Image_ptr;

class KAGAPI CMGFTrackerResult {
private:
    bool isSort;
    static bool comp(const MGFTrackedFace &a, const MGFTrackedFace &b){
        int arerA = std::abs(a.rect.right - a.rect.left) * std::abs(a.rect.bottom - a.rect.top);
        int arerB = std::abs(b.rect.right - b.rect.left) * std::abs(b.rect.bottom - b.rect.top);
        return arerA < arerB;
    }
public:
    std::vector<MGFTrackedFace> items;
    size_t size;
	CMGFTrackerResult():size(0),isSort(false){}
	~CMGFTrackerResult(){}
	CMGFTrackerResult(const MGFTrackerResult *p,std::shared_ptr<Image> img = nullptr){
        if (p) {
            size = p->size;
            if (size > 0) {
                items.assign(p->items, &p->items[p->size]);
            } else {
                items.clear();
            }
        }
    }
    std::shared_ptr<Image> img;
    void Sort(){
        if (!isSort) {
            std::sort(this->items.begin(), this->items.end(), CMGFTrackerResult::comp);
            isSort = true;
        }
    }
};
typedef std::shared_ptr<CMGFTrackerResult> CMGFTrackerResult_ptr;

class KAGAPI CMGFDetectorResult{
private:
    static bool comp(const MGFDetectedFace &a, const MGFDetectedFace &b){
        int arerA = std::abs(a.rect.right - a.rect.left) * std::abs(a.rect.bottom - a.rect.top);
        int arerB = std::abs(b.rect.right - b.rect.left) * std::abs(b.rect.bottom - b.rect.top);
        return arerA < arerB;
    }
public:
    std::vector<MGFDetectedFace> items;
    size_t size;
    std::shared_ptr<Image> img;
    void Sort(){
        if(!std::is_sorted(this->items.begin(),this->items.end(),CMGFDetectorResult::comp)){
            std::sort(this->items.begin(),this->items.end(),CMGFDetectorResult::comp);
        }
    }
    CMGFDetectorResult():size(0){}
    CMGFDetectorResult(const MGFDetectorResult* p, std::shared_ptr<Image> img = nullptr){
        if(!p){
            this->size = 0;
            this->items.clear();
            return;
        }
        this->size = p->size;
        for(size_t i=0;i<p->size;i++){
            items.push_back(p->items[i]);
        }
    }
    ~CMGFDetectorResult(){}
};
typedef std::shared_ptr<CMGFDetectorResult> CMGFDetectorResult_ptr;

class KAGAPI CFeatureResult {
public:
    size_t size;
    std::vector<MGFFeature> items;
    CFeatureResult():size(0){}
    CFeatureResult(const MGFFeature* p, size_t n):size(n) {
        if(!p){
            for(auto a = items.begin();a!=items.end();a++){
                delete[] a->data;
            }
            items.clear();
        }
        for(size_t i=0;i<n;i++){
            MGFFeature tmp;
            tmp.size=p[i].size;
            tmp.data = new char[tmp.size];
            if(!tmp.data){
                throw std::bad_alloc();
            }
            std::memcpy((char*)tmp.data,p[i].data,p[i].size);
            this->items.push_back(tmp);
        }
    }
    ~CFeatureResult() {
        for(auto a = items.begin();a!=items.end();a++){
            delete[] a->data;
        }
        this->items.clear();
    }
};
typedef std::shared_ptr<CFeatureResult> CFeatureResult_ptr;

class KAGAPI CMGLRGBIRResult{
public:
    size_t size;
    std::vector<MGLRGBIRResult> items;
    CMGLRGBIRResult():size(0) { }
    CMGLRGBIRResult(const MGLRGBIRResult* const p, size_t size):size(size) {
        if(!p || size == 0){
            return;
        }
        for(size_t i = 0; i<size; i++) {
            items.push_back(p[i]);
        }
    }
    CMGLRGBIRResult::~CMGLRGBIRResult() { }
};

class KAGAPI MGFIoU {
public: 
    static float MGFIoU::iou(const MGFRect * a, const MGFRect * b) {
        int a_area = area_of_rect(a);
        int b_area = area_of_rect(b);
        int overlap = overlap_area(a, b);
        auto ret = static_cast<float>(overlap) / static_cast<float>(a_area + b_area - overlap);
        return ret;
    }
};

class KAGAPI CAttrResult{
public:
    CAttrResult(){ }
    ~CAttrResult(){ }
    CAttrResult(const MGFLandmarkAttribute* p, int size):size(size){
        if(!p){
            this->size=0;
            items.clear();
        }
        for(int i = 0;i<this->size;i++) {
            items.push_back(p[i]);
        }
    }
    int size;
    std::vector<MGFLandmarkAttribute> items;
};

class KAGAPI CFMPResult {
public:
    CFMPResult::CFMPResult():size(0) { }
    CFMPResult(const MGLFMPResult* const p, size_t size):size(size) {
        if(!p){
            size = 0;
            this->items.clear();
        }
        for(size_t i = 0;i<size;i++){
            this->items.push_back(p[i]);
        }
    }

    ~CFMPResult() { }
    size_t size;
    std::vector<MGLFMPResult> items;
};

} // namespace kag

#endif // __KAG_SDK_H__

#pragma warning(pop)