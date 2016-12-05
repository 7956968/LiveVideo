//
// Created by huifangwu on 2016/12/5.
//

#ifndef LIVEVIDEO_FRAME_H
#define LIVEVIDEO_FRAME_H
extern "C"{
#include <libavutil/frame.h>
}

#include <cstdint>

struct Frame {
    Frame(AVFrame *f) : frame(f){

    }
    AVFrame *frame;
    int serial;
    double pts;           /* presentation timestamp for the frame */
    double duration;      /* estimated duration of the frame */
    int64_t pos;          /* byte position of the frame in the input file */
    int allocated;
    int width;
    int height;
    int format;
    AVRational sar;
    int uploaded;
};
#endif //LIVEVIDEO_FRAME_H
