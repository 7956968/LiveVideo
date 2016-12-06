//
// Created by huifangwu on 2016/12/6.
//

#ifndef LIVEVIDEO_VIDEODECODER_H
#define LIVEVIDEO_VIDEODECODER_H

#include <Decoder.h>

class VideoDecoder : public Decoder {
public:
    virtual int decoder_decode_frame() override ;
    virtual void decode() override ;
    int get_width();
    int get_height();
};
#endif //LIVEVIDEO_VIDEODECODER_H
