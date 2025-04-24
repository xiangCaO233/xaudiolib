#include "outdevice.h"

#include <memory>

#include "../sdl/xplayer.h"
#include "log/colorful-log.h"

XOutputDevice::XOutputDevice(int id, std::string &name)
    : sdl_id(id), device_name(name) {}

XOutputDevice::~XOutputDevice() {}

// 创建一个位于该设备的播放器
bool XOutputDevice::creat_player() {
  XTRACE("正在创建输出设备:[" + device_name + "]上的播放器");
  player = std::make_shared<XPlayer>();
  player->set_device_index(sdl_id);
  player->set_player_volume(device_volume);
  XINFO("成功创建输出设备:[" + device_name + "]上的播放器");
  return player != nullptr;
};
// 设置设备音量
void XOutputDevice::set_device_volume(float volume) {
  if (volume >= 0.0f && volume <= 1.0f) {
    device_volume = volume;
    player->set_player_volume(volume);
  }
}
