#include "outdevice.h"

#include <memory>
#include <utility>

#include "../sdl/xplayer.h"
#include "log/colorful-log.h"

XOutputDevice::XOutputDevice(int id, std::string name)
    : sdl_id(id), device_name(std::move(name)) {}

XOutputDevice::~XOutputDevice() {
  XTRACE("销毁输出设备:[" + device_name + "]");
}

// 创建一个位于该设备的播放器
bool XOutputDevice::creat_player() {
  player = std::make_shared<XPlayer>();
  player->set_device_index(sdl_id);
  return player != nullptr;
};
