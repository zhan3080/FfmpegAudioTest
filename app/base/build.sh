#!/usr/bin/env bash
echo "build start *************************"

ndk-build NDK_APPLICATION_MK=Application.mk NDK_PROJECT_PATH=. clean

ndk-build NDK_APPLICATION_MK=Application.mk NDK_PROJECT_PATH=.

cp -rf libs/armeabi-v7a ../libs/

echo "build end  **************************"
