#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <libavutil/avassert.h>
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
#include <libavutil/mathematics.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>

#define STREAM_DURATION   10.0
#define STREAM_FRAME_RATE 25 /* 25 images/s */
#define STREAM_PIX_FMT    AV_PIX_FMT_YUV420P /* default pix_fmt */

#define SCALE_FLAGS SWS_BICUBIC

typedef struct OutputStream {
    AVStream *st;
    AVCodecContext *enc;

    /* pts of the next frame that will be generated */
    int64_t next_pts;
    int samples_count;

    AVFrame *frame;
    AVFrame *tmp_frame;

    float t, tincr, tincr2;

    struct SwsContext *sws_ctx;
    struct SwrContext *swr_ctx;
} OutputStream;

int write_frame(AVFormatContext *fmt_ctx, AVCodecContext *c,
                       AVStream const *st, AVFrame const *frame);

void add_stream(OutputStream *ost, AVFormatContext *oc,
                       AVCodec **codec,
                       enum AVCodecID codec_id);

AVFrame *alloc_audio_frame(enum AVSampleFormat sample_fmt,
                                  uint64_t channel_layout,
                                  int sample_rate, int nb_samples);
void open_audio(AVCodec const *codec, OutputStream *ost, AVDictionary const *opt_arg);
AVFrame *get_audio_frame(OutputStream *ost);
int write_audio_frame(AVFormatContext *oc, OutputStream *ost);
AVFrame *alloc_picture(enum AVPixelFormat pix_fmt, int width, int height);
void open_video(AVCodec const *codec, OutputStream *ost, AVDictionary const *opt_arg);
void fill_yuv_image(AVFrame *pict, int frame_index, int width, int height);
AVFrame *get_video_frame(OutputStream *ost);
int write_video_frame(AVFormatContext *oc, OutputStream *ost);
void close_stream(AVFormatContext const *oc, OutputStream *ost);