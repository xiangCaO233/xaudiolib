#include "config.h"

#include <filesystem>
#include <fstream>
#include <iostream>

#include "log/colorful-log.h"

namespace x {
// 音频传输方式
transfertype Config::audio_transfer_method = transfertype::AUDIO_DATA_BUFFERED;
// 音频解码线程数
int Config::decode_thread_count = 4;
// 混音方式
mixtype Config::mix_method = mixtype::CPU_MIX;
// 混音处理时的环形缓冲区大小(至少为play_buffer_size 的48倍,否则影响音频)
int Config::mix_buffer_size = 12288;
// 播放音频时的声道数
channels Config::channel = channels::AUDIO_DOUBLE_CHANNEL;
// 播放音频时的采样率
sampleratetype Config::samplerate = sampleratetype::AVMEDIA_SAMPLERATE;
// 播放音频的缓冲区大小(有下限)
int Config::play_buffer_size = 256;

// 保存路径
std::string Config::config_file_path() {
    return "./config/audio_settings.json";
}

void Config::to_json(json& j) {
    j = json{{"audio_transfer_method", static_cast<int>(audio_transfer_method)},
             {"decode_thread_count", decode_thread_count},
             {"mix_method", static_cast<int>(mix_method)},
             {"mix_buffer_size", mix_buffer_size},
             {"channel", static_cast<int>(channel)},
             {"samplerate", static_cast<int>(samplerate)},
             {"play_buffer_size", play_buffer_size}};
}
void Config::from_json(const json& j) {
    audio_transfer_method =
        static_cast<transfertype>(j.at("audio_transfer_method").get<int>());
    decode_thread_count = j.at("decode_thread_count").get<int>();
    mix_method = static_cast<mixtype>(j.at("mix_method").get<int>());
    mix_buffer_size = j.at("mix_buffer_size").get<int>();
    channel = static_cast<channels>(j.at("channel").get<int>());
    samplerate = static_cast<sampleratetype>(j.at("samplerate").get<int>());
    play_buffer_size = j.at("play_buffer_size").get<int>();
}

void Config::load() {
    std::ifstream input(config_file_path());
    if (input) {
        json j;
        input >> j;
        from_json(j);
    } else {
        XERROR("无法打开配置文件: " + config_file_path());
    }
};

void Config::save() {
    json j;
    to_json(j);
    // std::cout << std::filesystem::absolute(config_file_path()) << "\n";
    std::ofstream output(config_file_path());
    if (output) {
        // 美化输出，缩进为4
        output << j.dump(4);
    } else {
        XERROR("无法保存配置文件!");
    }
    output.close();
}
}  // namespace x
