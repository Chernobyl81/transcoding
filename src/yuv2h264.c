#include "yuv2h264.h"

int flush_encoder(AVFormatContext *fmt_ctx, unsigned int stream_index){
    int ret;
    int got_frame;
    AVPacket encode_pkt;

    //fmt_ctx->streams[stream_index]->codecpar->codec_type
    if (!(fmt_ctx->streams[stream_index]->codec->codec->capabilities & AV_CODEC_CAP_DELAY))
        return 0;

    while (1) {
        encode_pkt.data = NULL;
        encode_pkt.size = 0;
        av_init_packet(&encode_pkt);

        ret = avcodec_encode_video2(fmt_ctx->streams[stream_index]->codec, 
                                    &encode_pkt,
                                    NULL, 
                                    &got_frame);
        av_frame_free(NULL);
        if (ret < 0)
            break;
        if (!got_frame){
            ret = 0;
            break;
        }
        printf("Flush Encoder: Succeed to encode 1 frame!\tsize:%5d\n", encode_pkt.size);
        /* mux encoded frame */
        ret = av_write_frame(fmt_ctx, &encode_pkt);
        if (ret < 0)
            break;
    }
    return ret;
}

void static init_av_codec_context(AVCodecContext** pCodecCtx,
                                  const AVStream *video_st,
                                  int width, 
                                  int height, 
                                  const AVOutputFormat* pOutputFormat)
{
   
    // if (avcodec_parameters_to_context(&pCodecCtx, video_st->codecpar) < 0) {
    //     av_log(NULL, AV_LOG_ERROR, "can not init av_codec_context!\n");
    //     EXIT_FAILURE;
    // }

    (*pCodecCtx) = video_st->codec;
    (*pCodecCtx)->codec_id = pOutputFormat->video_codec;
    (*pCodecCtx)->codec_type = AVMEDIA_TYPE_VIDEO;
    (*pCodecCtx)->pix_fmt = AV_PIX_FMT_YUV420P;
    (*pCodecCtx)->width = width;  
    (*pCodecCtx)->height = height;
    (*pCodecCtx)->time_base.num = 1;  
    (*pCodecCtx)->time_base.den = 25;  
    (*pCodecCtx)->bit_rate = 400000 ;  // set by user
    (*pCodecCtx)->gop_size = 10;
    //H264
    (*pCodecCtx)->qmin = 10;
    (*pCodecCtx)->qmax = 51;

    //Optional Param
    (*pCodecCtx)->max_b_frames = 0;
    av_log(NULL, AV_LOG_INFO, "init av codec context finished!\n");
}


int static init_frame(AVFrame **pFrame, 
                      const AVCodecContext *pCodecCtx, 
                      uint8_t **picture_buf,
                      int *picture_size,
                      int width, 
                      int height)
{ 
    *pFrame = av_frame_alloc();
    if (!*pFrame) {
        av_log(NULL, AV_LOG_ERROR, "alloc frame failed!\n");
        return -1;
    }

    (*pFrame)->width = width;
    (*pFrame)->height = height;
    (*pFrame)->format = pCodecCtx->pix_fmt;

    *picture_size = av_image_get_buffer_size(pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height, 1);
    *picture_buf = (uint8_t *)av_malloc(*picture_size);
    if (!picture_buf) {
        av_log(NULL, AV_LOG_ERROR, "can not alloc picture buffer!\n");
        return -1;
    }

    // Use av_image_fill_arrays instead
    AVPicture* picture = (AVPicture *)(*pFrame);
    return av_image_fill_arrays(picture->data, picture->linesize,
                                 picture_buf,  pCodecCtx->pix_fmt, width, height, 1);
}

