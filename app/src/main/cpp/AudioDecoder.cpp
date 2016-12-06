//
// Created by huifangwu on 2016/12/6.
//

#include "AudioDecoder.h"


int AudioDecoder::decoder_decode_frame() {
    int ret;

    do {

        if (pkt_queue.get_abort())
            return -1;
        if (!packet_pending || pkt_queue.get_serial() != pkt_serial) {

            if (pkt_queue.get_packet(&pkt) < 0) return -1;
        }
        if (pkt.data == NULL) {
            av_log(NULL, AV_LOG_FATAL, "reach eof.\n");
            return -1;
        }
        ret = avcodec_send_packet(avctx, &pkt);
        if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF)
            break;
        AVFrame *frame = av_frame_alloc();
        ret = avcodec_receive_frame(avctx, frame);
        if (ret < 0 && ret != AVERROR_EOF)
            break;
        frame->pts = av_frame_get_best_effort_timestamp(frame);
        frame_queue.put_frame(frame);
        if (ret < 0) {
            packet_pending = 0;
        } else {

        }
    } while (ret != 0 && !finished);

    return 0;
}


void AudioDecoder::decode() {

    AVFrame *frame = av_frame_alloc();
    for (;;) {
        if (pkt_queue.get_abort()) break;
        int got;
        if ((got = decoder_decode_frame()) < 0)
            return;
    }
}


int AudioDecoder::get_channels() {
    return avctx->channels;
}

int AudioDecoder::get_sample_rate(){
    return avctx->sample_rate;
}
