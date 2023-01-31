#include <stdio.h>

#include <libavformat/avformat.h>
#include <libavutil/dict.h>
#include "libavcodec/avcodec.h"
#include<libavdevice/avdevice.h>

int main(int argc, char **argv) {
    AVFormatContext *fmt_ctx = NULL;
    AVDictionaryEntry *tag = NULL;

    printf("avdevice_license      : %s\n", avdevice_license());
    printf("avcodec_configuration : %s\n", avcodec_configuration());
    return 0;
}
