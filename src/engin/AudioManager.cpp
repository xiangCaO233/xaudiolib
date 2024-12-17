#include "AudioManager.h"

#include "logger/logger.h"

XAudioManager::XAudioManager() {
    XLogger::init();
    LOG_TRACE("XAudioManager初始化");
    engin = XAudioEngin::init();
    if (engin) LOG_INFO("初始化引擎成功");
}

XAudioManager::~XAudioManager() = default;

std::shared_ptr<XAudioManager> XAudioManager::newmanager() {
    return std::make_shared<XAudioManager>();
}

int XAudioManager::loadaudio(const std::string &audio) {
    return engin->load(audio);
};
void XAudioManager::unloadaudio(const std::string &audio) {
    engin->unload(audio);
};

void XAudioManager::unloadaudio(int id) { engin->unload(id); };

// 获取音频名
const std::string &XAudioManager::get_audio_name(int id) {
    return engin->name(id);
};
// 获取音频路径
const std::string &XAudioManager ::get_audio_path(int id) {
    return engin->path(id);
};
// 设置音频当前播放到的位置
void XAudioManager::set_audio_current_pos(const std::string &auido,
                                          int64_t time) {
    engin->pos(auido, time);
};
void XAudioManager::set_audio_current_pos(int id, int64_t time) {
    engin->pos(id, time);
};

// 获取音量
float XAudioManager::getVolume(const std::string &audio) {
    return engin->volume(audio);
};
float XAudioManager::getVolume(int id) { return engin->volume(id); };
// 设置音量
void XAudioManager::setAudioVolume(const std::string &audio, float v) {
    engin->setVolume(audio, v);
};
void XAudioManager::setAudioVolume(int id, float v) {
    engin->setVolume(id, v);
};
// 设置全局音量
float XAudioManager::getGlobalVolume() { return engin->gVolume; };
void XAudioManager::setGlobalAudioVolume(float volume){};

// 播放和暂停音频
void XAudioManager::playAudio(int device_index, int id, bool isloop) {
    engin->play(device_index, id, isloop);
};
void XAudioManager::playAudio(const std::string &device,
                              const std::string &audioname, bool isloop) {
    engin->play(device, audioname, isloop);
};
void XAudioManager::playAudio(int device_index, const std::string &audioname,
                              bool isloop) {
    engin->play(device_index, audioname, isloop);
};
void XAudioManager::playAudio(const std::string &device, int id, bool isloop) {
    engin->play(device, id, isloop);
};

void XAudioManager::pauseAudio(int device_index, int id) {
    engin->pause(device_index, id);
};
void XAudioManager::pauseAudio(const std::string &device,
                               const std::string &audioname) {
    engin->pause(device, audioname);
};
void XAudioManager::pauseAudio(int device_index, const std::string &audioname) {
    engin->pause(device_index, audioname);
};
void XAudioManager::pauseAudio(const std::string &device, int id) {
    engin->pause(device, id);
};

// 设备是否暂停
bool XAudioManager::isDevicePause(int device_id) {
    return engin->is_pause(device_id);
}
bool XAudioManager::isDevicePause(const std::string &devicename) {
    return engin->is_pause(devicename);
}

void XAudioManager::pauseDevice(int device_id) {
    engin->pause_device(device_id);
};
void XAudioManager::pauseDevice(const std::string &devicename) {
    engin->pause_device(devicename);
};

void XAudioManager::resumeDevice(int device_id) {
    engin->resume_device(device_id);
};
void XAudioManager::resumeDevice(const std::string &devicename) {
    engin->resume_device(devicename);
};

void XAudioManager::stopDevice(int device_id) {
    engin->stop_player(device_id);
};
void XAudioManager::stopDevice(const std::string &devicename) {
    engin->stop_player(devicename);
};
