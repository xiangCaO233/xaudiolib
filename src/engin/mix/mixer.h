#ifndef X_AUDIO_MIXER_H
#define X_AUDIO_MIXER_H

#include <list>
#include <memory>
#include <thread>
#include <unordered_map>

class XSound;
class XPlayer;
class XAudioEngin;

class XAuidoMixer {
    // 混音线程
    std::thread mixthread;
    // 目标播放器指针(不释放,其他地方已管理)
    XPlayer* des_player;
    // 全部音轨(音频句柄-音频)
    std::unordered_map<int, std::shared_ptr<XSound>> audio_orbits;

    friend XAudioEngin;

   public:
    // 构造XAuidoMixer
    XAuidoMixer(XPlayer* player);
    // 析构XAuidoMixer
    virtual ~XAuidoMixer();
};

#endif  // X_AUDIO_MIXER_H
