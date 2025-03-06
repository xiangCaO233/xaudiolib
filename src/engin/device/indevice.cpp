#include "indevice.h"

#include <utility>

#include "log/colorful-log.h"

XInputDevice::XInputDevice(int id, std::string &name)
    : sdl_id(id), device_name(std::move(name)) {}

XInputDevice::~XInputDevice() { XTRACE("销毁输入设备:[" + device_name + "]"); }
