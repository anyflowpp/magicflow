#include <iostream>
#include <future>
#include <chrono>
#include <thread>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "kafface.h"
#include "sdk.h"

#define INFRARED_SDK_VERSION "1.3.1"

using namespace kag;

Image::~Image() {
    if (NULL != ctx) {
        MGF_release(ctx);
    }
}

Image::Image(const std::shared_ptr<cv::Mat> img, COLOR color,double time):time(time) {
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
        LOGF("error: make %s image failed.", COLOR::bgr == color ? "bgr" : "gray");
        ctx = NULL;
    }
}

Image::Image(const char *data, int width, int height, COLOR color):ctx(NULL),time(0.0f){
    auto code = COLOR::bgr == color ? 
        MGF_MAKE_BGR_IMAGE (data, height, width, &ctx) : 
        MGF_MAKE_GRAY_IMAGE(data, height, width, &ctx);
    if (MGF_OK != code) {
        LOGF("error: make %s image failed.", COLOR::bgr == color ? "bgr" : "gray");
        ctx = NULL;
    }
}

int area_of_rect(const MGFRect * r) {
    return (r->right - r->left) * (r->bottom - r->top);
}

int overlap_area(const MGFRect * a, const MGFRect * b) {
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

float MGFIoU::iou(const MGFRect * a, const MGFRect * b) {
    int a_area = area_of_rect(a);
    int b_area = area_of_rect(b);
    int overlap = overlap_area(a, b);
    auto ret = static_cast<float>(overlap) / static_cast<float>(a_area + b_area - overlap);
    return ret;
}

Event::Event() {
}

Event::~Event() {
}

float Event::threshold() const {
    return m_threshold;
}

void Event::refresh() const {
}

#define RELEASE_PTR(ptr) if (NULL != ptr) { \
    delete ptr; \
    ptr = NULL; \
}

template <class T, typename... Args>
int load_model(T* &member, const char * msg, const std::string & model_dir, const std::string & model, Args... args) {
    #ifdef _WIN32
        auto model_path = model_dir + "\\" + model; 
    #else
        auto model_path = model_dir + "/" + model; 
    #endif
    member = new T();
    auto code = member->load(model_path.data(), args...);
    if(MGF_OK != code) {
        LOGF("error: load %s model failed. code: %d [%s]", msg, code, model.data());
        RELEASE_PTR(member);
        throw std::string("SDK load error");
    } else {
        LOGF("load %10s model success.", msg);
    }
    return code;
}

#if MGF_ENABLE_KAG_LIMIT
void load_auth(const std::string & model_dir) {
    auto file = model_dir + "\\license.bin";
    auto code = MGF_init_auth(file.data());
    LOGF("load auth code: %d", code);
}
#endif

