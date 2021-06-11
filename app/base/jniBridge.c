// #include <jni.h>
#include "logger.h"
#include <jni.h>
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavfilter/avfilter.h"
#include "libswscale/swscale.h"

#include <pthread.h>

#define TAG "ffmpeg_jni"
#define DEBUG true

// com.xxx.ffmpeglibrary.FFmpegBridge
#define JNI_CLASS_FFMPEG_BRIDGE     "com/example/ffmepgtest/pcm/application/MainActivity"

void base_hello(JNIEnv * env,jobject obj);
void pcmDecode(JNIEnv* env, jobject thiz);

extern void* play_audio(void *argv);
extern void* decode_audio(void *argv);
extern void* jni_onload_audioTrack_player(JavaVM* vm, JNIEnv* env);

static JNINativeMethod gMethods[] =
{
    {"_pcmDecode",                 "()V",         (void*)pcmDecode },
};

static int registerFFplayer(JNIEnv *env, jclass cls){
    LogI(TAG, DEBUG, "registerNatives");
    //调用RegisterNatives来注册本地方法，完成映射
    if((*env)->RegisterNatives(env, cls, gMethods, sizeof(gMethods) / sizeof(gMethods[0])) < 0){
        LogE(TAG, DEBUG, "RegisterNatives failed");
        return 1;
    }
    return 0;
}

/* This function will be exec when so been loading.
   这个方法在so加载的时候执行
*/
jint JNI_OnLoad(JavaVM* vm, void *reserved)
{
    LogI(TAG, DEBUG, "JNI_OnLoad");
    JNIEnv* env = NULL;
    ff_jni_set_java_vm(vm);
    if((*vm)->GetEnv(vm,(void **)&env,JNI_VERSION_1_4) != JNI_OK){
        LogE(TAG, DEBUG, "JNI_OnLoad GetEnv error");
        return -1;
    }

    jclass cls = (*env)->FindClass(env,JNI_CLASS_FFMPEG_BRIDGE);//通过类路径字符串找到对应类
    registerFFplayer(env,cls);
    jni_onload_audioTrack_player(vm,env);
    LogE(TAG, DEBUG, "JNI_OnLoad success");
    return JNI_VERSION_1_4;
}

void JNI_OnUnload(JavaVM* vm, void *reserved)
{
    LogI(TAG, DEBUG, "JNI_OnUnload");
    return;
}

void base_hello(JNIEnv * env,jobject obj)
{
    LogI(TAG,DEBUG,"base_hello!!!");
    return;
}

void pcmDecode(JNIEnv* env, jobject obj)
{
    LogE(TAG,DEBUG, "pcmDecode play_audio");
    pthread_t thread;
    pthread_create(&thread, NULL, decode_audio, NULL);
    // pthread_create(&thread, NULL, play_audio, NULL);
    // return ;
}

