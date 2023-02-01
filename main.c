#include <stdio.h>

#include <libavformat/avformat.h>
#include <libavutil/dict.h>
#include "libavcodec/avcodec.h"
#include<libavdevice/avdevice.h>

#define YUV_FILE_PATH "/Users/dev/Documents/Android_work/main_ffmpeg/1_ffmpeg_env/yuv_file/akiyo_qcif.yuv"
#define Y_FILE_PATH "/Users/dev/Documents/Android_work/main_ffmpeg/1_ffmpeg_env/yuv_file/yuv420_y.y"
#define U_FILE_PATH "/Users/dev/Documents/Android_work/main_ffmpeg/1_ffmpeg_env/yuv_file/yuv420_u.y"
#define V_FILE_PATH "/Users/dev/Documents/Android_work/main_ffmpeg/1_ffmpeg_env/yuv_file/yuv420_v.y"

void yuv420(const char *path, int width, int height);

int main(int argc, char **argv) {
    yuv420(YUV_FILE_PATH, 176, 144);

    return 0;
}

/**
 * 像素的呈现方式 ： yuv = rgb;
 * 数据类型：
 * 1： 普通类型；
 * 2： 矩阵类型：mat
 * 3： 原组类型：vec3 ,vec4 : 三元组，四元组；
 * @param path
 * @param width
 * @param height
 */
void yuv420(const char *path, int width, int height) {
    // 打开
    FILE *fp = fopen(path, "rb+");
    if (fp == NULL) {
        fprintf(stderr, "the yuv file is null");
        return;
    }
    FILE *f1 = fopen(Y_FILE_PATH, "wb+");
    FILE *f2 = fopen(U_FILE_PATH, "wb+");
    FILE *f3 = fopen(V_FILE_PATH, "wb+");


    if (f1 == NULL) {
        fprintf(stderr, "the f1 file is null");
    }
    if (f2 == NULL) {
        fprintf(stderr, "the f2 file is null");
    }
    if (f3 == NULL) {
        fprintf(stderr, "the f3 file is null");
    }


    unsigned char *p = malloc(width * height * 3 / 2);
    int i = 0;
    int item = width * height;
    while (i < 1) {
        fread(p, 1, item * 3 / 2, fp);
        // y 亮度
        fwrite(p, 1, item, f1);
        // u 颜色
        fwrite(p + item, 1, item / 4, f2);
        // v 颜色
        fwrite(p + item * (1 + 1 / 4), 1, item / 4, f3);
        i++;
    }
    free(p);
    p = NULL;
    fclose(fp);
    fclose(f1);
    fclose(f2);
    fclose(f3);


}