SDK::SDK(const char * model_dir, int min_face, Event *event) : 
         m_detect(NULL), m_fmp(NULL), m_feature_cur(NULL),
         m_detectir(NULL),
         m_feature_src(NULL), m_track(NULL),
         m_rgbir(NULL), event(event),
         m_pose_blur(NULL),
         m_lmk_attr(NULL) {
    SDK_FUNC_PRE;
    std::lock_guard<std::mutex> lock(mtx);
    MGF_set_log_level(MGF_LOG_WARN);
#if MGF_ENABLE_KAG_LIMIT
    load_auth(model_dir);
#endif
    if(!m_init_thread){
        m_init_thread = std::make_shared<std::thread>(&SDK::_delayInit, this, std::string(model_dir), min_face);
    }
    load_model<Tracker      >(m_track ,      "track rgb",  model_dir, "data/tracker.mobile.v4.fast.conf");
    load_model<Detector, int>(m_detectir,  "detect ir",    model_dir, "data/detector.large.v3.pf.lyb.190119.conf", min_face);
    load_model<LandmarkAttr >(m_lmk_attr,    "lmk attr",   model_dir, "data/lmk.all_attr_configurable.all-in-one.190731.conf");
    load_model<LandmarkAttr >(m_pose_blur,   "lmk attr",   model_dir, "data/lmk.all_attr_configurable.pose.blur.190323.conf");
}
void SDK::_delayInit(const std::string model, int min_face){
    const char* model_dir = model.c_str();
    std::this_thread::sleep_for(std::chrono::microseconds(100));
    load_model<Feature      >(m_feature_cur, "feature",    model_dir, "data/feat.faceid_boltzmann191108.conf");
    load_model<Feature      >(m_feature_src, "feature",    model_dir, "data/feat.faceid_boltzmann191108.conf");
    load_model<FMP          >(m_fmp ,      "rgb fmp",      model_dir, "data/panorama.mobile.20191111.fmp.NIR_05.20191111.conf");
    load_model<Rgbir        >(m_rgbir,       "rgbir",      model_dir, "data/panorama.facepass.190630_RGBIR.combine.conf");
    load_model<Detector, int>(m_detect,    "detect rgb",   model_dir, "data/detector.large.v3.pf.lyb.190119.conf", min_face);
}
SDK::~SDK() {
    SDK_FUNC_PRE;
    std::lock_guard<std::mutex> lock(mtx);
    RELEASE_PTR(m_detect);
    RELEASE_PTR(m_detectir);
    RELEASE_PTR(m_feature_cur);
    RELEASE_PTR(m_feature_src);
    RELEASE_PTR(m_track);
    RELEASE_PTR(m_fmp);
    RELEASE_PTR(m_rgbir);
    RELEASE_PTR(m_lmk_attr);
    RELEASE_PTR(m_pose_blur);

    MGF_shutdown();
}

std::shared_ptr<CFMPResult> SDK::fmp_cur(std::shared_ptr<CMGFTrackerResult> image, int index) {
    SDK_FUNC_PRE;
    Guard locker(m_mtx_fmp);
    std::vector<MGFLandmarkMap> maps;
    std::shared_ptr<CFMPResult>  ret;
    if(index == -1){
        for(size_t i = 0;i<image->size;i++){
            maps.push_back(image->items[i].landmark);
        }
    }else{
        if((size_t)index>=image->size){
            throw std::out_of_range("fmp_cur");
        }
        maps.push_back(image->items[index].landmark);
    }
    Image gray(image->img->img,COLOR::gray);
    auto mgfret = this->m_fmp->classify(gray.data(),&maps[0],maps.size());
    if(mgfret != MGF_OK){
        return ret;
    }
    ret = m_fmp->current()?std::make_shared<CFMPResult>(m_fmp->current(),maps.size()):nullptr;
    return ret;
}

Result threshold_result() {
    Result result {
        MGF_UNKNOWN_ERROR,
        ResultType::THRESHOLD_RESULT,
    };
    ThresholdResult value {
        0.f,
        0.f,
        0.f,
        0.f,
        0.f,
    };
    result.threshold = std::move(value);
    return std::move(result);
}

Result verify_result() {
    Result result {
        MGF_UNKNOWN_ERROR,
        ResultType::VERIFY_RESULT,
    };
    VerifyResult value {
        0.f,
    };
    result.verify = std::move(value);
    return std::move(result);
}

Result detect_result() {
    Result result {
        MGF_UNKNOWN_ERROR,
        ResultType::DETECT_RESULT,
    };
    DetectResult value {
        0,
        0.f,
        FacePose{0.,0.,0.},
        Status{0.,0.,0.},
        Occlusion{0.,0.,0.},
        Rect{0,0,0,0},
    };
    result.detect = std::move(value);
    return std::move(result);
}

Result liveness_result() {
    Result result {
        MGF_UNKNOWN_ERROR,
        ResultType::LIVENSS_RESULT,
    };
    LivenessResult value {
        0,
        0.f,
        0.f,
        0.f,
        Rect{0,0,0,0},
        Rect{0,0,0,0},
    };
    result.liveness = std::move(value);
    return std::move(result);
}

// Result SDK::track_result() {
//     BEGIN_CREATE_RESULT(result.track = TrackResult {
//         -1,
//         Rect {0, 0, 0, 0},
//     });
//     END_CREATE_RESULT;
// }

