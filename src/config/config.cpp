#include "config.h"

// 音频传输方式
const int Config::audio_transfer_method = AUDIO_DATA_BUFFERED;
// 音频解码线程数
const int Config::decode_thread_count = 4;
// 混音方式
const int Config::mix_method = CPU_MIX_BY_OPENAL;
// 混音处理时的环形缓冲区大小(至少为play_buffer_size 的32倍,否则影响音频)
const int Config::mix_buffer_size = 2048;
// 播放音频时的声道数
const int Config::channel = AUDIO_DOUBLE_CHANNEL;
// 播放音频时的采样率
const int Config::samplerate = AVMEDIA_SAMPLERATE;
// 播放音频的缓冲区大小
const int Config::play_buffer_size = 64;
// 保存路径
std::string Config::config_file_path = "./config.json";

void Config::load(const std::string &config_file) {
    // TODO(xiang 2024-12-16): 读取配置文件
    config_file_path = config_file;
};

void Config::save() {
    // TODO(xiang 2024-12-16): 保存配置文件
}
