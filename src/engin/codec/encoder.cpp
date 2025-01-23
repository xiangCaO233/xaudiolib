#include "encoder.h"

#include <iostream>

XAudioEncoder::XAudioEncoder(AVCodecID id) {
  encoder = avcodec_find_encoder(id);
  if (!encoder)
    std::cout << "" << std::endl;
  else
    std::cout << "" + std::string(avcodec_get_name(id)) + "]"
    << std::endl;
}

XAudioEncoder::~XAudioEncoder() = default;