int filter_confidence(const MGFDetectorResult *result, float threshold = 0.5f) {
    int count = result->size;
    for (int i = 0; i < count; i++) {
        auto face = &(result->items[i]);
        if (face->confidence < threshold) {
            count --;
        }
    }
    return count;
}

int max_face(const MGFTrackerResult * result) {
    int max = 0;
    int index = -1;
    int count = result->size;
    for (int i = 0; i < count; i ++) {
        auto face = &(result->items[i]);
        auto area = area_of_rect(&face->rect);
        if (max < area) {
            index = i;
            max = area;
        }
    }
    return index;
}

int max_face(const MGFDetectorResult *result, float threshold = 0.5f) {
    if (filter_confidence(result, threshold) <= 0) {
        return -1;
    }
    int max = 0;
    int index = -1;
    int count = result->size;
    for (int i = 0; i < count; i ++) {
        auto face = &(result->items[i]);
        if (face->confidence <= threshold) {
            continue;
        }
        auto area = area_of_rect(&face->rect);
        if (max < area) {
            index = i;
            max = area;
        }
    }
    return index;
}

#define RESULT_CHECK(check, retval, expr) if(!(check)) { \
    expr; \
    LOGF("error: %s:%d", __FILE__, __LINE__); \
    return retval; \
}

Result SDK::threshold() {
    SDK_FUNC_PRE;
    Guard locker(m_mtx_feature_cur);
    LOGF("enter SDK::threshold()");
    auto result = threshold_result();
    auto code = m_feature_cur->threshold();
    LOGF("threshold code = %d", code);
    RESULT_CHECK(MGF_OK == code, result, result.code = MGF_UNKNOWN_ERROR);
    auto threshold = m_feature_cur->feature_threshold();
    result.code = MGF_OK;
    result.threshold = ThresholdResult {
        threshold.hundredth,
        threshold.thousandth,
        threshold.ten_thousandth,
        threshold.hundred_thousandth,
        threshold.millionth
    };
    LOGF("leave SDK::threshold()");
    return std::move(result);
}

std::shared_ptr<kag::CMGFTrackerResult> SDK::track(std::shared_ptr<cv::Mat> rgb,double time){
    SDK_FUNC_PRE;
    Guard locker(m_mtx_track);
    auto gray = std::make_shared<Image>(rgb,COLOR::gray,time);
	auto mgfret = m_track->track(gray->data());
    std::shared_ptr<kag::CMGFTrackerResult> ret;
    if (mgfret == MGF_OK) {
        if(m_track->current()){
            ret = std::make_shared<CMGFTrackerResult>(m_track->current(),gray);
        }
        if(event){
            std::async(std::launch::async,[=](std::shared_ptr<CMGFTrackerResult> rs,std::shared_ptr<Image> img){
                event->onTrackerPosInfo(rs,img);
            },ret,gray);
            //event->onTrackerPosInfo(ret,gray);
        }
	}
	else {
		LOGF("m_track error");
	}
    return ret;
}

std::shared_ptr<CMGFDetectorResult> SDK::detect_ir(std::shared_ptr<cv::Mat> image, double time) {
    SDK_FUNC_PRE;
    Guard locker(m_mtx_detectIR);
    auto mgfimg = std::make_shared<Image>(image,COLOR::gray);
    auto megret = this->m_detectir->detect(mgfimg->data());
    std::shared_ptr<CMGFDetectorResult> ret;
    if(megret!=MGF_OK){
        return ret;
    }
    ret = std::make_shared<CMGFDetectorResult>(m_detectir->current(),mgfimg);
    return ret;
}

std::shared_ptr<CMGFDetectorResult> SDK::detect(std::shared_ptr<cv::Mat> image) {
    SDK_FUNC_PRE;
    Guard locker(m_mtx_detect);
    auto mgfimg = std::make_shared<Image>(image,COLOR::gray);
    auto megret = this->m_detect->detect(mgfimg->data());
    std::shared_ptr<CMGFDetectorResult> ret;
    if(megret!=MGF_OK){
        return ret;
    }
    ret = std::make_shared<CMGFDetectorResult>(m_detect->current(),mgfimg);
    return ret;
}

