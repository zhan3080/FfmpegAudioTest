
#include "logger.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavfilter/avfilter.h"
#include "libswscale/swscale.h"

#define TAG "audioTrackPlayer"
#define DEBUG true

#define JNI_CLASS_AUDIOTRACK_PLAYER     "com/example/ffmepgtest/pcm/application/audioTrackPlayer"

#define OUTPUT_FILE "/sdcard/output.pcm"

FILE *pcmFile;
void *buffer;
uint8_t *out_buffer;


static JNINativeMethod g_methods[] =
{
    {"_create",             "([Ljava/lang/Object;)J",       (void *)create},
};

int jni_onload_audioTrack_player(JavaVM *vm)
{
    JNIEnv *env = NULL;

    if (!vm) {
        leLogE(TAG, DEBUG, "No Java virtual machine has been registered\n");
        return;
    }

    int ret = (*vm)->GetEnv(vm, (void **)&env, JNI_VERSION_1_4);
    switch(ret) {
    case JNI_EDETACHED:
        break;
    case JNI_OK:
        break;
    case JNI_EVERSION:
        leLogE(TAG, DEBUG, "The specified JNI version is not supported\n");
        break;
    default:
        leLogE(TAG, DEBUG, "Failed to get the JNI environment attached to this thread\n");
        break;
    }


    jclass cls = (*env)->FindClass(env, JNI_CLASS_AUDIOTRACK_PLAYER);
    if (cls == NULL)
    {
        leLogE(TAG, DEBUG, "FindClass failed.");
        return 1;
    }

    if ((*env)->RegisterNatives(env, cls, g_methods, sizeof(g_methods) / sizeof(g_methods[0])) < 0)
    {
        leLogE(TAG, DEBUG, "RegisterNatives failed.");
        return 1;
    }



    return 0;
}

