set(CMAKE_SYSTEM_NAME ANDROID)

if (NOT ${SYSTEM_PROCESSOR})
    message(STATUS "Set CMAKE_SYSTEM_PROCESSOR ${ANDROID_ABI}")
    set(SYSTEM_PROCESSOR aarch64)
endif()

unset(CMAKE_SYSTEM_PROCESSOR CACHE)
set(CMAKE_SYSTEM_PROCESSOR ${SYSTEM_PROCESSOR})

set(TARGET_LIBS_DIR  /home/david/Projects/ffmpeg/${ANDROID_ABI}/lib)
set(CMAKE_FIND_ROOT_PATH ${TARGET_LIBS_DIR} ${CMAKE_PREFIX_PATH})

add_library(avcodec SHARED IMPORTED )
set_target_properties(avcodec PROPERTIES IMPORTED_LOCATION ${TARGET_LIBS_DIR}/libavcodec.so)

add_library(avformat SHARED IMPORTED )
set_target_properties(avformat PROPERTIES IMPORTED_LOCATION ${TARGET_LIBS_DIR}/libavformat.so)

add_library(avfilter SHARED IMPORTED )
set_target_properties(avfilter PROPERTIES IMPORTED_LOCATION ${TARGET_LIBS_DIR}/libavfilter.so)

add_library(avutil SHARED IMPORTED )
set_target_properties(avutil PROPERTIES IMPORTED_LOCATION ${TARGET_LIBS_DIR}/libavutil.so)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)