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
    // 播放位置
    int playpos;
    // pcm声音数据
    std::vector<uint32_t> pcm_data;
    // 暂停标识
    bool pauseflag;
    // 播放速率
    float speed;
    // 音量
    float volume;

    // 未知常量
    static std::string unknown;
    static std::string unknown_path;

    friend XAudioManager;
    friend XAudioEngin;
    friend XAuidoMixer;

   public:
    // 构造XSound
    XSound(int h, std::string n, std::string p,
           std::shared_ptr<AVFormatContext> f, float s, float vm)
        : handle(h),
          playpos(0),
          pauseflag(false),
          name(n),
          path(p),
          audio_format(f),
          speed(s),
          volume(vm){};
    // 析构XSound
    virtual ~XSound() = default;

    // 音频文件名
    const std::string name;
    // 音频路径
    const std::string path;
    // 音频格式
    const std::shared_ptr<AVFormatContext> audio_format;
};

#endif  // X_AUDIO_SOUND_H
