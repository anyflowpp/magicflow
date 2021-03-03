#pragma once
#include<functional>
#include "flowSQ.h"
#include<mutex>
#include <thread>
#include <future>
// #include <utility>

class std_impl_thread_pool {
private:
    class ThreadWorker {
    private:
        int m_id;
        std_impl_thread_pool * m_pool;
    public:
        ThreadWorker(std_impl_thread_pool * pool, const int id)
        : m_pool(pool), m_id(id) {
        }

        void operator()() {
        std::function<void()> func;
        bool dequeued;
        while (!m_pool->m_shutdown) {
            {
            std::unique_lock<std::mutex> lock(m_pool->m_conditional_mutex);
            if (m_pool->m_queue.empty()) {
                m_pool->m_conditional_lock.wait(lock);
            }
            dequeued = m_pool->m_queue.dequeue(func);
            }
            if (dequeued) {
            func();
            }
        }
        }
    };

bool m_shutdown;
flowSQ<std::function<void()>> m_queue;
std::vector<std::thread> m_threads;
std::mutex m_conditional_mutex;
std::condition_variable m_conditional_lock;
public:
std_impl_thread_pool(const int n_threads)
    : m_threads(std::vector<std::thread>(n_threads)), m_shutdown(false) {
    init();
}

std_impl_thread_pool(const std_impl_thread_pool &) = delete;
std_impl_thread_pool(std_impl_thread_pool &&) = delete;

std_impl_thread_pool & operator=(const std_impl_thread_pool &) = delete;
std_impl_thread_pool & operator=(std_impl_thread_pool &&) = delete;

void init() {
    for (int i = 0; i < m_threads.size(); ++i) {
    m_threads[i] = std::thread(ThreadWorker(this, i));
    }
}

void shutdown() {
    m_shutdown = true;
    m_conditional_lock.notify_all();
    for (int i = 0; i < m_threads.size(); ++i) {
    if(m_threads[i].joinable()) {
        m_threads[i].join();
    }
    }
}

    template<typename F, typename...Args>
    void schedule(F&& f, Args&&... args){
        std::function<decltype(f(args...))()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);
        std::function<void()> wrapper_func = [task_ptr]() {
            (*task_ptr)(); 
        };
        m_queue.enqueue(wrapper_func);
        m_conditional_lock.notify_one();
        task_ptr->get_future();
        return ;
    }
};