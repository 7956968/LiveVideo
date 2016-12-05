//
// Created by huifangwu on 2016/12/5.
//

#ifndef LIVEVIDEO_FRAMEQUEUE_H
#define LIVEVIDEO_FRAMEQUEUE_H
extern "C"{
#include <libavutil/frame.h>
}


#include <mutex>
#include <memory>
#include <condition_variable>
#include <queue>
#include "Frame.h"

class FrameQueue {
public:
    void put_frame(AVFrame *frame);
    std::shared_ptr<Frame> get_frame();
    size_t get_size();
private:
    std::queue<std::shared_ptr<Frame>> queue;
    std::mutex mutex;
    std::condition_variable empty;
    std::condition_variable full;
    const size_t MAX_SIZE = 16;
};
#endif //LIVEVIDEO_FRAMEQUEUE_H
