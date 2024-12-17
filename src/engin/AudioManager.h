#ifndef X_AUDIO_MANAGER_H
#define X_AUDIO_MANAGER_H

#include "AudioEngin.h"

class XAudioManager {
   private:
    // 引擎(唯一)
    std::unique_ptr<XAudioEngin> engin;
    // 音频的句柄
    std::unordered_map<std::string, int> handles;

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
    void setAudioVolume(const std::string &audio, float v);
    void setAudioVolume(int id, float v);

    // 设置全局音量
    float getGlobalVolume();
    void setGlobalAudioVolume(float volume);

    // 播放和暂停音频
    void playAudio(int device_index, int id, bool isloop);
    void playAudio(const std::string &device, const std::string &audioname,
                   bool isloop);
    void playAudio(int device_index, const std::string &audioname, bool isloop);
    void playAudio(const std::string &device, int id, bool isloop);

    void pauseAudio(int device_index, int id);
    void pauseAudio(const std::string &device, const std::string &audioname);
    void pauseAudio(int device_index, const std::string &audioname);
    void pauseAudio(const std::string &device, int id);

    // 设备是否暂停
    bool isDevicePause(int device_id);
    bool isDevicePause(const std::string &devicename);

    // 播放暂停停止设备上的播放器
    void pauseDevice(int device_id);
    void pauseDevice(const std::string &devicename);

    void resumeDevice(int device_id);
    void resumeDevice(const std::string &devicename);

    void stopDevice(int device_id);
    void stopDevice(const std::string &devicename);

   public:
    // 构造XAudioManager音频管理器
    XAudioManager();
    // 析构XAudioManager
    virtual ~XAudioManager();

    static std::shared_ptr<XAudioManager> newmanager();
};

#endif  // X_AUDIO_MANAGER_H
