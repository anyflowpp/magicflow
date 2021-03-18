#pragma once
#include<memory>
#include<common/magic_func.h>
#include<common/node_info.h>
#include<core/flow.h>

class MAGIC_E_FUNC node_exec:public anyflow::node_exec{
public:
    node_exec();
    virtual ~node_exec();
};