std::shared_ptr<CPoseBlur> SDK::landmark_attr(std::shared_ptr<CMGFDetectorResult> dr, int index) {
    SDK_FUNC_PRE;
    Guard locker(m_mtx_lmk_attr);
    std::shared_ptr<CPoseBlur> ret;
    std::vector<MGFLandmarkMap> maps;
    if(index == -1){
        for(size_t i = 0;i<dr->size;i++){
            maps.push_back(dr->items[i].landmark);
        }
    }else{
        if(index >= (int)dr->size){
            throw std::out_of_range("exact");
        }
        maps.push_back(dr->items[index].landmark);
    }
    auto &image = dr->img;
    Image bgr(dr->img->img,COLOR::bgr);
    auto r = m_lmk_attr->predict(bgr.data(), &maps[0], maps.size());
    if(r != MGF_OK){
        return ret;
    }
    if(this->m_pose_blur->current()){
        ret = std::make_shared<CPoseBlur>(m_lmk_attr->current(), maps.size());
    }
    return ret;
}

std::shared_ptr<CMGLRGBIRResult> SDK::rgbir (std::shared_ptr<CMGFTrackerResult> trackret, std::shared_ptr<CMGFDetectorResult> trackretIR, int trackIndex, int trackretIRIndex) {
    SDK_FUNC_PRE;
    Guard locker(m_mtx_rgbir);
    std::shared_ptr<CMGLRGBIRResult> ret;
    if(trackIndex<0 || (size_t)trackIndex>=trackret->size) {
        throw std::out_of_range("rgb track");
    }
    if(trackretIRIndex<0 || (size_t)trackretIRIndex>= trackretIR->size){
        throw std::out_of_range("ir track");
    }
    Image rgb(trackret->img->img,COLOR::bgr);
    auto rgbirR = m_rgbir->classify(rgb.data(), &trackret->items[trackIndex].landmark, trackretIR->img->data(), &trackretIR->items[trackretIRIndex].landmark);
    if (!m_rgbir->current() || rgbirR != MGF_OK) {
        return ret;
    }
    ret = std::make_shared<CMGLRGBIRResult>(m_rgbir->current(), 1);
    return ret;
}

std::shared_ptr<CPoseBlur> SDK::pose_blur(std::shared_ptr<CMGFTrackerResult> image, int index){
    SDK_FUNC_PRE;
    Guard locker(m_mtx_pose_blur);
    std::shared_ptr<CPoseBlur> ret;
    std::vector<MGFLandmarkMap> maps;
    if(index == -1) {
        for(unsigned int i = 0;i<image->size;i++){
            maps.push_back(image->items[i].landmark);
        }
    } else {
        if(index>=(int)image->size){
            throw std::out_of_range("pose blur");
        }
        maps.push_back(image->items[index].landmark);
    }
    if(maps.size()==0){
        return ret;
    }
    auto megc = m_pose_blur->predict(image->img->data(), &maps[0], maps.size());
    if(megc!=MGF_OK || !m_pose_blur->current()){
        return ret;
    }
    if(this->m_pose_blur->current()){
        ret = std::make_shared<CPoseBlur>(m_pose_blur->current(), maps.size());
    }
    return ret;
}

std::shared_ptr<CPoseBlur> SDK::pose_blur(std::shared_ptr<CMGFDetectorResult> image, int index) {
    SDK_FUNC_PRE;
    std::shared_ptr<CPoseBlur> ret;
    std::vector<MGFLandmarkMap> maps;
    if(index == -1) {
        for(unsigned int i = 0;i<image->size;i++){
            maps.push_back(image->items[i].landmark);
        }
    } else {
        if(index>=(int)image->size){
            throw std::out_of_range("pose blur");
        }
        maps.push_back(image->items[index].landmark);
    }
    if(maps.size()==0){
        return ret;
    }
    auto megc = m_pose_blur->predict(image->img->data(), &maps[0], maps.size());
    if(megc!=MGF_OK || !m_pose_blur->current()){
        return ret;
    }
    ret = std::make_shared<CPoseBlur>(m_pose_blur->current(), maps.size());
    return ret;
}

