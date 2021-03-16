#include<gtest/gtest.h>
#include"flowFactory/include/flowFactory.h"
#include<common/node_info.h>
#include<map>
#include<iostream>
#include<common/flowtype.hpp>
#include<opencv2/opencv.hpp>
#include<ffmpeg-input/include/ffmpeg-input-struct.h>
#include<spdlog_wrap.h>
#include<thread>
#include<chrono>
#include<condition_variable>

std::mutex s_mutex;
std::condition_variable s_cond;
cv::Mat s_mat;

typedef anyflow::flow<void>::flow_data_ptr input_type_ptr;
typedef anyflow::node_info_ptr node_info_ptr;
typedef std::map<
	std::string,
	std::shared_ptr<void>
> input_Type;

inline void backfunc(input_type_ptr _input,node_info_ptr){


	std::shared_ptr< std::map<
		std::string,
		std::shared_ptr<void>
	>
	> input = std::static_pointer_cast<input_Type>(_input);
	if(!input){
		return;
	}

    auto mynodeout = input->find("mynode");
    auto ffmpegout = input->find("ffmpeginput");
    std::shared_ptr<int> intout = std::static_pointer_cast<int>(mynodeout->second);
    
	if (ffmpegout != input->end()) {
		std::shared_ptr<ffmpeg_input_struct> ffmpegoutitem = std::static_pointer_cast<ffmpeg_input_struct>(ffmpegout->second);
		uchar* data = (uchar*)&ffmpegoutitem->data[0];
		cv::Mat m(ffmpegoutitem->h, ffmpegoutitem->w, CV_8UC3, data);

		{
			std::unique_lock<std::mutex> locker(s_mutex);
			s_mat = m;
		}
		s_cond.notify_one();
	}
    
    EXPECT_GT(*intout,100);
    logw("callback:{}",*intout);
}

void show_func() {
	cvNamedWindow("show");
	cv::Mat m;
	while (true) {
		{
			std::unique_lock<std::mutex> locker(s_mutex);
			s_cond.wait(locker);
			m = s_mat.clone();
		}
		cv::imshow("show", m);
		cv::waitKey(100);
	}
}

TEST(flowplugin,t1){
    auto flow = flowFactory::GenFlowFromJsonFile("flow1.json");
    
    flow->SetCallBack(backfunc);
	std::thread t1(show_func);
    while(true){
		auto a = std::make_shared<input_Type>();
		std::string indata = "hello magflow";
		std::shared_ptr<std::string> dp = std::make_shared<std::string>(indata);
		a->insert(std::pair<std::string, std::shared_ptr<void>>("input", dp));

		flow->SetInput(a,std::make_shared<anyflow::Node_Info>());
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	
    }
};