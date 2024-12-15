#include "XAudioManager.h"

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
