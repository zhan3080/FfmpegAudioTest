APP_STL:=stlport_static
# APP_STL := gnustl_static
APP_PLATFORM := android-21
# APP_ABI := arm64-v8a
APP_ABI := armeabi-v7a
#APP_ABI := armeabi armeabi-v7a x86 mips x86_64 arm64-v8a
#APP_ABI := armeabi armeabi-v7a x86  x86_64 arm64-v8a
# APP_ABI := armeabi armeabi-v7a arm64-v8a
APP_CFLAGS := -DDISABLE_NEON
APP_CFLAGS += -Wno-error=format-security
APP_BUILD_SCRIPT := Android.mk
