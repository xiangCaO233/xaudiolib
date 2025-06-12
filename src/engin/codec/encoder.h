#ifndef X_AUDIO_ENCODER_H
#define X_AUDIO_ENCODER_H

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavcodec/codec.h>
#include <libavutil/opt.h>
}

class XAudioEncoder {
  // 编码器(ffmpeg 自动管理)
  const AVCodec* encoder;
  // 编码器上下文
  AVCodecContext encoder_context;
  // encoderid
  AVCodecID encoder_id_name;

 public:
  // 构造XAudioEncoder
  explicit XAudioEncoder(AVCodecID id);
  // 析构XAudioEncoder
  virtual ~XAudioEncoder();
};

#endif  // X_AUDIO_ENCODER_H
