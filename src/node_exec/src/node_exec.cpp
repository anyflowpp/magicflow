#include"node_exec.h"
#include"spdlog_wrap.h"

node_exec::node_exec():m_ctx(nullptr){}

node_exec::~node_exec(){}

input_type_ptr node_exec::NodeExec(input_type_ptr input, void *ctx, node_info_ptr info){
	return input;
}

void* node_exec::CreateThreadContext(){
	return nullptr;
}

void* node_exec::GetThreadContext(){
	return nullptr;
} 

void node_exec::DestroyThreadContext(void* ctx){
	;; //do noting
}