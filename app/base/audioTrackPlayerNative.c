
#include "logger.h"
#include <jni.h>
#include "jni_utils.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavfilter/avfilter.h"
#include "libswscale/swscale.h"

#define TAG "audioTrackPlayer"
#define DEBUG true


static jmethodID g_jMethodIdCreateAudioTrack = NULL;
static jmethodID g_jMethodIdAudioTrackPlay = NULL;
static jmethodID g_jMethodIdAudioTrackWrite = NULL;
static jobject audio_track = NULL;
static const int AUDIO_DST_SAMPLE_RATE = 44100;
// 音频编码通道数
static const int AUDIO_DST_CHANNEL_COUNTS = 2;
static JNIEnv *g_jEnv = NULL;
//这里保存全局变量，可以多线程共享
static jobject g_mObject = NULL;


void create_audiotrack(void)
{
    LogE(TAG, DEBUG, "create\n");
    JNIEnv *pEnv = NULL;
    pEnv = ff_jni_get_env();
    //直接调用系统类AudioTrack
    jclass cls = (*pEnv)->FindClass(pEnv, "android/media/AudioTrack");
    LogE(TAG, DEBUG, "create: NewObject1");
    jmethodID g_jMethodIdInit = (*pEnv)->GetMethodID(pEnv, cls, "<init>",
                                                           "(IIIIII)V");
    LogE(TAG, DEBUG, "create: NewObject2");
    jmethodID g_jMethodGetMinBufferSize = (*pEnv)->GetMethodID(pEnv, cls, "getMinBufferSize",
                                                           "(III)I");
    int bufferSizeInBytes = (*pEnv)->CallStaticIntMethod(pEnv, cls, g_jMethodGetMinBufferSize,AUDIO_DST_SAMPLE_RATE,(0x4 | 0x8),2);
    LogE(TAG, DEBUG, "create: NewObject");

    g_mObject = (*pEnv)->NewObject(pEnv, cls, g_jMethodIdInit,
                               3, AUDIO_DST_SAMPLE_RATE, (0x4 | 0x8), 2, bufferSizeInBytes, 1);


    //这里保存全局变量，可以就可以多线程共享了
    //g_mObject = (*pEnv)->NewGlobalRef(pEnv, audio_track);
    //jclass audio_track_class = (*pEnv)->GetObjectClass(pEnv, audio_track);
    jclass audio_track_class = (*pEnv)->GetObjectClass(pEnv, g_mObject);

    
    g_jMethodIdAudioTrackPlay = (*pEnv)->GetMethodID(pEnv, audio_track_class, "play", "()V");
    if (!g_jMethodIdAudioTrackPlay)
    {
        LogE(TAG, DEBUG, "get method: play failed.");
    }
 
    (*pEnv)->CallVoidMethod(pEnv, g_mObject, g_jMethodIdAudioTrackPlay);
    LogE(TAG, DEBUG, "create: play");
    if(g_jMethodIdAudioTrackWrite == NULL)
    {
        g_jMethodIdAudioTrackWrite = (*pEnv)->GetMethodID(pEnv, audio_track_class, "write","([BII)I");
    }
    LogE(TAG, DEBUG, "create end\n");

}
  

void pcm_write(uint8_t *out_buffer, int out_buffer_size){
    LogI(TAG, DEBUG, "pcm_write out_buffer_size:%d",out_buffer_size);
    JNIEnv *env = NULL;
    env = ff_jni_get_env();
  
    LogI(TAG, DEBUG, "pcm_write out_buffer 缓冲数据，转成byte数组");
    //out_buffer缓冲数据，转成byte数组
    jbyteArray data_array = (*env)->NewByteArray(env, out_buffer_size);
    jbyte *sample_byte = (*env)->GetByteArrayElements(env, data_array, NULL);
    LogI(TAG, DEBUG, "pcm_write out_buffer 赋值");
    //赋值
    memcpy(sample_byte, out_buffer, out_buffer_size);
    LogI(TAG, DEBUG, "pcm_write out_buffer 赋值---");
    //同步
    (*env)->ReleaseByteArrayElements(env, data_array, sample_byte, 0);
    LogE(TAG, DEBUG, "pcm_write write:%d",out_buffer_size);
    if(g_jMethodIdAudioTrackWrite == NULL)
    {
        jclass audio_track_class = (*env)->GetObjectClass(env, g_mObject);
        g_jMethodIdAudioTrackWrite = (*env)->GetMethodID(env, audio_track_class, "write","([BII)I");
    }
    LogE(TAG, DEBUG, "pcm_write write --- :%d",out_buffer_size);
    //将pcm的数据写入AudioTrack
    (*env)->CallIntMethod(env, g_mObject, g_jMethodIdAudioTrackWrite, data_array, 0,
                                      out_buffer_size);

    LogE(TAG, DEBUG, "pcm_write write end");
    //释放局部引用
    (*env)->DeleteLocalRef(env, data_array);

}