int YUV2H264(const char* input_filename, 
            const char *output_filename, 
            int width,
            int height,
            int frame_num)
{
    int ret;
    int framecnt = 0;
    int y_size;
    int picture_size;

    AVFormatContext         *fmt_ctx;
    AVOutputFormat const    *pOutputFormat;
    AVStream                *video_st;
    AVFrame                 *pFrame;
    AVCodecContext          *pCodecCtx = NULL;
    AVCodec        const    *pCodec;
    AVPacket                pkt;
    AVDictionary            *param = NULL;
    uint8_t                 *picture_buf;
    
    FILE *in_file = fopen(input_filename, "rb");
    if (!in_file) {
        av_log(NULL, AV_LOG_ERROR, "can not open tmp file %s\n", input_filename);
        EXIT_FAILURE;
    }

    ret = avformat_alloc_output_context2(&fmt_ctx, NULL, NULL, output_filename);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "can not allocate output context! \n");
        return ret;
    }
    pOutputFormat = fmt_ctx->oformat;

    ret = avio_open(&fmt_ctx->pb, output_filename, AVIO_FLAG_READ_WRITE);
    if (ret < 0){
        av_log(NULL, AV_LOG_ERROR, "Failed to open output file! \n");
        return ret;
    }

    video_st = avformat_new_stream(fmt_ctx, 0);

    if (!video_st){
        av_log(NULL, AV_LOG_ERROR, "can not create new video stream! \n");
        return -1;
    } else {
        video_st->time_base.num = 1; 
        video_st->time_base.den = 25;  
    }

    //Param that must set
    init_av_codec_context(&pCodecCtx, video_st, width, height, pOutputFormat);
      
    // Set Options
    if(pCodecCtx->codec_id == AV_CODEC_ID_H264) {
        if (av_dict_set(&param, "preset", "slow", 0) < 0 ||
            av_dict_set(&param, "tune", "zerolatency", 0) < 0 ||
            av_dict_set(&param, "profile", "main", 0) < 0)
            {
                av_log(NULL, AV_LOG_ERROR, "Set H264 option failed! \n ");
                return -1;
            }
    }

    if(pCodecCtx->codec_id == AV_CODEC_ID_H265){
        if (av_dict_set(&param, "preset", "ultrafast", 0) < 0 ||
            av_dict_set(&param, "tune", "zero-latency", 0) < 0)
            {
                av_log(NULL, AV_LOG_ERROR, "Set h265 option failed! \n");
                return -1;
            }
    }

    av_log(NULL, AV_LOG_INFO, "Set option finished! \n");
    //Show output file information
    av_dump_format(fmt_ctx, 0, output_filename, 1);

    pCodec = avcodec_find_encoder(pCodecCtx->codec_id);
    if (!pCodec){
        av_log(NULL, AV_LOG_ERROR, "Can not find encoder! \n");
        return -1;
    }
    if (avcodec_open2(pCodecCtx, pCodec, &param) < 0){
        av_log(NULL, AV_LOG_ERROR, "Failed to open encoder! \n");
        return -1;
    }

    ret = init_frame(&pFrame, pCodecCtx, &picture_buf, &picture_size, width, height);
    if (ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "can not fill frame! \n");
    }

    ret = avformat_write_header(fmt_ctx, NULL);
    if (ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "write header faild with code: %d \n", ret);
    }

    av_new_packet(&pkt, picture_size);
    y_size = pCodecCtx->width * pCodecCtx->height;

    for (int i = 0; i < frame_num; i++){
        //Read raw YUV data
        if (fread(picture_buf, 1, y_size*3/2, in_file) <= 0){
            printf("Failed to read raw data! \n");
            return -1;
        }else if(feof(in_file)){
            break;
        }
        pFrame->data[0] = picture_buf;              // Y
        pFrame->data[1] = picture_buf+ y_size;      // U 
        pFrame->data[2] = picture_buf+ y_size*5/4;  // V
        //PTS
        pFrame->pts=i;
        int got_picture=0;
        //Encode
        int ret = avcodec_encode_video2(pCodecCtx, &pkt, pFrame, &got_picture);
        if(ret < 0){
            printf("Failed to encode! \n");
            return -1;
        }
        if (got_picture == 1){
            // printf("Succeed to encode frame: %5d\tsize:%5d\n",framecnt, pkt.size);
            framecnt++;
            pkt.stream_index = video_st->index;
            ret = av_write_frame(fmt_ctx, &pkt);
            av_packet_unref(&pkt);
        }
    }
    //Flush Encoder
    ret = flush_encoder(fmt_ctx, 0);
    if (ret < 0) {
        printf("Flushing encoder failed\n");
        return -1;
    }

    //Write file trailer
    av_write_trailer(fmt_ctx);
    av_dump_format(fmt_ctx, 0, output_filename, 1);
    
    //Clean
    if (video_st){
        avcodec_close(video_st->codec);
        av_free(pFrame);
        av_free(picture_buf);
    }

    avio_close(fmt_ctx->pb);
    avformat_free_context(fmt_ctx);
    fclose(in_file);
    return 0;
}