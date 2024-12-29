#include "indevice.h"

#include <utility>

#include "logger/logger.h"

XInputDevice::XInputDevice(int id, std::string name)
    : sdl_id(id), device_name(std::move(name)) {}

XInputDevice::~XInputDevice() {
  LOG_TRACE("销毁输入设备:[" + device_name + "]");
}
