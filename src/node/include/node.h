#pragma once
#include<memory>
#include"json/json.h"
#include<list>
#include"core/flow.h"
#include"thread_pool.h"
#include<condition_variable>
#include<node_exec.h>
#include<magic_func.h>
#include<map>

typedef anyflow::flow<void>::Node Node;