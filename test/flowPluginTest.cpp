#include<gtest/gtest.h>
#include"flowFactory/include/flowFactory.h"
#include<core/common/node_info.h>
#include<map>
#include<iostream>
#include<core/common/flowtype.hpp>

TEST(flowplugin,t1){
    auto flow = flowFactory::GenFlowFromJsonFile("flow1.json");
    input_type_ptr a = std::make_shared<input_type>();
    std::string indata = "hello megflow";
    std::shared_ptr<std::string> dp = std::make_shared<std::string>(indata);
    a->insert(std::pair<std::string,std::shared_ptr<void>>("input",dp));
    flow->SetInput(a,std::make_shared<Node_Info>());
};