kag::MGFErrorCode SDK::extract_src(std::shared_ptr<Image> image, const MGFLandmarkMap *landmark) {
    SDK_FUNC_PRE;
    Guard locker(m_mtx_feature_src);
    return m_feature_src->extract(image->data(), landmark);
}

std::shared_ptr<CFeatureResult> SDK::extract(const std::shared_ptr<CMGFDetectorResult> &dr, int index) {
    SDK_FUNC_PRE;
    Guard locker(m_mtx_feature_src);
    std::shared_ptr<CFeatureResult> ret;
    std::vector<MGFLandmarkMap> maps;
    if(index == -1){
        for(size_t i = 0;i<dr->size;i++){
            maps.push_back(dr->items[i].landmark);
        }
    }else{
        if(index >= (int)dr->size){
            throw std::out_of_range("exact");
        }
        maps.push_back(dr->items[index].landmark);
    }
    if(maps.size()==0){
        return ret;
    }
    Image bgr(dr->img->bgr?dr->img->bgr:dr->img->img,COLOR::bgr);
    auto feat_ret = m_feature_src->extract(bgr.data(),&maps[0],maps.size());
    if(feat_ret != MGF_OK){
        return ret;
    }
    ret = m_feature_src->current()?std::make_shared<CFeatureResult>(m_feature_src->current(), maps.size()):nullptr;
    return ret;
}

std::shared_ptr<CFeatureResult> SDK::extract(const std::shared_ptr<CMGFTrackerResult>& tr, int index) {
    SDK_FUNC_PRE;
    Guard locker(m_mtx_feature_cur);
    std::shared_ptr<CFeatureResult> ret;
    std::vector<MGFLandmarkMap> maps;
    if(index == -1){
        for(size_t i = 0;i<tr->size;i++){
            maps.push_back(tr->items[i].landmark);
        }
    }else{
        if(index >= (int)tr->size){
            throw std::out_of_range("exact trackresult");
        }
        maps.push_back(tr->items[index].landmark);
    }
    Image bgr(tr->img->bgr?tr->img->bgr:tr->img->img,COLOR::bgr);
    auto feat_ret = m_feature_cur->extract(bgr.data(),&maps[0], maps.size());
    if(feat_ret != MGF_OK){
        return ret;
    }
    ret = m_feature_cur->current()?std::make_shared<CFeatureResult>(m_feature_cur->current(), maps.size()):nullptr;
    return ret;
}

int SDK::featureLen()  {
    SDK_FUNC_PRE;
    Guard locker(m_mtx_feature_cur);
    if(!m_feature_cur){
        return 0;
    }
    return m_feature_cur->size;
}   

MGFComparisonAlgorithm SDK::get_comparison_algorithm() {
    SDK_FUNC_PRE;
    Guard locker(m_mtx_feature_src);
    return m_feature_src->get_comparison_algorithm();
}

kag::MGFErrorCode SDK::compare_src(const MGFFeature *a, const MGFFeature *b, float* score) {
    SDK_FUNC_PRE;
    Guard locker(m_mtx_feature_src);
    auto megret = m_feature_src->compare(a, b);
    if(megret==MGF_OK) {
        *score = m_feature_src->score();
    }
    return megret;
}

kag::MGFErrorCode SDK::compare_cur(const MGFFeature *a, const MGFFeature *b, float* score) {
    SDK_FUNC_PRE;
    Guard locker(m_mtx_feature_cur);
    auto megret = m_feature_cur->compare(a, b);
    if(megret==MGF_OK) {
        *score = m_feature_cur->score();
    }
    return megret;
}

static const char * infrared_sdk_version = INFRARED_SDK_VERSION;

const char * kag::version() {
    return infrared_sdk_version;
}

bool CMGFTrackerResult::comp(const MGFTrackedFace &a, const MGFTrackedFace &b) {
    int arerA = std::abs(a.rect.right - a.rect.left) * std::abs(a.rect.bottom - a.rect.top);
    int arerB = std::abs(b.rect.right - b.rect.left) * std::abs(b.rect.bottom - b.rect.top);
    return arerA < arerB;
}

