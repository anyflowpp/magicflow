#pragma once
#include <functional>
#include<magic_func.h>

class MAGIC_E_FUNC flow_thread_pool{
public:
    typedef std::function<void(void)> Task;
    flow_thread_pool();
    void schedule(Task task); 
    int getCpuCount();
    virtual ~flow_thread_pool();
    static flow_thread_pool& GetInstance(); 
private:
    int m_cpuCount;
};