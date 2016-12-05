//
// Created by huifangwu on 2016/12/5.
//

#include "FrameQueue.h"

void FrameQueue::put_frame(AVFrame *frame) {
    std::unique_lock<std::mutex> lock(mutex);
    while (true) {
        if (queue.size() < MAX_SIZE) {
            auto m_frame = std::make_shared<Frame>(frame);
            queue.push(m_frame);
            empty.notify_one();
            return;
        }else{
            full.wait(lock);
        }
    }

}

size_t FrameQueue::get_size() {
    return queue.size();
}

