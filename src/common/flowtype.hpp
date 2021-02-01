#pragma once
#include"CMGFResult.hpp"

typedef std::shared_ptr<cv::Mat> img_ptr;
class mini_el{
public:
    img_ptr img;
    kag::Image_ptr mgfimg_bgr;
    kag::Image_ptr mgfimg_gray;
    kag::CMGFTrackerResult_ptr track_result;
    kag::CMGFDetectorResult_ptr detect_result;
    kag::CFeatureResult_ptr feat_result;
};
typedef std::shared_ptr<mini_el> mini_el_ptr; 

class flow_data;
typedef std::shared_ptr<flow_data> flow_data_ptr;
class flow_data{
    //TODO: 增加gen flowdata的代码 防止调用者自己调用std::make_shared,我们自己管理, 方便更换到其他指针最主要的,
    //如果以后出现嵌套指针,需要weak_ptr啥的, 不能保证调用者能了解内部逻辑

public:
    inline static flow_data_ptr flow_data::GenDataWithImg(img_ptr img){
        flow_data_ptr ret;
        ret = std::make_shared<flow_data>();
        mini_el_ptr el_Ptr = std::make_shared<mini_el>();
        el_Ptr->img = img;
        ret->data.push_back(el_Ptr);
        return ret;
    }
    std::vector<mini_el_ptr> data;
};