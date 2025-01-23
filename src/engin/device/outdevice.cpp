#include "outdevice.h"

#include <iostream>
#include <memory>
#include <utility>

#include "../sdl/xplayer.h"

XOutputDevice::XOutputDevice(int id, std::string name)
    : sdl_id(id), device_name(std::move(name)) {}

XOutputDevice::~XOutputDevice() {
  std::cout << "销毁输出设备:[" + device_name + "]" << std::endl;
}

// 创建一个位于该设备的播放器
bool XOutputDevice::creat_player() {
  player = std::make_shared<XPlayer>();
  player->set_device_index(sdl_id);
  return player != nullptr;
};
