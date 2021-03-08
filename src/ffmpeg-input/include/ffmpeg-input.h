#pragma once
#include <core/node_exec/include/node_exec.h>
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
    static std::shared_ptr<node_exec> CreateNode();
    virtual input_type_ptr NodeExec(input_type_ptr input, void *ctx, node_info_ptr info);
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