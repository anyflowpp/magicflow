#include<gtest/gtest.h>
#include"flowFactory/include/flowFactory.h"
#include<core/common/node_info.h>
#include<map>
#include<iostream>
#include<core/common/flowtype.hpp>
#include<spdlog_wrap.h>

inline void backfunc(input_type_ptr input,node_info_ptr){
    auto mynodeout = input->find("mynode");
    std::shared_ptr<int> intout = std::static_pointer_cast<int>(mynodeout->second);
    EXPECT_GT(*intout,100);
    logw("callback:{}",*intout);
}
TEST(flowplugin,t1){
    auto flow = flowFactory::GenFlowFromJsonFile("flow1.json");
    input_type_ptr a = std::make_shared<input_type>();
    std::string indata = "hello megflow";
    std::shared_ptr<std::string> dp = std::make_shared<std::string>(indata);
    a->insert(std::pair<std::string,std::shared_ptr<void>>("input",dp));
    flow->SetInput(a,std::make_shared<Node_Info>());
    flow->SetCallBack(backfunc);
};