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

class XAudioManager;

class XSound {
    // 句柄(id)
    int handle;
    // pcm声音数据
    std::vector<float> pcm_data;
    // 播放速率
    float speed;
    // 音量
    float volume;

    // 未知常量
    static std::string unknown;
    static std::string unknown_path;
    friend XAudioManager;

   public:
    // 构造XSound
    XSound(int h, std::string n, std::string p,
           std::shared_ptr<AVFormatContext> f, float s, float vm)
        : handle(h), name(n), path(p), audio_format(f), speed(s), volume(vm){};
    // 析构XSound
    virtual ~XSound() = default;

    // 音频文件名
    const std::string name;
    // 音频路径
    const std::string path;
    // 音频格式
    const std::shared_ptr<AVFormatContext> audio_format;
};

class XAudioManager {
   private:
    // 音频的句柄
    std::unordered_map<std::string, int> handles;
    // 全部音频
    std::unordered_map<int, std::shared_ptr<XSound>> audios;
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
