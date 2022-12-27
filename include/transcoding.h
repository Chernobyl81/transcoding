#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/opt.h>
#include <libavutil/pixdesc.h>
#include <stdio.h>
#include <unistd.h>
#include "config.h"

/**
 * @brief 
 * 
 * @param input_file path
 * @param output_file path 
 * @param logo path
 * @param x 
 * @param y 
 * @return int 
 */
int filter_video(const char* input_file, const char* output_file, const char* logo, int x, int y);