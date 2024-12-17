#include "outdevice.h"

#include <memory>

#include "../sdl/xplayer.h"

XOutputDevice::XOutputDevice(int id, std::string name)
    : sdl_id(id), device_name(name) {}

XOutputDevice::~XOutputDevice() {}

// 创建一个位于该设备的播放器
bool XOutputDevice::creat_player() {
    player = std::make_shared<XPlayer>();
    player->set_device_index(sdl_id);
    return player != nullptr;
};
