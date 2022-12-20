#ifndef UTILS_H
#define UTILS_H
#define _XOPEN_SOURCE 600 /* for usleep */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libavutil/timestamp.h>
#endif

/**
 * @brief Get video stream index, audio stream index,
 * and alloc decode context.
 * 
 */

int get_video_and_audio_indeies(AVFormatContext **fmt_ctx, 
                                AVCodecContext **dec_ctx,
                                const char* filename,
                                int *vidoe_stream_index,
                                int *audio_stream_index);




int copy_stream(const char* inputfile, const char* output, int stream_index);