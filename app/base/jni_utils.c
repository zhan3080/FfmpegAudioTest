#include <jni.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#include "jni_utils.h"
#include "logger.h"

#define TAG "jni_utils"
#define LEDEBUG true

static JavaVM *java_vm;
static pthread_key_t current_env;
static pthread_once_t once = PTHREAD_ONCE_INIT;
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

static void jni_detach_env(void *data)
{
    if (java_vm) {
        (*java_vm)->DetachCurrentThread(java_vm);
    }
}

static void jni_create_pthread_key(void)
{
    pthread_key_create(&current_env, jni_detach_env);
}

void ff_jni_set_java_vm(JavaVM *vm)
{
    java_vm = vm;
}

JNIEnv *ff_jni_get_env()
{
    int ret = 0;
    JNIEnv *env = NULL;

    pthread_mutex_lock(&lock);

    if (!java_vm) {
        LogI(TAG, LEDEBUG, "No Java virtual machine has been registered\n");
        goto done;
    }

    pthread_once(&once, jni_create_pthread_key);

    if ((env = pthread_getspecific(current_env)) != NULL) {
        goto done;
    }

    ret = (*java_vm)->GetEnv(java_vm, (void **)&env, JNI_VERSION_1_4);
    switch(ret) {
    case JNI_EDETACHED:
        if ((*java_vm)->AttachCurrentThread(java_vm, &env, NULL) != 0) {
            LogI(TAG, LEDEBUG, "Failed to attach the JNI environment to the current thread\n");
            env = NULL;
        } else {
            pthread_setspecific(current_env, env);
        }
        break;
    case JNI_OK:
        break;
    case JNI_EVERSION:
        LogI(TAG, LEDEBUG, "The specified JNI version is not supported\n");
        break;
    default:
        LogI(TAG, LEDEBUG, "Failed to get the JNI environment attached to this thread\n");
        break;
    }

done:
    pthread_mutex_unlock(&lock);
    LogI(TAG, LEDEBUG, "get env:%x\n",env);
    return env;
}

char *ff_jni_jstring_to_utf_chars(JNIEnv *env, jstring string)
{
    char *ret = NULL;
    const char *utf_chars = NULL;

    jboolean copy = 0;

    if (!string) {
        return NULL;
    }

    utf_chars = (*env)->GetStringUTFChars(env, string, &copy);
    if ((*env)->ExceptionCheck(env)) {
        (*env)->ExceptionClear(env);
        LogI(TAG, LEDEBUG, "String.getStringUTFChars() threw an exception\n");
        return NULL;
    }

    ret = strdup(utf_chars);

    (*env)->ReleaseStringUTFChars(env, string, utf_chars);
    if ((*env)->ExceptionCheck(env)) {
        (*env)->ExceptionClear(env);
        LogI(TAG, LEDEBUG, "String.releaseStringUTFChars() threw an exception\n");
        if(ret)
            free(ret);
        return NULL;
    }

    return ret;
}

jstring ff_jni_utf_chars_to_jstring(JNIEnv *env, const char *utf_chars)
{
    jstring ret;

    ret = (*env)->NewStringUTF(env, utf_chars);
    if ((*env)->ExceptionCheck(env)) {
        (*env)->ExceptionClear(env);
        LogI(TAG, LEDEBUG, "NewStringUTF() threw an exception\n");
        return NULL;
    }

    return ret;
}