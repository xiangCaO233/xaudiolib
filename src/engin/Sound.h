#ifndef X_AUDIO_SOUND_H
#define X_AUDIO_SOUND_H

#include <memory>
#include <string>
#include <vector>

class XAudioManager;
class XAudioEngin;
class XAuidoMixer;

struct AVFormatContext;

class XSound {
    // 句柄(id)
    int handle;
    // pcm声音数据
    std::vector<float> pcm_data;

    // 未知常量
    static std::string unknown;
    static std::string unknown_path;

    friend XAudioManager;
    friend XAudioEngin;
    friend XAuidoMixer;

   public:
    // 构造XSound
    XSound(int h, std::string n, std::string p,
           std::shared_ptr<AVFormatContext> f);
    // 析构XSound
    virtual ~XSound() = default;

    // 调整位置(按帧)
    void locateframe(size_t frameindex);
    // 调整位置(按采样)
    void locatesample(size_t sampleindex);
    // 调整位置(按时间)
    void locatetime(size_t milliseconds);

    // 音频文件名
    const std::string name;
    // 音频路径
    const std::string path;
    // 音频格式
    const std::shared_ptr<AVFormatContext> audio_format;
};

#endif  // X_AUDIO_SOUND_H
