#ifndef X_AUDIO_MANAGER_H
#define X_AUDIO_MANAGER_H

#include <cstdint>
#include <utility>
#include <vector>

#include "engin/codec/decoder.h"
#include "engin/codec/encoder.h"

extern "C" {
#include <libavformat/avformat.h>
}

#include <memory>
#include <string>
#include <unordered_map>

class XAudioManager {
   private:
    // 变量
    // 音频文件路径-句柄
    std::unordered_map<std::string, int> audio_handles;
    // 句柄-音频格式
    std::unordered_map<int, std::shared_ptr<AVFormatContext>> audio_formats;
    // 句柄-缓存音频pcm数据
    std::unordered_map<int, std::vector<float>> audio_pcm_datas;
    // 句柄-音频播放速率
    std::unordered_map<int, float> audio_speeds;
    // 句柄-音频名
    std::unordered_map<int, std::string> audio_names;
    // 句柄-音频路径
    std::unordered_map<int, std::string> audio_paths;
    // 句柄-音频音量
    std::unordered_map<int, float> audio_volumes;
    // 后缀名-编解码器
    std::unordered_map<std::string, std::pair<XAudioDecoder, XAudioEncoder>>
        audio_codecs;

    // 载入音频
    int loadaudio(const std::string &audio);
    // 卸载音频
    void unloadaudio(const std::string &audio);
    void unloadaudio(int id);
    // 获取音频名
    const std::string &get_audio_name(int id);
    // 获取音频路径
    const std::string &get_audio_path(int id);
    // 设置音频当前播放到的位置
    void set_audio_current_pos(const std::string &auido, int64_t time);
    void set_audio_current_pos(int id, int64_t time);
    // 获取音频音量
    float getVolume(const std::string &audio);
    float getVolume(int id);
    // 设置音频音量
    void setAudioVolume(const std::string &audio);
    void setAudioVolume(int id);

   public:
    // 构造XAudioManager音频管理器
    XAudioManager();
    // 析构XAudioManager
    virtual ~XAudioManager();
    static std::shared_ptr<XAudioManager> newmanager();
};

#endif  // X_AUDIO_MANAGER_H
