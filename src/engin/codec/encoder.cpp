#include "encoder.h"

#include <iostream>

XAudioEncoder::XAudioEncoder(AVCodecID id) {
  encoder = avcodec_find_encoder(id);
  if (!encoder)
    std::cout << "创建编码器失败" << std::endl;
  else
    std::cout << "成功创建[" + std::string(avcodec_get_name(id)) + "]编码器"
              << std::endl;
}

XAudioEncoder::~XAudioEncoder() = default;
