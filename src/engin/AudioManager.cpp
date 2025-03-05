#include "AudioManager.h"

#include <spdlog/common.h>

#include <string>

#include "log/colorful-log.h"

XAudioManager::XAudioManager() {
  XINFO("XAudioManager初始化");
  if (engin) return;
  engin = XAudioEngin::init();
  if (engin) XINFO("初始化引擎成功");
}

XAudioManager::~XAudioManager() { XTRACE("销毁音频管理器"); }

std::shared_ptr<XAudioManager> XAudioManager::newmanager() {
  XLogger::init();
  return std::make_shared<XAudioManager>();
}

// 启用日志
void XAudioManager::enableLoggin() const { XLogger::enable(); }
// 禁用日志
void XAudioManager::disableLoggin() const { XLogger::disable(); }
// 0-trace,1-debug,2-info,3-warning,4-error,5-critical
void XAudioManager::setLogginLevel(int level) const {
  XLogger::setlevel(spdlog::level::level_enum(level));
}
int XAudioManager::loadaudio(const std::string &audio,
                             std::string &loaded_audio_name) {
  return engin->load(audio, loaded_audio_name);
}
void XAudioManager::unloadaudio(const std::string &audio) {
  engin->unload(audio);
}

void XAudioManager::unloadaudio(int id) { engin->unload(id); }

// 获取音频名
const std::string &XAudioManager::get_audio_name(int id) {
  return engin->audio_name(id);
}
// 获取音频路径
const std::string &XAudioManager ::get_audio_path(int id) {
  return engin->audio_path(id);
}
// 设置音频当前播放到的位置
void XAudioManager::set_audio_current_pos(int device_id, int id, int64_t time) {
  engin->pos(device_id, id, time);
};
void XAudioManager::set_audio_current_pos(int device_id,
                                          const std::string &auido,
                                          int64_t time) {
  int audioid;
  if (!engin->is_audio_loaded(auido, audioid)) {
    return;
  }
  engin->pos(device_id, audioid, time);
};
void XAudioManager::set_audio_current_pos(const std::string &device, int id,
                                          int64_t time) {
  int deviceid;
  if (!engin->is_outdevice_exist(device, deviceid)) {
    return;
  }
  engin->pos(deviceid, id, time);
};
void XAudioManager::set_audio_current_pos(const std::string &device,
                                          const std::string &audio,
                                          int64_t time) {
  int deviceid;
  if (!engin->is_outdevice_exist(device, deviceid)) {
    return;
  }
  set_audio_current_pos(deviceid, audio, time);
};

// 获取音频音量
float XAudioManager::getVolume(int deviceid, int id) {
  return engin->volume(deviceid, id);
}
float XAudioManager::getVolume(int deviceid, const std::string &audioname) {
  int audioid;
  if (!engin->is_audio_loaded(audioname, audioid)) {
    return -1.0f;
  }
  return engin->volume(deviceid, audioid);
}
float XAudioManager::getVolume(const std::string &devicename, int id) {
  int deviceid;
  if (!engin->is_outdevice_exist(devicename, deviceid)) {
    return -1.0f;
  }
  return engin->volume(deviceid, id);
}
float XAudioManager::getVolume(const std::string &devicename,
                               const std::string &audio) {
  int deviceid;
  if (!engin->is_outdevice_exist(devicename, deviceid)) {
    return -1.0f;
  }
  return getVolume(deviceid, audio);
}
// 设置音频音量
void XAudioManager::setAudioVolume(int deviceid, int id, float v) {
  engin->setVolume(deviceid, id, v);
}
void XAudioManager::setAudioVolume(int deviceid, const std::string &audioname,
                                   float v) {
  int audioid;
  if (!engin->is_audio_loaded(audioname, audioid)) {
    return;
  }
  engin->setVolume(deviceid, audioid, v);
}
void XAudioManager::setAudioVolume(const std::string &devicename, int id,
                                   float v) {
  int deviceid;
  if (!engin->is_outdevice_exist(devicename, deviceid)) {
    return;
  }
  engin->setVolume(deviceid, id, v);
}
void XAudioManager::setAudioVolume(const std::string &devicename,
                                   const std::string &audio, float v) {
  int deviceid;
  if (!engin->is_outdevice_exist(devicename, deviceid)) {
    return;
  }
  setAudioVolume(deviceid, audio, v);
}
// 设置全局音量
float XAudioManager::getGlobalVolume() const { return engin->gVolume; }
void XAudioManager::setGlobalAudioVolume(float volume) {
  engin->setGlobalVolume(volume);
}
// 全局播放速度(变调)
float XAudioManager::getDevicePlaySpeed(int device_index) const {
  if (!engin->outdevices[device_index]) {
    XERROR("[" + std::to_string(device_index) + "]设备id不存在");
    return -1;
  }
  return engin->outdevices[device_index]->player->global_speed;
}
void XAudioManager::setDevicePlaySpeed(int device_index, float speed) {
  if (!engin->outdevices[device_index]) {
    XERROR("[" + std::to_string(device_index) + "]设备id不存在");
    return;
  }
  engin->outdevices[device_index]->player->ratio(speed);
}

