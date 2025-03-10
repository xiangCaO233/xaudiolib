#ifndef X_AUDIO_DECODER_H
#define X_AUDIO_DECODER_H

#include <vector>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavcodec/codec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}

class XAudioDecoder {
  // 解码器(ffmpeg 自动管理)
  const AVCodec* decoder;
  // 解码器上下文
  AVCodecContext* decoder_context{nullptr};
  // decoderid
  AVCodecID decoder_id_name{AVCodecID::AV_CODEC_ID_MP3};
  // 重采样器
  SwrContext* resampler{nullptr};

 public:
  // 构造XAudioDecoder
  explicit XAudioDecoder(AVCodecID id);
  // 析构XAudioDecoder
  virtual ~XAudioDecoder();
  // 解码整个音频
  int decode_audio(AVFormatContext* format,
                   int streamIndex, std::vector<float>& pcm_data);
};

#endif  // X_AUDIO_DECODER_H
