#pragma once
#include <node_exec.h>
#include<boost/dll/alias.hpp>
#include<core/flow.h>

class mynode :public node_exec{
public:
	typedef anyflow::flow<void>::flow_data_ptr input_type_ptr;
	typedef anyflow::node_info_ptr node_info_ptr;
    static std::shared_ptr<node_exec> CreateNode();
    virtual input_type_ptr NodeExec(input_type_ptr input, void *ctx, node_info_ptr info);
    virtual void* CreateThreadContext();
    virtual void DestroyThreadContext(void* ctx);
};

BOOST_DLL_ALIAS(
    mynode::CreateNode,
    create_node                                       // <-- ...this alias name
)