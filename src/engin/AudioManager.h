#ifndef X_AUDIO_MANAGER_H
#define X_AUDIO_MANAGER_H

#include <string>

class XAudioManager {
   private:
    void loadaudio(std::string &audio);
    void unloadaudio(std::string &audio);
    void unloadaudio(int id);

   public:
    // 构造XAudioManager音频管理器
    XAudioManager();
    // 析构XAudioManager
    virtual ~XAudioManager();
    static XAudioManager *newmanager();
};

#endif  // X_AUDIO_MANAGER_H
