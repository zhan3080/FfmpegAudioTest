// #include <jni.h>

#include "logger.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavfilter/avfilter.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
#include "libavutil/samplefmt.h"
//#include "OpenSLRender.h"


#include <pthread.h>

#include "libavutil/pixfmt.h"

#define TAG "ffmpeg_audioPlayer"
#define DEBUG true


extern void pcm_write(uint8_t *out_buffer, int out_buffer_size);
extern void create_audiotrack(void);

#define TEST_URL "/sdcard/t.mp4"
#define OUTPUT_FILE "/sdcard/output.pcm"
FILE *file = NULL;

AVCodecContext *acodec_ctx;
AVCodec *acodec;
SwrContext   *m_SwrContext;

//dst frame data size
// 音频编码采样率
static const int AUDIO_DST_SAMPLE_RATE = 44100;
// 音频编码通道数
static const int AUDIO_DST_CHANNEL_COUNTS = 2;
// 音频格式
const enum AVSampleFormat DST_SAMPLT_FORMAT = AV_SAMPLE_FMT_S16;
// 音频编码声道格式
static const uint64_t AUDIO_DST_CHANNEL_LAYOUT = AV_CH_LAYOUT_STEREO;
// ACC音频一帧采样数
static const int ACC_NB_SAMPLES = 1024;
//number of sample per channel
int m_nbSamples = 0;

//dst frame data size
int m_DstFrameDataSze = 0;
uint8_t *m_AudioOutBuffer = NULL;

//设置重采样参数
void initAudio(AVCodecContext *codeCtx)
{

    m_SwrContext = swr_alloc();

    av_opt_set_int(m_SwrContext, "in_channel_layout", codeCtx->channel_layout, 0);
    av_opt_set_int(m_SwrContext, "out_channel_layout", AUDIO_DST_CHANNEL_LAYOUT, 0);

    av_opt_set_int(m_SwrContext, "in_sample_rate", codeCtx->sample_rate, 0);
    av_opt_set_int(m_SwrContext, "out_sample_rate", AUDIO_DST_SAMPLE_RATE, 0);

    av_opt_set_sample_fmt(m_SwrContext, "in_sample_fmt", codeCtx->sample_fmt, 0);
    av_opt_set_sample_fmt(m_SwrContext, "out_sample_fmt", DST_SAMPLT_FORMAT,  0);

    swr_init(m_SwrContext);
    LogI(TAG, DEBUG, "initAudioaudio metadata sample rate: %d, channel: %d, format: %d, frame_size: %d, layout: %lld",
             codeCtx->sample_rate, codeCtx->channels, codeCtx->sample_fmt, codeCtx->frame_size,codeCtx->channel_layout);
    // resample params
    m_nbSamples = (int)av_rescale_rnd(ACC_NB_SAMPLES, AUDIO_DST_SAMPLE_RATE, codeCtx->sample_rate, AV_ROUND_UP);
    LogE(TAG, DEBUG, "initAudio [m_nbSamples, m_DstFrameDataSze]=[%d]", m_nbSamples);
    m_DstFrameDataSze = av_samples_get_buffer_size(NULL, AUDIO_DST_CHANNEL_COUNTS,m_nbSamples, DST_SAMPLT_FORMAT, 1);

    LogE(TAG, DEBUG, "initAudio [m_nbSamples, m_DstFrameDataSze]=[%d, %d]", m_nbSamples, m_DstFrameDataSze);

    m_AudioOutBuffer = (uint8_t *) malloc(m_DstFrameDataSze);
}

void OnFrameAvailable(AVCodecContext *acodec_ctx, AVFrame *frame)
{
    LogI(TAG, DEBUG, "OnFrameAvailable frame=%p, frame->nb_samples=%d", frame, frame->nb_samples);
    if(file != NULL)
    {
        // 重采样后保存音频，也没问题（这个大小有点疑问m_DstFrameDataSze / 2）
        int result = swr_convert(m_SwrContext, &m_AudioOutBuffer, m_DstFrameDataSze / 2, (const uint8_t **) frame->data, frame->nb_samples);
        //fwrite(m_AudioOutBuffer, 1, m_DstFrameDataSze, file);
        pcm_write(m_AudioOutBuffer, m_DstFrameDataSze);
    }
    //LogI(TAG, DEBUG, "OnFrameAvailable frame=%p, frame->nb_samples=%d", frame, frame->nb_samples);
    /*if(file != NULL)
    {
        m_DstFrameDataSze = av_get_bytes_per_sample(acodec_ctx->sample_fmt);
        //LogI(TAG, DEBUG, "OnFrameAvailable acodec_ctx->sample_fmt=%d, m_DstFrameDataSze=%d", acodec_ctx->sample_fmt, m_DstFrameDataSze);
        if (m_DstFrameDataSze < 0) {
            //This should not occur, checking just for paranoia
            LogE(TAG, DEBUG, "Failed to calculate data size");
            return;
        }
        for (int i = 0; i < frame->nb_samples; i++)
        {
            for (int ch = 0; ch < acodec_ctx->channels; ch++)  // 交错的方式写入, 大部分float的格式输出
                fwrite(frame->data[ch] + m_DstFrameDataSze*i, 1, m_DstFrameDataSze, file);
        }

    }*/

}

