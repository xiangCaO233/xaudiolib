#ifndef X_AUDIO_ENGIN_H
#define X_AUDIO_ENGIN_H

#include "engin/sdl/xplayer.h"
#if defined(__APPLE__)
// 苹果coreaudio库
#include <AudioToolbox/AudioToolbox.h>
#include <CoreAudio/CoreAudio.h>
#elif defined(__linux__)
#elif defined(_WIN32)
#elif defined(_WIN64)
#elif defined(__ANDROID__)
#else
#endif

#include <unordered_map>

#include "Sound.h"
#include "engin/codec/decoder.h"
#include "engin/codec/encoder.h"
#include "engin/device/indevice.h"
#include "engin/device/outdevice.h"
#include "engin/mix/mixer.h"

class XAudioEngin {
    // 引擎接口
    // 当前注册到的id
    static int currentid;

    // 后缀名-编解码器池
    std::unordered_map<std::string, std::pair<XAudioDecoder, XAudioEncoder>>
        audio_codecs;

    // 音频句柄池
    std::unordered_map<std::string, int> handles;
    // 音频池
    std::unordered_map<int, std::shared_ptr<XSound>> audios;

    // 设备列表
    std::unordered_map<int, std::shared_ptr<XOutputDevice>> outdevices;
    std::unordered_map<std::string, int> outdevice_indicies;
    std::unordered_map<int, std::shared_ptr<XInputDevice>> inputdevices;
    std::unordered_map<std::string, int> inputdevice_indicies;

    // 全局音量
    float gVolume{0.5f};

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

    // 设置全局音量
    void setGlobalVolume(float volume);

    // 播放
    void play(int device_index, int audio_id, bool loop);
    void play(const std::string &devicename, int audio_id, bool loop);
    void play(int device_index, const std::string &audioname, bool loop);
    void play(const std::string &devicename, const std::string &audioname,
              bool loop);

    // 暂停
    void pause(int device_index, int audio_id);
    void pause(const std::string &devicename, int audio_id);
    void pause(int device_index, const std::string &audioname);
    void pause(const std::string &devicename, const std::string &audioname);

    // 恢复
    void resume(int device_index, int audio_id);
    void resume(const std::string &devicename, int audio_id);
    void resume(int device_index, const std::string &audioname);
    void resume(const std::string &devicename, const std::string &audioname);

    // 终止
    void stop(int device_index, int audio_id);
    void stop(const std::string &devicename, int audio_id);
    void stop(int device_index, const std::string &audioname);
    void stop(const std::string &devicename, const std::string &audioname);

    // 获取设备播放器状态
    bool is_pause(int device_id);
    bool is_pause(const std::string &devicename);

    // 暂停播放器
    void pause(int device_id);
    void pause(const std::string &devicename);

    // 恢复播放器
    void resume(int device_id);
    void resume(const std::string &devicename);

    // 播放暂停停止设备上的播放器
    void pause_device(int device_id);
    void pause_device(const std::string &devicename);

    void resume_device(int device_id);
    void resume_device(const std::string &devicename);

    void stop_player(int device_id);
    void stop_player(const std::string &devicename);

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
