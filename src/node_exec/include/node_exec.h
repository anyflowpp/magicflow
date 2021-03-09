#pragma once
#include<memory>
#include<magic_func.h>
#include<node_info.h>
#include<core/common/flowtype.hpp>

class MAGIC_E_FUNC node_exec{
public:
    node_exec();
    virtual ~node_exec();
    virtual input_type_ptr  NodeExec(input_type_ptr input, void *ctx, node_info_ptr info);
    virtual void* CreateThreadContext();
    virtual void* GetThreadContext(); 
    virtual void DestroyThreadContext(void* ctx);
private:
    void* m_ctx;
};