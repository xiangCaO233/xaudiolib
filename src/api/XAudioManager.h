#ifndef API_X_AUDIO_MANAGER_H
#define API_X_AUDIO_MANAGER_H

#include <string>
class XAudioManager {
   private:
    void loadaudio(std::string &audio);
    void unloadaudio(std::string &audio);
    void unloadaudio(int id);

   public:
    static XAudioManager *newmanager();
    // 载入音频
    void load(std::string &audio_path);

    // 卸载音频
    void unload(std::string &audio_name);
    void unload(int audio_id);

    // 获取音频音量
    float volume(std::string &audio_name);
    float volume(int audio_id);

    // 设置音频音量
    void setVolume(std::string &audio_name, float volume);
    void setVolume(int audio_id, float volume);

    // 获取全局音量
    float globalVolume();

    // 设置全局音量
    void globalVolume(float volume);
};

#endif  // API_X_AUDIO_MANAGER_H
