#include "AudioManager.h"

#include <string>

#include "logger/logger.h"

XAudioManager::XAudioManager() { XLogger::init(); }

XAudioManager::~XAudioManager() {}

XAudioManager *XAudioManager::newmanager() { return new XAudioManager(); }

void XAudioManager::loadaudio(std::string &audio) {
    // TODO(xiang 2024-12-15): 实现载入音频
    LOG_DEBUG("载入音频[" + audio + "]");
};
void XAudioManager::unloadaudio(std::string &audio) {
    // TODO(xiang 2024-12-15): 实现卸载音频
    LOG_DEBUG("卸载音频[" + audio + "]");
};
void XAudioManager::unloadaudio(int id) {
    // TODO(xiang 2024-12-15): 实现使用id卸载音频
    LOG_DEBUG("卸载音频[" + std::to_string(id) + "]");
};
