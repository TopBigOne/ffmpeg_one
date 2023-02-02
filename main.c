#include <stdio.h>

#include <libavformat/avformat.h>
#include <libavutil/dict.h>
#include <libavutil/opt.h>
#include "libavcodec/avcodec.h"
#include<libavdevice/avdevice.h>
#include<libswscale/swscale.h>
#include <stdbool.h>


#define  video_path  "/Users/dev/Desktop/mp4/貳佰《玫瑰》1080P.mp4"
// 苹果提供的测试源
#define rtmp_web_video_url "http://devimages.apple.com.edgekey.net/streaming/examples/bipbop_4x3/gear2/prog_index.m3u8"
int ret = -1;

void test_ffmpeg_env();

int open_web_video();

int open_local_video();


// Q: pFormat 不用显示声明吗？
AVFormatContext *pFormat = NULL;

void InitLibAvFormat();


int main(int argc, char **argv) {
    test_ffmpeg_env();


    // step 1: 初始化
    InitLibAvFormat();


    FILE *f1 = fopen("/Users/dev/Documents/Android_work/main_ffmpeg/1_ffmpeg_env/yuv_file/yuv420_y.y", "web+");
    FILE *f2 = fopen("/Users/dev/Documents/Android_work/main_ffmpeg/1_ffmpeg_env/yuv_file/yuv420_u.y", "web+");
    FILE *f3 = fopen("/Users/dev/Documents/Android_work/main_ffmpeg/1_ffmpeg_env/yuv_file/yuv420_v.y", "web+");

    // step 2: 打开源文件
    open_local_video();
    // step 3:寻流器信息： H264, width , height;
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

    // 寻找流信息
    int videoStream = -1;
    int audioStream = -1;
    videoStream = av_find_best_stream(pFormat, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, NULL);
    audioStream = av_find_best_stream(pFormat, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, NULL);

    // step 4:寻找解码器
    AVCodec *vCodec = avcodec_find_decoder(pFormat->streams[videoStream]->codec->codec_id);
    if (!vCodec) {
        fprintf(stderr, "step 4 avcodec_find_decoder in failure.\n");
        return -1;
    }
    fprintf(stdout, "step 4 avcodec_find_decoder in success.\n");
    // step 5: 打开解码器
    ret = avcodec_open2(pFormat->streams[videoStream]->codec,
                        vCodec, NULL);
    if (ret) {
        fprintf(stderr, "step 5 avcodec_open2 in failure.\n");
        return -1;
    }
    fprintf(stdout, "step 5 avcodec_open2 in success.\n");
    // step 6 : 解码视频
    // 申请原始空间==> 创建帧空间
    AVFrame *frame = av_frame_alloc();
    AVFrame *frameYUV = av_frame_alloc();
    fprintf(stdout, "step 6.\n");
    int width = pFormat->streams[videoStream]->codec->width;
    int height = pFormat->streams[videoStream]->codec->height;
    enum AVPixelFormat fmt = pFormat->streams[videoStream]->codec->pix_fmt;
    // step 7: 分配空间,进行图像转换
    int nSize = avpicture_get_size(AV_PIX_FMT_YUV420P, width, height);
    fprintf(stdout, "step 7.\n");
    uint8_t *buff = NULL;
    buff = (uint8_t *) av_malloc(nSize);

    // 一帧图像
    avpicture_fill((AVPicture *) frameYUV, buff, AV_PIX_FMT_YUV420P, width, height);

    AVPacket *packet = (AVPacket *) av_malloc(sizeof(AVPacket));
    // 转换上下文
    struct SwsContext *swsCtx = NULL;
    // sws_getCachedContext(); 用于单线程；
    // sws_getContext(); 用于多线程；
    swsCtx = sws_getContext(width, height, fmt, width, height,
                            AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
    fprintf(stdout, "step 9.\n");
    // 读取帧率
    int go = -1;
    int FrameCount = 0;
    while (av_read_frame(pFormat, packet) >= 0) {
        // 判断 stream_index , 判断流类型
        if (packet->stream_index == AVMEDIA_TYPE_VIDEO) {
            ret = avcodec_decode_video2(pFormat->streams[videoStream]->codec,
                                        frame, &go, packet);
            if (ret < 0) {
                printf(stderr, "avcodec_decode_video2  in failure.");
                return -1;
            }

            if (go) {
                sws_scale(swsCtx, (const uint8_t **) frame->data,
                          frame->linesize,
                          0,
                          height,
                          frameYUV->data,
                          frameYUV->linesize);


                fwrite(frameYUV->data[0], 1, width * height, f1);
                fwrite(frameYUV->data[1], 1, width * height / 4, f2);
                fwrite(frameYUV->data[2], 1, width * height / 4, f3);

                FrameCount++;
                printf("frame index: %d \n", FrameCount++);

            }
        }
    }

    av_free(packet);
    fclose(f1);
    fclose(f2);
    fclose(f3);

    sws_freeContext(swsCtx);
    av_frame_free(&frame);
    av_frame_free(&frameYUV);
    avformat_close_input(&pFormat);


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

