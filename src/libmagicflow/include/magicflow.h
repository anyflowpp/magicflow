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

    static std::shared_ptr<flow> GetInstance(const Json::Value& flowcfg,GenNodeOutWay_STD OutGenner);

    virtual ~flow();

	flow(const Json::Value& flowcfg, GenNodeOutWay_STD OutGenner);
    flow(const Json::Value& flowcfg);

    void SetInput(input_type_ptr input,node_info_ptr info);
    void SetFlowBack(input_type_ptr input, node_info_ptr info); 

    void IncraseInputCount();

    void SetCallBack(callback_type func);
    void lastNodeCallBack(input_type_ptr input, node_info_ptr info);

    std::vector<input_type_ptr> GetOutput();
   
    void SetCfg(flowcfg cfg);
    
};