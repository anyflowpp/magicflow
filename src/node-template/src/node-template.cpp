#include"node-template.h"
#include<spdlog_wrap.h>
#include<memory>

input_type_ptr mynode::NodeExec(input_type_ptr input, void *ctx, node_info_ptr info){
    auto idata = input->find("input");
    std::shared_ptr<std::string> istring = std::static_pointer_cast<std::string>(idata->second);
    logw("input:{}",*istring);
    return input;
}
void* mynode::CreateThreadContext(){
    return nullptr;
}
void mynode::DestroyThreadContext(void* ctx){}
std::shared_ptr<node_exec> mynode::CreateNode(){
    return std::make_shared<mynode>();
}