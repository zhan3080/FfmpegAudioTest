#include <jni.h>

/*
 * set java vm.
 *
 */
void ff_jni_set_java_vm(JavaVM *vm);

/*
 * Attach permanently a JNI environment to the current thread and retrieve it.
 *
 * If successfully attached, the JNI environment will automatically be detached
 * at thread destruction.
 *
 * @return the JNI environment on success, NULL otherwise
 */
JNIEnv *ff_jni_get_env();

#define JNI_GET_ENV_OR_RETURN(env, ret) do {                       \
    (env) = ff_jni_get_env();                                      \
    if (!(env)) {                                                  \
        return ret;                                                \
    }                                                              \
} while (0)

#define JNI_GET_ENV_OR_RETURN_VOID(env) do {                       \
    (env) = ff_jni_get_env();                                      \
    if (!(env)) {                                                  \
        return;                                                    \
    }                                                              \
} while (0)
