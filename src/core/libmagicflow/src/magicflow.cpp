#include"magicflow.h"

std::shared_ptr<flow> flow::GetInstance(const Json::Value& flowcfg,GenNodeOutWay_STD OutGenner){
	std::shared_ptr<flow> ret = std::make_shared<flow>(flowcfg,OutGenner);
    for(auto it = ret->m_nodes.begin();it!=ret->m_nodes.end();++it){
        //(*it)->setOwnedFlow(ret);
    }
    return ret;
}

flow::~flow(){
    std::unique_lock<std::mutex> locker(m_input_count_mutex);
    while(m_input_count>0){
        m_input_count_cond.wait(locker);
        // m_input_count_cond.wait_for(locker,std::chrono::milliseconds(100));
    }
    m_nodes.clear();
}
flow::flow(const Json::Value& flowcfg,GenNodeOutWay_STD OutGenner):m_input_count(0){
    //TODO: 处理找不到节点的情况
    m_nodes.clear();
    Json::Value flow = flowcfg["flow"];
    for(auto i = flow.begin();i!=flow.end();++i){
        auto jnode = *i;
        std::string nodetype = jnode["type"].asString();
        std::shared_ptr<Node> node;
        auto func = OutGenner;
        node = func(jnode);
        if(!node){
            continue;
        }
        if(m_nodes.size()!=0){
            auto last = m_nodes.back();
            last->setNext(node);
        }
        m_nodes.push_back(node);
    }
    if(m_nodes.size()>0){
        auto node = m_nodes.back();
        node->setCallBack(std::bind(&flow::lastNodeCallBack,this,std::placeholders::_1,std::placeholders::_2));
    }
}

void flow::SetInput(std::shared_ptr<void> input,node_info_ptr info){
    {
        std::unique_lock<std::mutex> locker(m_input_count_mutex);
        m_input_count += 1;
    }
    auto& first = m_nodes.front();
    first->setInput(input,info);
}
void flow::SetFlowBack(std::shared_ptr<void> input, node_info_ptr info) {
    auto it = m_nodes.end();
    --it;
    while(it!=m_nodes.begin()){
        input = (*it)->NodeProcessBack(input,info);
        --it;
    }
    input = (*it)->NodeProcessBack(input,info);
}

void flow::IncraseInputCount(){
    std::unique_lock<std::mutex> locker(m_input_count_mutex);
    m_input_count+=1;
}

void flow::SetCallBack(callback_type func){
    if(m_nodes.size()>0){
        auto node = m_nodes.back();
        node->setCallBack(std::bind(&flow::lastNodeCallBack,this,std::placeholders::_1,std::placeholders::_2));
    }
    m_callback = func;
}
void flow::lastNodeCallBack(std::shared_ptr<void> input, node_info_ptr info){
    {
        std::unique_lock<std::mutex> locker(m_input_count_mutex);
        m_input_count--;
        if(m_flowcfg.m_mode==flowcfg::microsync || m_flowcfg.m_mode==flowcfg::sync){
            m_output.push_back(input);
        }
        if(m_input_count<=0){
            m_input_count_cond.notify_all();
        }
        if(m_flowcfg.m_cb_mode==flowcfg::async){
            locker.unlock();
        }
        if(this->m_callback){
            m_callback(input,info);
        }
        this->SetFlowBack(input,info);
    }
}

std::vector<std::shared_ptr<void>> flow::GetOutput(){
	std::vector<std::shared_ptr<void>> ret;
    std::unique_lock<std::mutex> locker(m_input_count_mutex);
    if(m_flowcfg.m_mode==flowcfg::microsync){
        while(m_input_count>0){
            m_input_count_cond.wait(locker);
        }
    }
    ret = m_output;
    m_output.clear();
    return ret;
}
void flow::SetCfg(flowcfg cfg){
    m_flowcfg = cfg;
    if(m_flowcfg.m_mode == flowcfg::sync){
        for(auto i = m_nodes.begin();i!=m_nodes.end();i++){
            (*i)->SetThreadNum(1);
        }
    }
}