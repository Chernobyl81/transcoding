#include "transcoding.h"

int main(int argc, char **argv)
{
#ifdef USE_DEBUG
    av_log_set_level(AV_LOG_DEBUG);
#endif
    if (argc != 4) {
        av_log(NULL, AV_LOG_ERROR, "Usage: %s <input file> <output file> <logo path>\n", argv[0]);
        return 1;
    }

    filter_video(argv[1], argv[2], argv[3], 30, 10);
}