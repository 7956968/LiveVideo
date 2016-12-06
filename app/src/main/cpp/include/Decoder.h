//
// Created by huifangwu on 2016/12/5.
//

#ifndef LIVEVIDEO_DECODER_H
#define LIVEVIDEO_DECODER_H
extern "C"{
#include <libavcodec/avcodec.h>
}

#include <thread>
#include <condition_variable>
#include "PacketQueue.h"

class Decoder {
public:
    virtual int decoder_decode_frame() = 0;
    virtual void decode() = 0;
    void init(AVCodecContext *ctx);
    void start_decode_thread();
    PacketQueue pkt_queue;
    FrameQueue frame_queue;
    AVCodecContext *avctx;
protected:
    AVPacket pkt;
    AVPacket pkt_temp;
    int pkt_serial;
    int finished;
    int packet_pending;
    std::condition_variable empty_queue_cond;
    int64_t start_pts;
    AVRational start_pts_tb;
    int64_t next_pts;
    AVRational next_pts_tb;
};


#endif //LIVEVIDEO_DECODER_H
