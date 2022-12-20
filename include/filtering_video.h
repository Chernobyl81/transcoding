#ifndef FILTERING_VIDEO_H
#define FILTERING_VIDEO_H
#include "muxing.h"
#include "utils.h"
#endif

/**
 * @brief Create YUV format file with watermark
 * 
 * @param input_filename Path of input mp4.
 * @param video_dst_filename YUV output file name.
 * @return int 
 */
size_t filter(const char* input_file, AVFormatContext *oc, OutputStream *video_st);