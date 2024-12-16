#include "AudioManager.h"

#include "logger/logger.h"

XAudioManager::XAudioManager() {
    XLogger::init();
    engin = XAudioEngin::init();
    if (engin) LOG_INFO("初始化引擎成功");
}

XAudioManager::~XAudioManager() {}

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
float XAudioManager::getGlobalVolume() { return engin->globalVolume; };
void XAudioManager::setGlobalAudioVolume(float volume){};

// 播放和暂停音频
void XAudioManager::playAudio(int id, bool isloop){};
void XAudioManager::pauseAudio(int id){};
