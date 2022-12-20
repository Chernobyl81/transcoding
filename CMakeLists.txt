cmake_minimum_required(VERSION 3.16.3)
project(water_mark VERSION 0.1.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 17)
include(CTest)
enable_testing()


add_executable(water_mark src/transcoding.c)
# add_executable(${PROJECT_NAME} src/main.c src/filtering_video.c src/yuv2h264.c src/utils.c)
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PRIVATE avfilter avcodec avformat avutil swscale swresample m)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
