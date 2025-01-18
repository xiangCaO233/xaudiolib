#include "encoder.h"
#include <iostream>

XAudioEncoder::XAudioEncoder(AVCodecID id) {
  encoder = avcodec_find_encoder(id);
  if (!encoder)
    std::cout << "创建编码器失败" << std::endl;
  else
    std::cout << "成功创建编码器:[" + std::string(avcodec_get_name(id)) + "]"
              << std::endl;
}

XAudioEncoder::~XAudioEncoder() = default;
