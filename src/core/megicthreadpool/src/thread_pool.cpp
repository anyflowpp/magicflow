#pragma once
#include <string>
#include <iostream>
#include<intrin.h>
#include<atomic>
#include<chrono>
#include <vector>
#include "flow_cpuinfo.h"
#include"std_impl_thread_pool.h"
#include"thread_pool.h"

typedef std_impl_thread_pool impl_thread_pool;
static std::unique_ptr<impl_thread_pool> _pool;

flow_thread_pool::flow_thread_pool(){
    if(!_pool){
        flow_cpuinfo cpuinfo;
        auto thread_num = cpuinfo.hard_core;
        m_cpuCount = thread_num;
        _pool = std::make_unique<impl_thread_pool>(thread_num);
    }
}

void flow_thread_pool::schedule(Task task) {
    return _pool->schedule(task);
}

int flow_thread_pool::getCpuCount(){
    return m_cpuCount;
}

flow_thread_pool::~flow_thread_pool() {
    if(_pool){
        _pool.reset();
    }
}

flow_thread_pool& flow_thread_pool::GetInstance() {
    static flow_thread_pool __instance;
    return __instance;
}