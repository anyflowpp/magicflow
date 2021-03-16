#pragma once
#include <node_exec/include/node_exec.h>
#include<boost/dll/alias.hpp>
extern "C"{
#include<libavcodec/codec.h>
#include<libavformat/avformat.h>
#include<libswscale/swscale.h>
#include<libavdevice/avdevice.h>
}


class FFmpegInput :public node_exec{
public:
    FFmpegInput();
    ~FFmpegInput();
	typedef anyflow::flow<void>::flow_data_ptr input_type_ptr;
	typedef anyflow::node_info_ptr node_info_ptr;
    static std::shared_ptr<node_exec> CreateNode();
    virtual input_type_ptr NodeExec(input_type_ptr input, void *ctx, node_info_ptr info)override;
    virtual void* CreateThreadContext();
    virtual void DestroyThreadContext(void* ctx);
private:
    AVInputFormat *video_device;
    AVFormatContext *pFmtCtx;
    AVCodecContext* pCodeCtx;
    AVCodec* pCode;
    AVFrame * pFrame ;
    AVPacket *package ;
    int video_index; 
    struct SwsContext *sws_context;
};

BOOST_DLL_ALIAS(
    FFmpegInput::CreateNode,
    create_node                                       // <-- ...this alias name
)