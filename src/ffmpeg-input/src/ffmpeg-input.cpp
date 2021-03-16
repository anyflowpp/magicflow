#include"ffmpeg-input.h"
#include<spdlog_wrap.h>
#include<memory>
#include"ffmpeg-input-struct.h"
#include<thread>
#include<sstream>

typedef anyflow::flow<void>::flow_data_ptr input_type_ptr;
typedef anyflow::node_info_ptr node_info_ptr;

FFmpegInput::~FFmpegInput(){
    avcodec_close(pCodeCtx);
    avformat_close_input(&pFmtCtx);
    avformat_free_context(pFmtCtx);
    sws_freeContext(sws_context);
}

FFmpegInput::FFmpegInput(){
    avdevice_register_all();
    video_device = nullptr;
    do{
        video_device = av_input_video_device_next(video_device);
    // }while(video_device);
    }while(false);

    pFmtCtx = avformat_alloc_context();

    auto open_ret = avformat_open_input(&pFmtCtx,"video=Full HD webcam",video_device,nullptr);
    auto find_stream_info_ret = avformat_find_stream_info(pFmtCtx,NULL);
    video_index = -1;
    for(int i = 0;i<pFmtCtx->nb_streams;i++){
        if(pFmtCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO){
            video_index = i;
            break;
        }
    }
    pCodeCtx = pFmtCtx->streams[video_index]->codec;
    pCode = avcodec_find_decoder(pCodeCtx->codec_id);
    auto codec_open_ret = avcodec_open2(pCodeCtx,pCode,NULL);
    
    
    sws_context = sws_getContext(pCodeCtx->width,
                   pCodeCtx->height,
                   pCodeCtx->pix_fmt,
                   pCodeCtx->width,
                   pCodeCtx->height,
                   AVPixelFormat::AV_PIX_FMT_BGR24,
                   SWS_BICUBIC,
                   NULL,NULL,NULL
                   );
}

input_type_ptr FFmpegInput::NodeExec(input_type_ptr _input, void *ctx, node_info_ptr info){
    static std::mutex s_mutex;
    std::lock_guard<std::mutex> locker(s_mutex);

	pFrame = av_frame_alloc();
	package = av_packet_alloc();
	auto this_thread_id = std::this_thread::get_id();
	std::stringstream ss;
	ss << this_thread_id;
	std::string this_id_string;
	ss >> this_id_string;
	logw("ffmpeg thread id:{}", this_id_string);
	typedef std::map<
		std::string,
		std::shared_ptr<void>
	> input_Type;
	std::shared_ptr< std::map<
		std::string,
		std::shared_ptr<void>
	>
	> input = std::static_pointer_cast<input_Type>(_input);
    if(!input){
        return input;
    }
    auto idata = input->find("input");
    std::shared_ptr<std::string> istring = std::static_pointer_cast<std::string>(idata->second);
    auto read_frame_ret = av_read_frame(pFmtCtx,package);
    int got_picture;
	
    if(package->stream_index==video_index){
        avcodec_decode_video2(pCodeCtx,pFrame,&got_picture,package);
    }
    
    if(got_picture){
		auto item_ptr = std::make_shared<ffmpeg_input_struct>();
        AVFrame *pFrameBGR = av_frame_alloc();
        int bgr_size = avpicture_get_size(AV_PIX_FMT_BGR24,pCodeCtx->width,pCodeCtx->height);
        uint8_t *bgr_buf = (uint8_t*)av_malloc(bgr_size);
        avpicture_fill((AVPicture*)pFrameBGR,bgr_buf,AV_PIX_FMT_BGR24,pCodeCtx->width,pCodeCtx->height);

        sws_scale(sws_context,
                  pFrame->data,pFrame->linesize,0,
                  pCodeCtx->height,pFrameBGR->data,pFrameBGR->linesize);

        char* begin_addr = (char*)pFrameBGR->data[0];

        item_ptr->w = this->pCodeCtx->width;
        item_ptr->h = this->pCodeCtx->height;
        item_ptr->c = 3;
        item_ptr->data.assign(begin_addr,begin_addr+bgr_size);
        av_free(bgr_buf);
        av_frame_free(&pFrameBGR);
		input->insert(std::pair<std::string, std::shared_ptr<void>>("ffmpeginput", item_ptr));
    } 
	av_frame_free(&pFrame);
    // av_packet_unref(package);
	av_packet_free(&package);
    return input;
}
void* FFmpegInput::CreateThreadContext(){
    return nullptr;
}
void FFmpegInput::DestroyThreadContext(void* ctx){}

std::shared_ptr<node_exec> FFmpegInput::CreateNode(){
    return std::make_shared<FFmpegInput>();
}