void* decode_audio(void *argv)
{
    LogI(TAG, DEBUG, "decode_audio begin");
    int i = 0;
    int err = 0;
    int framecnt = 0;

    AVFormatContext *fmt_ctx = NULL;
    AVPacket *m_Packet;
    AVFrame *m_Frame;
    int audio_stream_index = -1;

    avfilter_register_all();
    av_register_all();
    avformat_network_init();
    //1.创建封装格式上下文
    fmt_ctx = avformat_alloc_context();

    //2.打开输入文件，解封装
    err = avformat_open_input(&fmt_ctx, TEST_URL, NULL, NULL);
    if(err < 0)
    {
        LogE(TAG, DEBUG, "decode_audio avformat_open_input fail err:" + err);
        goto failure;
    }
    LogI(TAG, DEBUG, "decode_audio avformat_find_stream_info");
    //3.获取音视频流信息
    if((err == avformat_find_stream_info(fmt_ctx,NULL)) < 0)
    {
        LogE(TAG, DEBUG, "decode_audio avformat_find_stream_info fail err:" + err);
        goto failure;
    }

    //4.获取音视频流索引
    for(i = 0; i < fmt_ctx->nb_streams; i++)
    {
        LogE(TAG, DEBUG, "decode_audio codec_type %d",fmt_ctx->streams[i]->codec->codec_type);
        if(fmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            audio_stream_index = i;
        }
    }
    LogE(TAG, DEBUG, "decode_audio audio_stream_index %d",audio_stream_index);

    if(-1 != audio_stream_index)
    {

        //5.创建解码器上下文    
        acodec_ctx = fmt_ctx->streams[audio_stream_index]->codec;
        //6.获取解码器
        acodec = avcodec_find_decoder(acodec_ctx->codec_id);

        if(avcodec_open2(acodec_ctx,acodec, NULL) < 0)
        {
            LogE(TAG, DEBUG, "decode_audio avcodec_open2 fail");
            goto failure;
        }

        if((file=fopen(OUTPUT_FILE,"wb+"))==NULL)
        {
            LogE(TAG, DEBUG, "decode_audio fopen error");
        }
        LogI(TAG, DEBUG, "initAudioaudio metadata sample rate: %d, channel: %d, format: %d, frame_size: %d, layout: %lld",
             acodec_ctx->sample_rate, acodec_ctx->channels, acodec_ctx->sample_fmt, acodec_ctx->frame_size,acodec_ctx->channel_layout);

        initAudio(acodec_ctx);
        create_audiotrack(); // audioTrackPlayerNative
        //9.创建存储编码数据和解码数据的结构体
        m_Packet = av_packet_alloc(); //创建 AVPacket 存放编码数据
        m_Frame = av_frame_alloc(); //创建 AVFrame 存放解码后的数据
        //10.解码循环
        while (av_read_frame(fmt_ctx, m_Packet) >= 0) 
        { //读取帧
            if (m_Packet->stream_index == audio_stream_index) 
            {
                if (avcodec_send_packet(acodec_ctx, m_Packet) != 0) 
                { //视频解码
                    LogE(TAG, DEBUG, "decode_audio avcodec_send_packet fail");
                    goto failure;
                }
                while (avcodec_receive_frame(acodec_ctx, m_Frame) == 0) 
                {
                    OnFrameAvailable(acodec_ctx,m_Frame);
                }
                av_packet_unref(m_Packet);//释放 m_Packet 引用，防止内存泄漏
                //LogI(TAG, DEBUG, "decode_audio av_init_packet");
                av_init_packet(m_Packet);
            }
        
            //LogI(TAG, DEBUG, "decode_audio usleep");
            usleep(10000);
        }
    }


    failure:

    if(file != NULL)
    {
        fclose(file);
    }
    //11.释放资源，解码完成
    if(m_Frame != NULL) {
        av_frame_free(&m_Frame);
        m_Frame = NULL;
    }

    if(m_Packet != NULL) {
        av_packet_free(&m_Packet);
        m_Packet = NULL;
    }

    if(acodec_ctx) {
        avcodec_close(acodec_ctx);
        avcodec_free_context(&acodec_ctx);
        acodec_ctx = NULL;
        acodec = NULL;
    }

    if(fmt_ctx)
    {
        avformat_close_input(&fmt_ctx);
        avformat_free_context(fmt_ctx);
    }
    avformat_network_deinit();
    LogI(TAG, DEBUG, "decode_audio end");
    return 0;
}