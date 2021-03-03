#include"node.h"

Node::Node(std::shared_ptr<node_exec> exec) : m_input_count(0), m_max_thread_number(0), m_bRelease_thread(false) {
    m_cb_func = nullptr;
    m_exec = exec;
}

Node::~Node() {
    ReleaseThread();
}

void Node::setInput(input_type input, node_info_ptr info) {
    if (m_run_mode == thread_mode::shared){
        std::unique_lock<std::mutex> lock(this->m_mutex);
        while (m_input_count > m_max_thread_number)
        {
            m_cond.wait(lock);
        }
        m_input_count++;
        auto &g = flow_thread_pool::GetInstance();
        g.schedule(std::bind((void (Node::*)(std::shared_ptr<void> input, node_info_ptr info)) & Node::thread_loop, this, input, info));
    }
    else{
        //判断线程是否存在,不存在则创建
        if (m_node_Threads.size()==0)
        {
            auto &g = flow_thread_pool::GetInstance();
            for (int i = 0; i < this->m_max_thread_number; i++)
            {
                auto f = std::bind((void (Node::*)()) & Node::thread_loop, this);
                m_node_Threads.push_back(std::make_shared<std::thread>(f));
            }
        }
        std::unique_lock<std::mutex> locker(this->m_mutex);
        while (this->m_input_buf.size() > m_max_input_buf_number){
             this->m_cond.wait(locker);
        }
        input_struct st_input;
        st_input.input = input;
        st_input.info = info;
        this->m_input_buf.push_back(st_input);
        this->m_cond.notify_one();
    }
}

void Node::setCallBack(callback_type func) {
    this->m_cb_func = func;
}

void Node::setNext(std::shared_ptr<Node> next) {
    this->next_node = next;
}

void Node::SetThreadRunMode(thread_mode mode) {
    m_run_mode = mode;
}

void Node::SetNodeType(std::string type) {
    m_strNode_Type = type;
}

std::string Node::GetNodeType() {
    return m_strNode_Type;
}

//设置缓存区长度
void Node::SetInputBufNum(int length) {
    m_max_input_buf_number = length;
}
//设置线程的最大长度
void Node::SetThreadNum(int num) {
    m_max_thread_number = num;
}

void Node::thread_loop(std::shared_ptr<void> input, node_info_ptr info) {
    if (info->status == Node_Info::NodeStatus::BREAK)
    {
        if (m_cb_func)
        {
            m_cb_func(input, info);
        }
        if (next_node)
            this->next_node->setInput(input, info);
    }
    else
    {
        void *ctx = m_exec->GetThreadContext();
        auto output = this->NodeProcess(input, ctx, info);
        m_exec->DestroyThreadContext(ctx);
        if (m_cb_func)
        {
            m_cb_func(output, info);
        }
        if (next_node)
            this->next_node->setInput(output, info);
    }
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_input_count--;
        m_cond.notify_one();
    }
};

void Node::thread_loop() {
    void* ctx = nullptr;
    ctx = m_exec->GetThreadContext();
    while (!m_bRelease_thread) {
        {
            std::unique_lock<std::mutex> locker(this->m_mutex);
            while (this->m_input_buf.size() == 0 && !m_bRelease_thread){
                // this->m_cond.wait_for(locker,std::chrono::milliseconds(100));
                this->m_cond.wait(locker);
            }
            if (m_bRelease_thread){
                break;
            }
        }
        while (true) {
            input_struct input_st;
            {
                std::unique_lock<std::mutex> locker(this->m_mutex);
                if (this->m_input_buf.size() > 0){
                    input_st = this->m_input_buf.front();
                }
                else{
                    break;
                }
                m_input_buf.pop_front();
                m_cond.notify_one();
            }
            if (input_st.info->status == Node_Info::NodeStatus::BREAK)
            {
                if (m_cb_func)
                {
                    m_cb_func(input_st.input, input_st.info);
                }
                if (next_node)
                    this->next_node->setInput(input_st.input, input_st.info);
            }
            else
            {
                auto output = this->NodeProcess(input_st.input, ctx, input_st.info);
                if (m_cb_func)
                {
                    m_cb_func(output, input_st.info);
                }
                if (next_node)
                    this->next_node->setInput(output, input_st.info);
            }
        }
    }
    m_exec->DestroyThreadContext(ctx);
    {
        std::unique_lock<std::mutex> locker(this->m_wait_mutex);
        this->m_max_thread_number--;
        m_wait_thread.notify_one();
    }
}

void Node::ReleaseThread() {
    if (m_run_mode == thread_mode::holdon)
    {
        m_bRelease_thread = true;
        {
            std::unique_lock<std::mutex> locker(this->m_mutex);
            this->m_cond.notify_all();
        }
        std::unique_lock<std::mutex> locker(this->m_wait_mutex);
        while (m_max_thread_number)
        {
            m_wait_thread.wait(locker);
        }
        for(int i = 0;i<m_node_Threads.size();i++){
            if(m_node_Threads[i]->joinable()){
                m_node_Threads[i]->join();
            }
        }
    }
}

std::shared_ptr<void> Node::NodeProcess(std::shared_ptr<void> input, void *ctx, node_info_ptr info) {
    input = m_exec->NodeExec(input,ctx,info);
    return input; 
} 

std::shared_ptr<void> Node::NodeProcessBack(std::shared_ptr<void> input, node_info_ptr info) {
    return input;
}