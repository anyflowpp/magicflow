#pragma once
#include<memory>
#include"json/json.h"
#include<list>
#include"node_info.h"
#include"thread_pool.h"
#include<condition_variable>
#include<node_exec.h>
#include<magic_func.h>

class MAGIC_E_FUNC Node {
public:

    typedef std::function<void (std::shared_ptr<void>, node_info_ptr)> callback_type;
    typedef std::shared_ptr<void> input_type;
    // flow_w_ptr m_ownedToFlow;
    class input_struct
    {
    public:
        input_type input;
        node_info_ptr info;
    };
    typedef enum
    {
        shared = 0, //线程共享
        holdon = 1  //线程独占
    } thread_mode;

    Node(std::shared_ptr<node_exec> exec);
    virtual ~Node();

    virtual void setInput(input_type input, node_info_ptr info);

    void setCallBack(callback_type func);
    void setNext(std::shared_ptr<Node> next);
    void SetThreadRunMode(thread_mode mode);
    void SetNodeType(std::string type);
    std::string GetNodeType();
    //设置缓存区长度
    void SetInputBufNum(int length);
    //设置线程的最大长度
    void SetThreadNum(int num);
	std::shared_ptr<void> NodeProcess(std::shared_ptr<void> input, void *ctx, node_info_ptr info);
	std::shared_ptr<void> NodeProcessBack(std::shared_ptr<void> input, node_info_ptr info);

protected:
    void thread_loop(std::shared_ptr<void> input, node_info_ptr info);
    void thread_loop();
    void ReleaseThread();
    
    std::string m_strNode_Type;
    int m_input_count;
    std::shared_ptr<Node> next_node;
    std::mutex m_mutex;
    std::condition_variable m_cond;
    callback_type m_cb_func;
    //针对多线程独占线程进行的修改
    std::vector<std::shared_ptr<std::thread>> m_node_Threads;
    bool m_bRelease_thread;
    std::list<input_struct> m_input_buf; //待处理消息队列
    thread_mode m_run_mode;
    int m_max_input_buf_number;
    int m_max_thread_number;
    std::mutex m_wait_mutex;
    std::condition_variable m_wait_thread;
    std::shared_ptr<node_exec> m_exec;
};