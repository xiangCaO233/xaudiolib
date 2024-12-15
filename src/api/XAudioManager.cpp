#include "XAudioManager.h"

void XAudioManager::load(std::string &audio_path) { loadaudio(audio_path); }

void XAudioManager::unload(std::string &audio_path) { unloadaudio(audio_path); }

void XAudioManager::unload(int audio_id) { unloadaudio(audio_id); }
