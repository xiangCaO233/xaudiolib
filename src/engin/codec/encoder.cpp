#include "encoder.h"

#include "logger/logger.h"

XAudioEncoder::XAudioEncoder(AVCodecID id) {
    encoder = avcodec_find_encoder(id);
    if (!encoder)
        LOG_ERROR("创建编码器失败");
    else
        LOG_INFO("成功创建编码器:[" + std::string(avcodec_get_name(id)) + "]");
}

XAudioEncoder::~XAudioEncoder() {}
