cmake_minimum_required(VERSION 3.16.3)
project(water_mark VERSION 0.1.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 17)
set(VERSION_STRING 0.1.0)
set(SOVERSION_STRING 0.1.0)
message(STATUS "SHARED LIBRARY Version: ${SOVERSION_STRING}")
include(CTest)
enable_testing()


if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "BUILD TYPE is ${CMAKE_BUILD_TYPE}")
    option(USE_DEBUG "DEBUG FFMpeg" ON)
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

configure_file(config.h.in include/config.h @ONLY)

file(GLOB HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
file(GLOB SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)



if(ANDROID_ABI)
    message(STATUS "Building for android ${ANDROID_ABI}")
    include(cmake/cc-android.cmake)
endif()

add_library(${PROJECT_NAME} SHARED ${SRC})
target_include_directories(${PROJECT_NAME} PUBLIC include ${CMAKE_CURRENT_BINARY_DIR}/include ffmpeg)
target_link_libraries(${PROJECT_NAME} PRIVATE avfilter avcodec avformat avutil)
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${VERSION_STRING} SOVERSION ${SOVERSION_STRING})

# add_subdirectory(tests)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
