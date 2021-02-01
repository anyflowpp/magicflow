#pragma once
#include"common/rootnode.hpp"
#include<json/json.h>
#include<megface-c/tracker_context.h>
class TrackNode:public RootNode{
public:
    TrackNode(const Json::Value& cfg);
    ~TrackNode();
    MGFTrackerSettings setting;
    static std::shared_ptr<TrackNode> GenNode(const Json::Value& cfg);
private:
     virtual flow_data_ptr NodeProcess(flow_data_ptr input,void* ctx, node_info_ptr info);
};