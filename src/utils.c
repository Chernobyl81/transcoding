#include "utils.h"

int get_video_and_audio_indeies(AVFormatContext **fmt_ctx,
                                AVCodecContext **dec_ctx,
                                const char* filename,
                                int *video_stream_index,
                                int *audio_stream_index)
{
    AVCodec *dec;
    int ret;

    if ((ret = avformat_open_input(fmt_ctx, filename, NULL, NULL)) < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "Cannot open input file\n");
        return ret;
    }
    

    if ((ret = avformat_find_stream_info(*fmt_ctx, NULL)) < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "Cannot find stream information\n");
        return ret;
    }

    av_dump_format(*fmt_ctx, 0, filename, 0);

    ret = av_find_best_stream(*fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, &dec, 0);
    if (ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "Cannot find a audio stream in the input file\n");
        return ret;
    }

    *audio_stream_index = ret;
   
    /* select the video stream */
    ret = av_find_best_stream(*fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &dec, 0);
    if (ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "Cannot find a video stream in the input file\n");
        return ret;
    }
    *video_stream_index = ret;
    av_log(NULL, AV_LOG_INFO, "video stream index is %d\n", *video_stream_index);

    /* create decoding context */
    *dec_ctx = avcodec_alloc_context3(dec);
    if (!dec_ctx)
        return AVERROR(ENOMEM);
    avcodec_parameters_to_context(*dec_ctx, (*fmt_ctx)->streams[*video_stream_index]->codecpar);

    /* init the video decoder */
    if ((ret = avcodec_open2(*dec_ctx, dec, NULL)) < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "Cannot open video decoder\n");
        return ret;
    }

    return 0;
}

// void log_packet(const AVFormatContext *fmt_ctx, const AVPacket *pkt, const char *tag)
// {
//     AVRational *time_base = &fmt_ctx->streams[pkt->stream_index]->time_base;
 
//   printf("%s: pts:%s pts_time:%s dts:%s dts_time:%s duration:%s duration_time:%s stream_index:%d\n",
//                tag,
//                av_ts2str(pkt->pts), av_ts2timestr(pkt->pts, time_base),
//                av_ts2str(pkt->dts), av_ts2timestr(pkt->dts, time_base),
//                av_ts2str(pkt->duration), av_ts2timestr(pkt->duration, time_base),
//                pkt->stream_index);
// }

int copy_stream(const char* inputfile, const char* output, int stream_index)
{
    int ret = 0;
    AVOutputFormat *ofmt = NULL;
    AVFormatContext *ifmt_ctx = NULL, *ofmt_ctx = NULL;
    AVPacket pkt;
    
    ret = avformat_open_input(&ifmt_ctx, inputfile, NULL, NULL);
    if (ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "Could not open input file!\n");
        goto end;
    }

    ret = avformat_find_stream_info(ifmt_ctx, 0);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Failed to retrive input stream information!\n");
        goto end;
    }

    // av_dump_format(ifmt_ctx, 0, inputfile, 0);

    avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, output);
    if (!ofmt_ctx) {
        av_log(NULL, AV_LOG_ERROR, "Could not create output context!\n");
        ret = AVERROR_UNKNOWN;
        goto end;
    }
    ofmt = ifmt_ctx->oformat;

    AVStream const *input_stream = ifmt_ctx->streams[stream_index];
    AVStream const *output_stream = avformat_new_stream(ofmt_ctx, input_stream->codec->codec);
    if (!output_stream) {
        av_log(NULL, AV_LOG_ERROR, "Failed allocating output stream!\n");
        ret = AVERROR_UNKNOWN;
        goto end;
    }

    ret = avcodec_copy_context(output_stream->codec, input_stream->codec);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Failed to copy context from input to output stream codec context!\n");
        goto end;
    }
    output_stream->codec->codec_tag = 0;
    if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
        output_stream->codec->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    av_dump_format(ofmt_ctx, 0, output, 1);

    if (!ofmt->flags & AVFMT_NOFILE) {
        ret = avio_open(&ofmt_ctx->pb, output, AVIO_FLAG_WRITE);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Could not open output file '%s'\n", output);
            goto end;
        }
    }

    ret = avformat_write_header(ofmt_ctx, NULL);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Error occurred when opening output file\n");
        goto end;
    }

    while (1)
    {
        AVStream *in_stream, *out_stream;
        ret = av_read_frame(ifmt_ctx, &pkt);
        if (ret < 0) 
            break;

        in_stream = ifmt_ctx->streams[stream_index];
        out_stream = ofmt_ctx->streams[stream_index];

        log_packet(ifmt_ctx, &pkt, "in");

        av_packet_unref(&pkt);
    }
    
    av_write_trailer(ofmt_ctx);


end:
    avformat_close_input(&ifmt_ctx);
    if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
        avio_closep(&ofmt_ctx->pb);
    
    avformat_free_context(ofmt_ctx);
   
    if (ret < 0 && ret != AVERROR_EOF) {
        fprintf(stderr, "Error occurred: %s\n", av_err2str(ret));
        return 1;
    }

    return 0;
}