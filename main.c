#include <stdio.h>

#include <libavformat/avformat.h>
#include <libavutil/dict.h>
#include <libavutil/opt.h>
#include "libavcodec/avcodec.h"
#include<libavdevice/avdevice.h>

enum openVideoType {
    LOCAL,
    WEBSITE
};

#define  video_path  "/Users/dev/Desktop/mp4/零一九零贰 - 忘川彼岸 (DJ名龙版)【動態歌詞_pīn yīn gē cí】抖音一夜之间火了.mp4"
// 苹果提供的测试源
#define rtmp_web_video_url "http://devimages.apple.com.edgekey.net/streaming/examples/bipbop_4x3/gear2/prog_index.m3u8"
int ret = -1;

void test_ffmpeg_env();

int open_web_video();

int open_local_video();

// 打开视频源类型
enum openVideoType openType = LOCAL;

// Q: pFormat 不用显示声明吗？
AVFormatContext *pFormat = NULL;

void InitLibAvFormat();


int main(int argc, char **argv) {
    test_ffmpeg_env();
    // step 1: 初始化
    InitLibAvFormat();

    // step 2: 打开源文件
    if (openType == LOCAL) {
        open_local_video();
    } else if (openType == WEBSITE) {
        open_web_video();
    }

    // step 3:寻找解码器信息
    ret = avformat_find_stream_info(pFormat, NULL);
    if (ret) {
        fprintf(stderr, "step 3 : avformat_find_stream_info failed. \n");
        return -1;
    }
    fprintf(stdout, "step 3 : avformat_find_stream_info success. \n");
    int time = pFormat->duration;
    int video_minutes_time = time / 1000000 / 60;
    int video_seconds_time = time / 1000000 % 60;
    fprintf(stdout, "   video_minutes_time   : %d minutes \n", video_minutes_time);
    fprintf(stdout, "   video_seconds_time   : %d seconds\n", video_seconds_time);

    // step 4:寻找解码器
    return 0;
}

void InitLibAvFormat() {
    // step 1:
    av_register_all();
    fprintf(stdout, "step 1 : av_register_all success. \n");
}


void test_ffmpeg_env() {
    printf("ffmpeg_env--> avcodec_configuration\n %s: \n", avcodec_configuration());
    printf("----------------------------------------------------------------↓\n");
}

int open_web_video() {
    avformat_network_init();
    AVDictionary *opt = NULL;
    av_dict_set(&opt, "rtsp_transport", "tcp", 0);
    // 时间延迟
    av_dict_set(&opt, "max_delay", "5500", 0);
    int open_web_video_result = avformat_open_input(&pFormat, rtmp_web_video_url, NULL, &opt);
    if (open_web_video_result) {
        fprintf(stderr, "open web video input stream in failure\n");
        return -1;
    }
    fprintf(stdout, "open web video input stream in success\n");

}

int open_local_video() {
    // step 2:
    ret = avformat_open_input(&pFormat, video_path, NULL, NULL);
    if (ret) {
        fprintf(stderr, "step 2 : avformat_open_input failed. \n");
        return -1;
    }
    fprintf(stdout, "step 2 : avformat_open_input success. \n");
    // 打印本地视频流信息
    av_dump_format(pFormat, 0, video_path, 0);
}

