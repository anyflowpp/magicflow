#include<gtest/gtest.h>
#include"mgfflow.h"
#include<fstream>
#include"spdlog_wrap.h"

typedef flow<flow_data> mgfflow;
typedef std::shared_ptr<mgfflow> mgfflow_ptr;
static inline void callback(flow_data_ptr output, node_info_ptr info){
    auto el = output->data[0];
    static int i = 0;
    i++;
    logi("alloutput count:{}",i);
    if(el->track_result) logi("track face num:{}",el->track_result->items.size());
    if(el->detect_result) logi("detect face num:{}",el->detect_result->items.size());
    if(el->feat_result){ logi("every feat length :{}",el->feat_result->items[0].size);}
}
TEST(flowtest,t1){
    setLogLevel(spdlog::level::debug);
    auto &g = flow_thread_pool::GetInstance();
    int cpus = g.getCpuCount();
    logi("cpu count:{}",cpus);
    Json::Value root;
    Json::Reader jsonread;
    std::fstream fin("./flow1.json",std::ios::in);
    jsonread.parse(fin,root);
    auto flow = mgfflow::GetInstance(root);
    flow->SetCfg(flowcfg(flowcfg::async,flowcfg::sync));
    flow->SetCallBack(callback);
    cv::Mat oneimg = cv::imread("rgb.face.jpg");
    for(int i = 0;i<100;i++){
        logi("i = {}",i);
        auto rgb = std::make_shared<cv::Mat>(oneimg);
		node_info_ptr info = std::make_shared<Node_Info>();
        flow_data_ptr input = flow_data::GenDataWithImg(rgb);
        flow->SetInput(input,info);
        if(i%1000==0){
        //    auto out =  flow->GetOutput();
        }
    }
    // flow->GetOutput();
};
