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
// 播放音频句柄
void XAudioManager::play(int device_index, int audio_id, bool loop) {
    playAudio(device_index, audio_id, loop);
};
void XAudioManager::play(const std::string &devicename,
                         const std::string &audioname, bool loop) {
    playAudio(devicename, audioname, loop);
};
void XAudioManager::play(int device_index, const std::string &audioname,
                         bool loop) {
    playAudio(device_index, audioname, loop);
};
void XAudioManager::play(const std::string &devicename, int audio_id,
                         bool loop) {
    playAudio(devicename, audio_id, loop);
};

// 暂停音频句柄
void XAudioManager::pause(int device_index, int audio_id) {
    pauseAudio(device_index, audio_id);
};
void XAudioManager::pause(const std::string &devicename,
                          const std::string &audioname) {
    pauseAudio(devicename, audioname);
};
void XAudioManager::pause(int device_index, const std::string &audioname) {
    pauseAudio(device_index, audioname);
};
void XAudioManager::pause(const std::string &devicename, int audio_id) {
    pauseAudio(devicename, audio_id);
};
