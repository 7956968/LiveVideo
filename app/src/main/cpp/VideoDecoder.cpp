//
// Created by huifangwu on 2016/12/6.
//

#include "VideoDecoder.h"


int VideoDecoder::get_height() {
    if (!avctx) return 0;
    return avctx->height;
}

int VideoDecoder::get_width(){
    if (!avctx) return 0;
    return avctx->width;
}

void VideoDecoder::decode() {
    for (;;) {
        if (pkt_queue.get_abort()) break;
        int got_picture;
        if ((got_picture = decoder_decode_frame()) < 0)
            return;
    }
}


int VideoDecoder::decoder_decode_frame() {
    int ret;
    do {
        if (pkt_queue.get_abort())
            return -1;
        if (!packet_pending || pkt_queue.get_serial() != pkt_serial) {
            if (pkt_queue.get_packet(&pkt) < 0) return -1;
            if (pkt.data == NULL) {
                av_log(NULL, AV_LOG_FATAL, "reach eof.\n");
                return -1;
            }

        }
        ret = avcodec_send_packet(avctx, &pkt);
        if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF) {
            av_log(NULL, AV_LOG_FATAL, "video avcodec_send_packet error %d.\n", ret);
            break;
        }

        AVFrame *frame = av_frame_alloc();
        ret = avcodec_receive_frame(avctx, frame);
        if (ret < 0 && ret != AVERROR_EOF) {
            av_log(NULL, AV_LOG_FATAL, "video avcodec_receive_frame error %d.\n", ret);
            break;
        }
        frame->pts = av_frame_get_best_effort_timestamp(frame);
        frame_queue.put_frame(frame);
        if (ret < 0) {
            packet_pending = 0;
        } else {

        }
    } while (ret != 0 && !finished);
    return 0;
}
