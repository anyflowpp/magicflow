#include<stdlib.h>
#include<functional>
#include<string.h>
#include<gtest/gtest.h>
#include"core/node.h"
#include<thread>
#include<spdlog_wrap.h>
#include<chrono>
#include<functional>

class context{
public:
    context(){a=0;}
    int a;
    int b;
    float c;
};

typedef std::function<void (std::shared_ptr<context>, node_info_ptr)> callback_type;
class cNode:public flow<context>::Node{
public:
    cNode(){}
    cNode(callback_type func):cb_func(func){}
    virtual ~cNode(){}
private:
    callback_type cb_func;
    std::shared_ptr<context> NodeProcess(std::shared_ptr<context> input,std::shared_ptr<Node_Info> info){
        input->a++;
        input->a++;
        if(cb_func){
            cb_func(input,info);
        }
        return input;
    }
};

void back(std::shared_ptr<context> i,std::shared_ptr<Node_Info> f){
    logi("terminal result:{}",i->a);
}

TEST(NodeTest,T1){
    auto n1 = std::make_shared<cNode>();
    auto n2 = std::make_shared<cNode>();
    auto n3 = std::make_shared<cNode>(back);
    n1->setNext(n2);
    n2->setNext(n3);

    auto c1 = std::make_shared<context>();
    auto f1 = std::make_shared<Node_Info>();
    n1->setInput(c1,f1);
    c1 = std::make_shared<context>();
    n1->setInput(c1,f1);
    c1 = std::make_shared<context>();
    n1->setInput(c1,f1);
    c1 = std::make_shared<context>();
    n1->setInput(c1,f1);
    c1 = std::make_shared<context>();
    n1->setInput(c1,f1);
    c1 = std::make_shared<context>();
    n1->setInput(c1,f1);
    auto &g = flow_thread_pool::GetInstance();
    // cNode::StopFlow();
    logi("g.join");
    n1->setInput(c1,f1);
    logi("g.join");
    g.wait();
};