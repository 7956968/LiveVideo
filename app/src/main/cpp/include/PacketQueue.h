//
// Created by huifangwu on 2016/12/5.
//

#ifndef LIVEVIDEO_PACKETQUEUE_H
#define LIVEVIDEO_PACKETQUEUE_H
extern "C"{
#include <libavcodec/avcodec.h>
}

#include <queue>
#include <mutex>
#include <FrameQueue.h>

class PacketQueue {
public:
    int put_packet(AVPacket *pkt);
    int get_packet(AVPacket *pkt);
    int put_nullpacket();
    void set_abort(int abort);
    int get_abort();
    int get_serial();
    size_t get_queue_size();
private:
    std::queue<AVPacket> queue;
    int64_t duration;
    int abort_request = 1;
    int serial;
    std::mutex mutex;
    std::condition_variable cond;
    std::condition_variable full;
    const size_t MAX_SIZE = 8;

};
#endif //LIVEVIDEO_PACKETQUEUE_H
