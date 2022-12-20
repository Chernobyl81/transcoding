#ifndef YUV_2_H264_H
#define YUV_2_H264_H
#include "utils.h"
#define __STDC_CONSTANT_MACROS
#endif

int flush_encoder(AVFormatContext *fmt_ctx, unsigned int stream_index);

/**
 * @brief Create a H264 file use YVU420P.
 * 
 * @param in_file Input YUV(YUV420P) file
 * @param output_filename Output H264 file name.
 * @param width 
 * @param height 
 * @param frame_num 
 * @return int 
 */
int YUV2H264(const char *in_file, 
            const char* output_filename,
            int width,
            int height, 
            int frame_num);