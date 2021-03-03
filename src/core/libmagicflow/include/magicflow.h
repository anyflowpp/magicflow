#pragma once
#include<memory>
#include"json/json.h"
#include<list>
#include"magicflowcfg.h"
#include"node.h"
#include"thread_pool.h"
#include"magic_func.h"

class MAGIC_E_FUNC flow{
public:
    typedef std::shared_ptr<Node> RootNode_ptr;
    typedef std::function<void (std::shared_ptr<void>, node_info_ptr)> callback_type;
    
    typedef std::weak_ptr<flow> flow_w_ptr;
    typedef std::shared_ptr<Node> RootNode_ptr;
    typedef std::function<RootNode_ptr(const Json::Value&)> genfunc; 

    typedef RootNode_ptr(*GenFuncType_C)(const Json::Value&);
    typedef RootNode_ptr(GenNodeOutWay_C)(const Json::Value& name);
    typedef std::function<GenNodeOutWay_C> GenNodeOutWay_STD;

    static std::shared_ptr<flow> GetInstance(const Json::Value& flowcfg,GenNodeOutWay_STD OutGenner);

    virtual ~flow();

	flow(const Json::Value& flowcfg, GenNodeOutWay_STD OutGenner);
    flow(const Json::Value& flowcfg);

    void SetInput(std::shared_ptr<void> input,node_info_ptr info);
    void SetFlowBack(std::shared_ptr<void> input, node_info_ptr info); 

    void IncraseInputCount();

    void SetCallBack(callback_type func);
    void lastNodeCallBack(std::shared_ptr<void> input, node_info_ptr info);

    std::vector<std::shared_ptr<void>> GetOutput();
   
    void SetCfg(flowcfg cfg);
    
private:
    flowcfg m_flowcfg;
	std::vector<std::shared_ptr<void>> m_output;
    callback_type  m_callback;
    int m_input_count;
    std::mutex m_input_count_mutex;
    std::condition_variable m_input_count_cond;
	std::list<RootNode_ptr> m_nodes;
};