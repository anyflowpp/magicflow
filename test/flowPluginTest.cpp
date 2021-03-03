#include<gtest/gtest.h>
#include"flowFactory/include/flowFactory.h"
#include<core/common/node_info.h>

TEST(flowplugin,t1){
    auto flow = flowFactory::GenFlowFromJsonFile("flow1.json");
    flow->SetInput(nullptr,std::make_shared<Node_Info>());
};