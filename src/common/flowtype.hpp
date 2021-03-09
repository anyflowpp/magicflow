#pragma once
#include<map>
#include<memory>
#include<iostream>

typedef std::map<std::string,std::shared_ptr<void>> input_type;
typedef std::shared_ptr<input_type> input_type_ptr;