CMGFTrackerResult::CMGFTrackerResult():size(0), isSort(false) {

}

CMGFTrackerResult::~CMGFTrackerResult() {

}

CMGFTrackerResult::CMGFTrackerResult(const MGFTrackerResult *p,std::shared_ptr<Image> img) : size(0), isSort(false), img(img) {
    if (p) {
        size = p->size;
        if (size > 0) {
            items.assign(p->items, &p->items[p->size]);
        } else {
            items.clear();
        }
    }
}
CMGFTrackerResult::CMGFTrackerResult(std::shared_ptr<CMGFTrackerResult> t,int index):size(0),isSort(false){
    if(!t){
        return;
    }
    if(index >= (int)t->size){
        throw std::out_of_range("track result items");
    }
    if(index == -1){
        *this = *t;
        return;
    }else{
        this->size = 1;
        this->items.push_back(t->items[index]);
        this->img=t->img;
    }
}

void CMGFTrackerResult::Sort() {
    if (!isSort) {
        std::sort(this->items.begin(), this->items.end(), CMGFTrackerResult::comp);
        isSort = true;
    }
}

CPoseBlur::CPoseBlur(){
}

CPoseBlur::~CPoseBlur(){
}

CPoseBlur::CPoseBlur(const MGFLandmarkAttribute* p, int size):size(size){
    if(!p){
        this->size=0;
        items.clear();
    }
    for(int i = 0;i<this->size;i++) {
        items.push_back(p[i]);
    }
}

CMGFDetectorResult::CMGFDetectorResult():size(0) {

}

bool CMGFDetectorResult::comp(const MGFDetectedFace &a, const MGFDetectedFace &b) {
    int arerA = std::abs(a.rect.right - a.rect.left) * std::abs(a.rect.bottom - a.rect.top);
    int arerB = std::abs(b.rect.right - b.rect.left) * std::abs(b.rect.bottom - b.rect.top);
    return arerA < arerB;
}

void CMGFDetectorResult::Sort(){
    if(!std::is_sorted(this->items.begin(),this->items.end(),CMGFDetectorResult::comp)){
        std::sort(this->items.begin(),this->items.end(),CMGFDetectorResult::comp);
    }
}

CMGFDetectorResult::CMGFDetectorResult(std::shared_ptr<CMGFDetectorResult> t,int index):size(0){
    if(!t){
        return;
    }
    if(index >= (int)t->size){
        throw std::out_of_range("track result items");
    }
    if(index == -1){
        *this = *t;
        return;
    }else{
        this->size = 1;
        this->items.push_back(t->items[index]);
        this->img= t->img;
    }
}

CMGFDetectorResult::CMGFDetectorResult(const MGFDetectorResult* p, std::shared_ptr<Image> img):img(img){
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

CMGFDetectorResult::~CMGFDetectorResult() {

}

CFeatureResult::CFeatureResult():size(0) {

}

CFeatureResult::CFeatureResult(const MGFFeature* p, size_t n):size(n) {
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

CFeatureResult::~CFeatureResult() {
    for(auto a = items.begin();a!=items.end();a++){
        delete[] a->data;
    }
    this->items.clear();
}

CFMPResult::CFMPResult():size(0) {

}

CFMPResult::CFMPResult(const MGLFMPResult* const p, size_t size):size(size) {
    if(!p){
        size = 0;
        this->items.clear();
    }
    for(size_t i = 0;i<size;i++){
        this->items.push_back(p[i]);
    }
}

CFMPResult::~CFMPResult() {

}

CMGLRGBIRResult::CMGLRGBIRResult():size(0) {

}

CMGLRGBIRResult::CMGLRGBIRResult(const MGLRGBIRResult* const p, size_t size):size(size) {
    if(!p || size == 0){
        return;
    }
    for(size_t i = 0; i<size; i++) {
        items.push_back(p[i]);
    }

}

CMGLRGBIRResult::~CMGLRGBIRResult() {

}
