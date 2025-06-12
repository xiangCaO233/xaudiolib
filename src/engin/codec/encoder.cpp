#include "encoder.h"

#include "log/colorful-log.h"

XAudioEncoder::XAudioEncoder(AVCodecID id) {
  encoder = avcodec_find_encoder(id);
  if (!encoder)
    XERROR("创建编码器失败");
  else
    XINFO("成功创建[" + std::string(avcodec_get_name(id)) + "]编码器");
}

XAudioEncoder::~XAudioEncoder() = default;
