#include "../XAudioManager.h"

// 接口实现
void XAudioManager::load(const std::string &audio_path) {
    loadaudio(audio_path);
}

void XAudioManager::unload(const std::string &audio_path) {
    unloadaudio(audio_path);
}

void XAudioManager::unload(int audio_id) { unloadaudio(audio_id); }

float XAudioManager::volume(const std::string &audio_name) {
    return getVolume(audio_name);
}

float XAudioManager::volume(int audio_id) { return getVolume(audio_id); }

const std::string &XAudioManager::audio_name(int audio_id) {
    return get_audio_name(audio_id);
};

const std::string &XAudioManager::audio_path(int audio_id) {
    return get_audio_path(audio_id);
};
// 设置播放时间
void XAudioManager::set_audio_time(const std::string &audio_name,
                                   int64_t time) {
    set_audio_current_pos(audio_name, time);
};
void XAudioManager::set_audio_time(int audio_id, int64_t time) {
    set_audio_current_pos(audio_id, time);
};
// 获取全局音量
float XAudioManager::globalVolume() { return getGlobalVolume(); };
// 设置全局音量
void XAudioManager::setGlobalVolume(float volume) {
    setGlobalAudioVolume(volume);
};
// 是否循环播放句柄
void XAudioManager::play(int audio_id, bool loop) {
    playAudio(audio_id, loop);
};
// 暂停音频句柄
void XAudioManager::pause(int audio_id) { pauseAudio(audio_id); };
