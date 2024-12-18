#ifndef X_AUDIO_MIXER_H
#define X_AUDIO_MIXER_H

#include <memory>
#include <thread>
#include <unordered_map>
#include <vector>

class XSound;
class XPlayer;
class XAudioEngin;

class XAuidoMixer {
    // 混音线程
    std::thread mixthread;
    // 目标播放器指针(仅传递方便访问,不释放,其他地方已管理)
    XPlayer* des_player;
    // 全部音轨(音频句柄-音频)
    std::unordered_map<int, std::shared_ptr<XSound>> audio_orbits;
    // 音轨循环标识(句柄-循环标识)
    std::unordered_map<int, bool> audio_loopflags;

    friend XAudioEngin;
    friend XPlayer;

    // 混合音频
    void mix(std::vector<std::shared_ptr<XSound>>& src_sounds,
             std::vector<float>& mixed_pcm, float global_volume);

    // 向播放器发送数据的线程函数
    void send_pcm_thread();

   public:
    // 构造XAuidoMixer
    XAuidoMixer(XPlayer* player);
    // 析构XAuidoMixer
    virtual ~XAuidoMixer();
};

#endif  // X_AUDIO_MIXER_H
