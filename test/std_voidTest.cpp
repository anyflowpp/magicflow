#include <gtest/gtest.h>
#include <spdlog_wrap.hpp>

class A{
public:
    A(){}
    virtual void Name(){
        logw("");
    }
    ~A(){
        logw("");
    }
};

class B:public A{
public:
    B(){}
    void Name(){
        logw("");
    }
    ~B(){
        logw("");
    }
};
TEST(std,t1){
    std::shared_ptr<void> p = std::make_shared<B>();
    std::shared_ptr<A> ap = std::static_pointer_cast<B>(p);
    ap->Name();
    std::shared_ptr<B> bp = std::static_pointer_cast<B>(p);
    bp->Name();
};