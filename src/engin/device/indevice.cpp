#include "indevice.h"
#include "logger/logger.h"

#include <utility>

XInputDevice::XInputDevice(int id, std::string name)
    : sdl_id(id), device_name(std::move(name)) {}

XInputDevice::~XInputDevice() {LOG_TRACE("销毁输入设备:["+device_name+"]");}
