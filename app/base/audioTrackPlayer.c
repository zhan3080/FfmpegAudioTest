
#include "logger.h"
#include <jni.h>
#include "jni_utils.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavfilter/avfilter.h"
#include "libswscale/swscale.h"

#define TAG "audioTrackPlayer"
#define DEBUG true

#define JNI_CLASS_AUDIOTRACK_PLAYER     "com/example/ffmepgtest/pcm/application/audioTrackPlayer"


static void create(JNIEnv *pEnv, jobject jobjPlayer);

static jmethodID g_jMethodIdCreateAudioTrack = NULL;
static jmethodID g_jMethodIdAudioTrackPlay = NULL;
static jmethodID g_jMethodIdAudioTrackWrite = NULL;
static jobject audio_track = NULL;
static const int AUDIO_DST_SAMPLE_RATE = 44100;
// 音频编码通道数
static const int AUDIO_DST_CHANNEL_COUNTS = 2;
static JNIEnv *g_jEnv = NULL;
static jobject g_mObject = NULL;


static JNINativeMethod g_methods[] =
{
    {"_create",             "()V",       (void *)create},
};

static void create(JNIEnv *pEnv, jobject jobjPlayer)
{
    LogE(TAG, DEBUG, "create\n");
    jclass cls = (*pEnv)->FindClass(pEnv, JNI_CLASS_AUDIOTRACK_PLAYER);
    //调用java层audioTrackPlayer（封装了AudioTrack）
    g_jMethodIdCreateAudioTrack = (*pEnv)->GetMethodID(pEnv, cls, "createAudioTrack",
                                                           "(II)Landroid/media/AudioTrack;");
    if (!g_jMethodIdCreateAudioTrack)
    {
        LogE(TAG, DEBUG, "get method: onNotifyFromNative failed.");
    }
    audio_track = (*pEnv)->CallObjectMethod(pEnv, jobjPlayer, g_jMethodIdCreateAudioTrack,
                                                   AUDIO_DST_SAMPLE_RATE, AUDIO_DST_CHANNEL_COUNTS);
    LogE(TAG, DEBUG, "create: createAudioTrack");

    //这里保存全局变量，可以就可以多线程共享了
    g_mObject = (*pEnv)->NewGlobalRef(pEnv, audio_track);
    jclass audio_track_class = (*pEnv)->GetObjectClass(pEnv, audio_track);
    // jclass audio_track_class = (*pEnv)->GetObjectClass(pEnv, g_mObject);

    
    g_jMethodIdAudioTrackPlay = (*pEnv)->GetMethodID(pEnv, audio_track_class, "play", "()V");
    if (!g_jMethodIdAudioTrackPlay)
    {
        LogE(TAG, DEBUG, "get method: play failed.");
    }
 
    (*pEnv)->CallVoidMethod(pEnv, audio_track, g_jMethodIdAudioTrackPlay);
    LogE(TAG, DEBUG, "create: play");

    // 1、 g_jMethodIdAudioTrackWrite可以audiotrack创建的时候保存到全局变量，也可以通过全局mObject来获取
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

    // 2、 g_jMethodIdAudioTrackWrite可以通过全局mObject来获取，也可以在audiotrack创建的时候保存到全局变量
    if(g_jMethodIdAudioTrackWrite == NULL)
    {
        jclass audio_track_class = (*env)->GetObjectClass(env, g_mObject);
        g_jMethodIdAudioTrackWrite = (*env)->GetMethodID(env, audio_track_class, "write","([BII)I");
    }
    

    //将pcm的数据写入AudioTrack
    (*env)->CallIntMethod(env, g_mObject, g_jMethodIdAudioTrackWrite, data_array, 0,
                                      out_buffer_size);

    LogE(TAG, DEBUG, "pcm_write write end");
    //释放局部引用
    (*env)->DeleteLocalRef(env, data_array);

}

int jni_onload_audioTrack_player(JavaVM *vm, JNIEnv* env)
{
    //JNIEnv *env = NULL;
    if (!vm) {
        LogE(TAG, DEBUG, "No Java virtual machine has been registered\n");
        return -1;
    }


    jclass cls = (*env)->FindClass(env, JNI_CLASS_AUDIOTRACK_PLAYER);
    if (cls == NULL)
    {
        LogE(TAG, DEBUG, "FindClass failed.");
        return 1;
    }

    if ((*env)->RegisterNatives(env, cls, g_methods, sizeof(g_methods) / sizeof(g_methods[0])) < 0)
    {
        LogE(TAG, DEBUG, "RegisterNatives failed.");
        return 1;
    }

    return 0;
}

