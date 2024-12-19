#include "../XAudioManager.h"

// 接口实现
void XAudioManager::load(const std::string &audio_path) {
    loadaudio(audio_path);
}

void XAudioManager::unload(const std::string &audio_path) {
    unloadaudio(audio_path);
}

void XAudioManager::unload(int audio_id) { unloadaudio(audio_id); }

float XAudioManager::volume(int deviceid, int id) {
    return getVolume(deviceid, id);
}
float XAudioManager::volume(int deviceid, const std::string &audioname) {
    return getVolume(deviceid, audioname);
}
float XAudioManager::volume(const std::string &devicename, int id) {
    return getVolume(devicename, id);
}
float XAudioManager::volume(const std::string &devicename,
                            const std::string &audio) {
    return getVolume(devicename, audio);
}

const std::string &XAudioManager::audio_name(int audio_id) {
    return get_audio_name(audio_id);
}

const std::string &XAudioManager::audio_path(int audio_id) {
    return get_audio_path(audio_id);
}

// 设置音频音量
void XAudioManager::setVolume(int deviceid, int id, float v) {
    setAudioVolume(deviceid, id, v);
}
void XAudioManager::setVolume(int deviceid, const std::string &audioname,
                              float v) {
    setAudioVolume(deviceid, audioname, v);
}
void XAudioManager::setVolume(const std::string &devicename, int id, float v) {
    setAudioVolume(devicename, id, v);
}
void XAudioManager::setVolume(const std::string &devicename,
                              const std::string &audio, float v) {
    setAudioVolume(devicename, audio, v);
}
// 获取全局音量
float XAudioManager::globalVolume() { return getGlobalVolume(); };
// 设置全局音量
void XAudioManager::setGlobalVolume(float volume) {
    setGlobalAudioVolume(volume);
}
// 播放音频句柄
void XAudioManager::play(int device_index, int audio_id, bool loop) {
    playAudio(device_index, audio_id, loop);
}
void XAudioManager::play(const std::string &devicename,
                         const std::string &audioname, bool loop) {
    playAudio(devicename, audioname, loop);
}
void XAudioManager::play(int device_index, const std::string &audioname,
                         bool loop) {
    playAudio(device_index, audioname, loop);
}
void XAudioManager::play(const std::string &devicename, int audio_id,
                         bool loop) {
    playAudio(devicename, audio_id, loop);
}

// 暂停音频句柄
void XAudioManager::pause(int device_index, int audio_id) {
    pauseAudio(device_index, audio_id);
}
void XAudioManager::pause(const std::string &devicename,
                          const std::string &audioname) {
    pauseAudio(devicename, audioname);
}
void XAudioManager::pause(int device_index, const std::string &audioname) {
    pauseAudio(device_index, audioname);
}
void XAudioManager::pause(const std::string &devicename, int audio_id) {
    pauseAudio(devicename, audio_id);
}

// 恢复音频句柄
void XAudioManager::resume(int device_index, int audio_id) {
    resumeAudio(device_index, audio_id);
}
void XAudioManager::resume(const std::string &devicename,
                           const std::string &audioname) {
    resumeAudio(devicename, audioname);
}
void XAudioManager::resume(int device_index, const std::string &audioname) {
    resumeAudio(device_index, audioname);
}
void XAudioManager::resume(const std::string &devicename, int audio_id) {
    resumeAudio(devicename, audio_id);
}

// 停止音频句柄
void XAudioManager::stop(int device_index, int audio_id) {
    stopAudio(device_index, audio_id);
};
void XAudioManager::stop(const std::string &devicename,
                         const std::string &audioname) {
    stopAudio(devicename, audioname);
};
void XAudioManager::stop(int device_index, const std::string &audioname) {
    stopAudio(device_index, audioname);
};
void XAudioManager::stop(const std::string &devicename, int audio_id) {
    stopAudio(devicename, audio_id);
};

// 设置播放时间
void XAudioManager::set_audio_time(int device_id, int id, int64_t time) {
    set_audio_current_pos(device_id, id, time);
};
void XAudioManager::set_audio_time(int device_id, const std::string &auido,
                                   int64_t time) {
    set_audio_current_pos(device_id, auido, time);
};
void XAudioManager::set_audio_time(const std::string &device, int id,
                                   int64_t time) {
    set_audio_current_pos(device, id, time);
};
void XAudioManager::set_audio_time(const std::string &device,
                                   const std::string &auido, int64_t time) {
    set_audio_current_pos(device, auido, time);
};

// 获取设备播放器状态
bool XAudioManager::is_pause(int device_id) { return isDevicePause(device_id); }
bool XAudioManager::is_pause(const std::string &devicename) {
    return isDevicePause(devicename);
}

// 暂停播放器
void XAudioManager::pause(int device_id) { pauseDevice(device_id); };
void XAudioManager::pause(const std::string &devicename) {
    pauseDevice(devicename);
}

// 恢复播放器
void XAudioManager::resume(int device_id) { resumeDevice(device_id); };
void XAudioManager::resume(const std::string &devicename) {
    resumeDevice(devicename);
}

// 终止播放器
void XAudioManager::stop(int device_id) { stopDevice(device_id); };
void XAudioManager::stop(const std::string &devicename) {
    stopDevice(devicename);
};
