#include<vector>
#include<iostream>
#include<memory>

class ffmpeg_input_struct{
public:
    std::vector<char> data;
    int w;
    int h;
    int c;
};
typedef std::shared_ptr<ffmpeg_input_struct> ffmpeg_input_struct_ptr;