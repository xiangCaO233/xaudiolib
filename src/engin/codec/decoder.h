#ifndef X_AUDIO_DECODER_H
#define X_AUDIO_DECODER_H

#include <memory>
#include <vector>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavcodec/codec.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libswresample/swresample.h>
}

class XAudioDecoder {
    // 解码器(ffmpeg 自动管理)
    const AVCodec* decoder;
    // 解码器上下文
    AVCodecContext* decoder_context;
    // decoderid
    AVCodecID decoder_id_name;
    // 重采样器
    SwrContext* resampler{nullptr};

   public:
    // 构造XAudioDecoder
    XAudioDecoder(AVCodecID id);
    // 析构XAudioDecoder
    virtual ~XAudioDecoder();
    // 解码整个音频
    int decode_audio(std::shared_ptr<AVFormatContext>& format, int streamIndex,
                     std::vector<float>& pcm_data);
};

#endif  // X_AUDIO_DECODER_H
