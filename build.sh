#!/bin/bash

set -o errexit
set -o pipefail
set -o nounset
set +x

platform=arm64-v8a
#platform=armeabi-v7a
# android_project=onnxruntime-inference-examples/mobile/examples/image_classification/android
# project_cpp_path=/mnt/c/Users/David/StudioProjects/"$android_project"/app/src/main/cpp

if [ -d "build" ];then
    echo "****** Remove build directory ******"
    rm -rf build
fi

cmake \
-DCMAKE_TOOLCHAIN_FILE=$NDK/build/cmake/android.toolchain.cmake \
-DCMAKE_BUILD_TYPE=MinSizeRel \
-DANDROID_ABI=$platform \
-DANDROID_PLATFORM=21 \
-DANDROID_NATIVE_API_LEVEL=21 \
-DCMAKE_SYSTEM_VERSION=21 \
-DANDROID_NDK_ABI_NAME=$platform -B build

cmake --build build -j8
echo "Build task done"