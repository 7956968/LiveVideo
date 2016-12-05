//
// Created by Administrator on 2016/11/10.
//

#ifndef EASYPLAYER_EASYPLAYER_H
#define EASYPLAYER_EASYPLAYER_H


extern "C"{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
#include "libavutil/opt.h"
#include "libavutil/imgutils.h"
#include "libavutil/time.h"
#include "libavcodec/avfft.h"
};

#include <mutex>
#include <thread>
#include <queue>
#include <condition_variable>
#include <memory>
#include <unistd.h>
#include "PlayerState.h"
#include "Decoder.h"

class EasyPlayer {
public:
    EasyPlayer() = default;
    void init(const std::string input_filename);
    bool has_video();
    bool get_img_frame(AVFrame *frame);
    bool get_aud_buffer(int &nextSize, uint8_t *outputBuffer);
    void wait_state(PlayerState need_state);
    void wait_paused();
    void release();
    void togglePaused() {
        std::unique_lock<std::mutex> lock(mutex);
        paused = !paused;
        pause_condition.notify_all();
    }
    bool get_paused() {
        return paused;
    }
    AVFormatContext *ic;
    char *filename;
    int abort_request;
    int force_refresh;

    int last_paused;
    int queue_attachments_req;
    int seek_req;
    int seek_flags;
    int64_t seek_pos;
    int64_t seek_rel;
    int read_pause_return;
    bool realtime;
    AudioDecoder auddec;
    VideoDecoder viddec;
    PlayerState state = PlayerState::UNKNOWN;
private:
    void read();
    bool is_realtime();
    int stream_component_open(int stream_index);
    void on_state_change(PlayerState state);
    int last_video_stream, last_audio_stream;
    int video_stream = -1;
    AVStream *video_st;
    bool paused = false;
    double max_frame_duration;      // maximum duration of a frame - above this, we consider the jump a timestamp discontinuity
    struct SwsContext *img_convert_ctx;

    double audio_clock;
    int audio_clock_serial;
    double audio_diff_cum; /* used for AV difference average computation */
    double audio_diff_avg_coef;
    double audio_diff_threshold;
    int audio_diff_avg_count;
    AVStream *audio_st;

    int audio_hw_buf_size;
    uint8_t *audio_buf;
    uint8_t *audio_buf1;
    unsigned int audio_buf_size; /* in bytes */
    unsigned int audio_buf1_size;
    int audio_buf_index; /* in bytes */
    int audio_write_buf_size;
    int audio_volume;
    int muted;
    struct SwrContext *swr_ctx;
    int frame_drops_early;
    int frame_drops_late;
    int eof;

    int audio_stream = -1;
    int av_sync_type;

    int64_t start_time = AV_NOPTS_VALUE;
    int64_t duration = AV_NOPTS_VALUE;
    std::mutex mutex;
    std::condition_variable state_condition;
    std::condition_variable pause_condition;
};

#endif //EASYPLAYER_EASYPLAYER_H
