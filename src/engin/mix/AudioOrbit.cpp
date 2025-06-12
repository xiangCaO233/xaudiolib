#include "AudioOrbit.h"

#include "log/colorful-log.h"

// 构造XAudioOrbit
XAudioOrbit::XAudioOrbit(std::shared_ptr<XSound> audio) : sound(audio) {};
// 析构XAudioOrbit
XAudioOrbit::~XAudioOrbit() = default;

// 添加播放位置回调
void XAudioOrbit::add_playpos_callback(
    std::shared_ptr<PlayposCallBack> callback) {
  auto it = playpos_callbacks.find(callback);
  if (it != playpos_callbacks.end()) return;
  playpos_callbacks.emplace(callback);
};

// 移除回调
void XAudioOrbit::remove_playpos_callback(
    std::shared_ptr<PlayposCallBack> callback) {
  auto it = playpos_callbacks.find(callback);
  if (it != playpos_callbacks.end()) {
    playpos_callbacks.erase(it);
  } else {
    XWARN("不存在此回调");
  }
};
