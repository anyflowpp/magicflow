#pragma once
#include <core/node_exec/include/node_exec.h>
#include<boost/dll/alias.hpp>


class mynode :public node_exec{
public:
    static std::shared_ptr<node_exec> CreateNode();
    virtual std::shared_ptr<void> NodeExec(std::shared_ptr<void> input, void *ctx, node_info_ptr info);
    virtual void* CreateThreadContext();
    virtual void DestroyThreadContext(void* ctx);
};

BOOST_DLL_ALIAS(
    mynode::CreateNode,
    create_node                                       // <-- ...this alias name
)