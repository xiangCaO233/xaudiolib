#ifndef API_X_AUDIO_MANAGER_H
#define API_X_AUDIO_MANAGER_H

#include <memory>
#include <string>

class XAudioManager {
   private:
    // 内部接口
    void loadaudio(const std::string &audio);
    void unloadaudio(const std::string &audio);
    void unloadaudio(int id);
    const std::string &get_audio_name(int id);
    const std::string &get_audio_path(int id);
    float getVolume(const std::string &audio);
    float getVolume(int id);
    void setAudioVolume(const std::string &audio);
    void setAudioVolume(int id);

   public:
    // 公开接口
    static std::shared_ptr<XAudioManager> newmanager();
    // 载入音频
    void load(const std::string &audio_path);

    // 卸载音频
    void unload(const std::string &audio_name);
    void unload(int audio_id);

    // 获取音频信息
    const std::string &audio_name(int audio_id);
    const std::string &audio_path(int audio_id);

    // 获取音频音量
    float volume(const std::string &audio_name);
    float volume(int audio_id);

    // 设置音频音量
    void setVolume(const std::string &audio_name, float volume);
    void setVolume(int audio_id, float volume);

    // 获取全局音量
    float globalVolume();

    // 设置全局音量
    void setGlobalVolume(float volume);
};

#endif  // API_X_AUDIO_MANAGER_H
