//
// Created by huifangwu on 2016/12/5.
//


#include "Decoder.h"

void Decoder::init(AVCodecContext *ctx) {
    avctx = ctx;

}


void Decoder::start_decode_thread() {
    pkt_queue.set_abort(0);
    std::thread t(&Decoder::decode, this);
    t.detach();
}


