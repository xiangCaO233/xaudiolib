#ifndef X_AUDIO_DECODER_H
#define X_AUDIO_DECODER_H

extern "C" {
#include <libavcodec/avcodec.h>
}

class XAudioDecoder {
    // 解码器
    AVCodec codec;
    // 解码器上下文
    AVCodecContext codec_context;
    // decoderid
    AVCodecID decoder_id_name;

   public:
    // 构造XAudioDecoder
    XAudioDecoder(AVCodecID id);
    // 析构XAudioDecoder
    virtual ~XAudioDecoder();
};

#endif  // X_AUDIO_DECODER_H
