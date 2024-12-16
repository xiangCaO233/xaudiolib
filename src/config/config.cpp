#include "config.h"

// 音频传输方式
int Config::audio_transfer_method = AUDIO_DATA_BUFFERED;
// 音频解码线程数(max128)
int Config::decode_thread_count = 4;
// 混音方式
int Config::mix_method = CPU_MIX_BY_OPENAL;
// GPU混音时的环形缓冲区大小
int Config::gpu_buffer_size = 512;
// 播放音频的缓冲区大小
int Config::play_buffer_size = 128;

Config::Config(std::string &config_file) {}

Config::~Config() {}
