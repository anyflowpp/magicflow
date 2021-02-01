#pragma once
#include <functional>


class flow_thread_pool{
public:
    typedef std::function<void(void)> Task;
    flow_thread_pool();
    void schedule(Task task); 
    int getCpuCount();
    virtual ~flow_thread_pool();
    static flow_thread_pool& GetInstance(); 
private:
    std::unique_ptr<impl_thread_pool> _pool;
};