//
// Created by huifangwu on 2016/12/6.
//

#ifndef LIVEVIDEO_AUDIODECODER_H
#define LIVEVIDEO_AUDIODECODER_H


#include <Decoder.h>

class AudioDecoder : public Decoder {
public:
    virtual int decoder_decode_frame() override ;
    virtual void decode() override ;
    int get_channels();
    int get_sample_rate();
};

#endif //LIVEVIDEO_AUDIODECODER_H
