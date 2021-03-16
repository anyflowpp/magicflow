#pragma once
#include <string>
#include <iostream>
#include<intrin.h>
#include<atomic>
#include<chrono>
#include <vector>
#include"thread_pool.h"
#include"core/thread_pool.h"

class magic_thread_pool_impl:magic_thread_pool{
public:
    anyflow::flow_thread_pool& _pool;
	magic_thread_pool_impl() 
    :_pool(anyflow::flow_thread_pool::GetInstance()){
    }
    int getCpuCount() override{
        return _pool.getCpuCount();
    }
    void schedule(Task task)override{
        _pool.schedule(task);
    }
};

magic_thread_pool::magic_thread_pool(){
}

void magic_thread_pool::schedule(Task task) {
}

int magic_thread_pool::getCpuCount(){
    return 1;
}

magic_thread_pool::~magic_thread_pool() {
}

std::shared_ptr<magic_thread_pool>& magic_thread_pool::GetInstance() {
    static auto  s__instance = std::make_shared<magic_thread_pool_impl>();
	static std::shared_ptr<magic_thread_pool> r = std::dynamic_pointer_cast<magic_thread_pool>(s__instance);
    return r;
}