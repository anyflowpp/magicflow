#pragma once
#include"common/rootnode.hpp"
#include<json/json.h>
#include<megface-c/tracker_context.h>
#include <boost/config.hpp> // for BOOST_SYMBOL_EXPORT
#include<boost/dll/alias.hpp>

namespace kag{
    class DetectNode:public RootNode{
    public:
        DetectNode(const Json::Value& cfg);
        ~DetectNode();
        MGFDetectorSettings setting;
        static std::shared_ptr<DetectNode> GenNode(const Json::Value&);
    private:
        virtual flow_data_ptr NodeProcess(flow_data_ptr input,void* ctx, node_info_ptr info);
        virtual flow_data_ptr NodeProcessBack(flow_data_ptr input,node_info_ptr info);
    };

    BOOST_DLL_ALIAS(
        kag::DetectNode::GenNode, // <-- this function is exported with...
        create_plugin             // <-- ...this alias name
    )
}