#include"node-template.h"
#include<spdlog_wrap.h>
#include<memory>
typedef anyflow::flow<void>::flow_data_ptr input_type_ptr;
typedef anyflow::node_info_ptr node_info_ptr;

input_type_ptr mynode::NodeExec(input_type_ptr _input, void *ctx, node_info_ptr info){

	typedef std::map<
		std::string,
		std::shared_ptr<void>
	> input_Type;
	std::shared_ptr< std::map<
		std::string,
		std::shared_ptr<void>
	>
	> input = std::static_pointer_cast<input_Type>(_input);
    auto idata = input->find("input");
    std::shared_ptr<std::string> istring = std::static_pointer_cast<std::string>(idata->second);
    int i = 101;
    int sum = 0;
    while(i--){
        logw("input:{},{}",i,*istring);
        sum +=i;
    }
    auto mynodeout=std::make_shared<int>(sum);
    input->insert(std::pair<std::string,std::shared_ptr<void>>("mynode",mynodeout));
    std::shared_ptr<void> ret = (input);
    return _input;
}
void* mynode::CreateThreadContext(){
    return nullptr;
}
void mynode::DestroyThreadContext(void* ctx){}
std::shared_ptr<node_exec> mynode::CreateNode(){
    return std::make_shared<mynode>();
}