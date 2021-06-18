LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := libavcodec
LOCAL_SRC_FILES := $(LOCAL_PATH)/libavcodec.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libavfilter
LOCAL_SRC_FILES := $(LOCAL_PATH)/libavfilter.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libavformat
LOCAL_SRC_FILES := $(LOCAL_PATH)/libavformat.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libavutil
LOCAL_SRC_FILES := $(LOCAL_PATH)/libavutil.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libswresample
LOCAL_SRC_FILES := $(LOCAL_PATH)/libswresample.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libswscale
LOCAL_SRC_FILES := $(LOCAL_PATH)/libswscale.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libavdevice
LOCAL_SRC_FILES := $(LOCAL_PATH)/libavdevice.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libpostproc
LOCAL_SRC_FILES := $(LOCAL_PATH)/libpostproc.a
include $(PREBUILT_STATIC_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE    := ffplayer
#sourcecode for ffplayer
LOCAL_SRC_FILES := \
    jniBridge.c \
	jni_utils.c \
	audioTrackPlayerNative.c\
	audioPlayer.c \
#	audioTrackPlayer.c \
#	openSLRender.c \

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH) \
	$(LOCAL_PATH)/logger.h \
	$(LOCAL_PATH)/openSLRender.h \
	$(LOCAL_PATH)/jni_utils.h

LOCAL_C_INCLUDES += $(LOCAL_PATH)/include
#LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/libavfilter
#LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/libavformat
#LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/libavutil
#LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/libswresample
#LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/libswscale
#LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/libavcodec



LOCAL_LDLIBS := -llog -lz -landroid -lOpenSLES

#LOCAL_STATIC_LIBRARIES := libavcodec libavfilter libavformat libavutil libswresample libswscale libavdevice libpostproc
#顺序很重要
LOCAL_STATIC_LIBRARIES := libavformat libavfilter libavcodec libavutil libswresample libavdevice  libpostproc libswscale

LOCAL_LDFLAGS += -Wl,--gc-sections


include $(BUILD_SHARED_LIBRARY)
