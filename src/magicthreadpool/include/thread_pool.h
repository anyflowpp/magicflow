#pragma once
#include <functional>
#include<common/magic_func.h>
#include<memory>

class MAGIC_E_FUNC magic_thread_pool{
public:
    typedef std::function<void(void)> Task;
    magic_thread_pool();
    virtual void schedule(Task task); 
    virtual int getCpuCount();
    virtual ~magic_thread_pool();
    static std::shared_ptr<magic_thread_pool>& GetInstance(); 
};