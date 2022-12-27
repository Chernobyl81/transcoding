add_executable(watermark_test main.c)
target_include_directories(watermark_test PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/include)
target_link_libraries(watermark_test PRIVATE water_mark avfilter avcodec avformat avutil)