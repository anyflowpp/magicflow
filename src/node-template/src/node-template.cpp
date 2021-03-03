#include"node-template.h"

std::shared_ptr<void> mynode::NodeExec(std::shared_ptr<void> input, void *ctx, node_info_ptr info){
    return input;
}
void* mynode::CreateThreadContext(){
    return nullptr;
}
void mynode::DestroyThreadContext(void* ctx){}
std::shared_ptr<node_exec> mynode::CreateNode(){
    return std::make_shared<mynode>();
}