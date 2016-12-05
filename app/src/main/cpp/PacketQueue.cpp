//
// Created by huifangwu on 2016/12/5.
//


#include "PacketQueue.h"

int PacketQueue::put_packet(AVPacket *pkt) {
    if (abort_request) {
        av_log(NULL, AV_LOG_INFO, "put_packet abort.\n");
        return -1;
    }
    int ret;
    std::unique_lock<std::mutex> lock(mutex);
    while (true) {
        if (queue.size() < MAX_SIZE) {
            AVPacket *packet = (AVPacket *)av_malloc(sizeof(AVPacket));
            if (packet == NULL) {
                av_log(NULL, AV_LOG_FATAL, "Could not create new AVPacket.\n");
                return -1;
            }
            ret = av_copy_packet(packet, pkt);
            if (ret != 0) {
                av_log(NULL, AV_LOG_FATAL, "Could not copy AVPacket.\n");
                return -1;
            }
            queue.push(*packet);
            duration += packet->duration;
            cond.notify_one();
            break;
        }else {
            full.wait(lock);
        }
    }
    return 0;
}


int PacketQueue::get_packet(AVPacket *pkt) {
    std::unique_lock<std::mutex> lock(mutex);
    for (;;) {
        if (abort_request) {
            return -1;
        }
        if (queue.size() > 0) {
            AVPacket tmp = queue.front();
            av_copy_packet(pkt, &tmp);
            duration -= tmp.duration;
            queue.pop();
            av_packet_unref(&tmp);
            full.notify_one();
            return 0;
        }else {
            cond.wait(lock);
        }
    }
}


int PacketQueue::put_nullpacket() {
    AVPacket *pkt = new AVPacket();
    av_init_packet(pkt);
    pkt->data = NULL;
    pkt->size = 0;
    put_packet(pkt);
    return 0;
}


void PacketQueue::set_abort(int abort) {
    abort_request = abort;
}

int PacketQueue::get_abort() {
    return abort_request;
}

int PacketQueue::get_serial(){
    return serial;
}

size_t PacketQueue::get_queue_size() {
    return queue.size();
}


std::shared_ptr<Frame> FrameQueue::get_frame() {
    std::unique_lock<std::mutex> lock(mutex);
    for (;;) {
        if (queue.size() > 0) {
            auto tmp = queue.front();
            queue.pop();
            full.notify_one();
            return tmp;
        }else {
            empty.wait(lock);
        }
    }
}