// 播放和暂停音频
void XAudioManager::playAudio(int device_index, int id, bool isloop) {
  engin->play(device_index, id, isloop);
}
void XAudioManager::playAudio(int device_index, const std::string &audioname,
                              bool isloop) {
  int audioid;
  if (!engin->is_audio_loaded(audioname, audioid)) {
    return;
  }
  engin->play(device_index, audioid, isloop);
}
void XAudioManager::playAudio(const std::string &device, int id, bool isloop) {
  int deviceid;
  if (!engin->is_outdevice_exist(device, deviceid)) {
    return;
  }
  engin->play(deviceid, id, isloop);
}
void XAudioManager::playAudio(const std::string &device,
                              const std::string &audioname, bool isloop) {
  int deviceid;
  if (!engin->is_outdevice_exist(device, deviceid)) {
    return;
  }
  playAudio(deviceid, audioname, isloop);
}

void XAudioManager::pauseAudio(int device_index, int id) {
  engin->pause(device_index, id);
}
void XAudioManager::pauseAudio(int device_index, const std::string &audioname) {
  int audioid;
  if (!engin->is_audio_loaded(audioname, audioid)) {
    return;
  }
  engin->pause(device_index, audioid);
}
void XAudioManager::pauseAudio(const std::string &device, int id) {
  int deviceid;
  if (!engin->is_outdevice_exist(device, deviceid)) {
    return;
  }
  engin->pause(deviceid, id);
}
void XAudioManager::pauseAudio(const std::string &device,
                               const std::string &audioname) {
  int deviceid;
  if (!engin->is_outdevice_exist(device, deviceid)) {
    return;
  }
  pauseAudio(deviceid, audioname);
}
void XAudioManager::resumeAudio(int device_index, int id) {
  engin->resume(device_index, id);
}
void XAudioManager::resumeAudio(int device_index,
                                const std::string &audioname) {
  int audioid;
  if (!engin->is_audio_loaded(audioname, audioid)) {
    return;
  }
  engin->resume(device_index, audioid);
}
void XAudioManager::resumeAudio(const std::string &device, int id) {
  int deviceid;
  if (!engin->is_outdevice_exist(device, deviceid)) {
    return;
  }
  engin->resume(deviceid, id);
}
void XAudioManager::resumeAudio(const std::string &device,
                                const std::string &audioname) {
  int deviceid;
  if (!engin->is_outdevice_exist(device, deviceid)) {
    return;
  }
  resumeAudio(deviceid, audioname);
}
void XAudioManager::stopAudio(int device_index, int id) {
  engin->stop(device_index, id);
}
void XAudioManager::stopAudio(int device_index, const std::string &audioname) {
  int audioid;
  if (!engin->is_audio_loaded(audioname, audioid)) {
    return;
  }
  engin->stop(device_index, audioid);
}
void XAudioManager::stopAudio(const std::string &device, int id) {
  int deviceid;
  if (!engin->is_outdevice_exist(device, deviceid)) {
    return;
  }
  engin->stop(deviceid, id);
}
void XAudioManager::stopAudio(const std::string &device,
                              const std::string &audioname) {
  int deviceid;
  if (!engin->is_outdevice_exist(device, deviceid)) {
    return;
  }
  stopAudio(deviceid, audioname);
}

// 设备是否暂停
bool XAudioManager::isDevicePause(int device_id) {
  return engin->is_pause(device_id);
}
bool XAudioManager::isDevicePause(const std::string &devicename) {
  int deviceid;
  if (!engin->is_outdevice_exist(devicename, deviceid)) {
    return false;
  }
  return engin->is_pause(deviceid);
}

void XAudioManager::pauseDevice(int device_id) {
  engin->pause_device(device_id);
}
void XAudioManager::pauseDevice(const std::string &devicename) {
  int deviceid;
  if (!engin->is_outdevice_exist(devicename, deviceid)) {
    return;
  }
  engin->pause_device(deviceid);
}

void XAudioManager::resumeDevice(int device_id) {
  engin->resume_device(device_id);
}
void XAudioManager::resumeDevice(const std::string &devicename) {
  int deviceid;
  if (!engin->is_outdevice_exist(devicename, deviceid)) {
    return;
  }
  engin->resume_device(deviceid);
}

void XAudioManager::stopDevice(int device_id) { engin->stop_player(device_id); }
void XAudioManager::stopDevice(const std::string &devicename) {
  int deviceid;
  if (!engin->is_outdevice_exist(devicename, deviceid)) {
    return;
  }
  engin->stop_player(deviceid);
}
std::unordered_map<std::string, int> *XAudioManager::get_handles() {
  return &engin->handles;
}
std::unordered_map<int, std::shared_ptr<XSound>> *XAudioManager::get_audios() {
  return &engin->audios;
}
std::unordered_map<int, std::shared_ptr<XOutputDevice>> *
XAudioManager::get_outdevices() {
  return &engin->outdevices;
}
std::unordered_map<std::string, int> *XAudioManager::get_outdevice_indicies() {
  return &engin->outdevice_indicies;
}
std::unordered_map<int, std::shared_ptr<XInputDevice>> *
XAudioManager::get_inputdevices() {
  return &engin->inputdevices;
}
std::unordered_map<std::string, int> *XAudioManager::get_inputdevice_indices() {
  return &engin->inputdevice_indicies;
}
