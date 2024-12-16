#ifndef X_AUDIO_ENGIN_H
#define X_AUDIO_ENGIN_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "engin/codec/decoder.h"
#include "engin/codec/encoder.h"
#include "engin/device/indevice.h"
#include "engin/device/outdevice.h"
#include "engin/mix/mixer.h"

extern "C" {
#include <libavformat/avformat.h>
}

class XAudioManager;
class XAudioEngin;

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
    friend XAudioEngin;

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

class XAudioEngin {
    static int currentid;

    // 后缀名-编解码器
    std::unordered_map<std::string, std::pair<XAudioDecoder, XAudioEncoder>>
        audio_codecs;

    // 音频的句柄
    std::unordered_map<std::string, int> handles;
    // 全部音频
    std::unordered_map<int, std::shared_ptr<XSound>> audios;

    // 设备列表
    std::unordered_map<int, std::shared_ptr<XOutputDevice>> outdevices;
    std::unordered_map<int, std::shared_ptr<XInputDevice>> inputdevices;

    // 混音器
    XAuidoMixer mixer;

    // 载入音频
    int load(const std::string &audio);

    // 卸载音频
    void unload(const std::string &audio);
    void unload(int id);

    // 获取音频名
    const std::string &name(int id);

    // 获取音频路径
    const std::string &path(int id);

    // 设置音频当前播放到的位置
    void pos(const std::string &auido, int64_t time);
    void pos(int id, int64_t time);

    // 获取音频音量
    float volume(const std::string &audio);
    float volume(int id);

    // 设置音频音量
    void setVolume(const std::string &audio, float v);
    void setVolume(int id, float v);

    friend XAudioManager;

   public:
    // 创建XAudioEngin 引擎
    XAudioEngin();
    virtual ~XAudioEngin();
    // 初始化引擎
    static std::unique_ptr<XAudioEngin> init();
    // 关闭引擎
    void shutdown();
};

#endif  // X_AUDIO_ENGIN_H
