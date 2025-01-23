#include "indevice.h"

#include <iostream>
#include <utility>

XInputDevice::XInputDevice(int id, std::string name)
    : sdl_id(id), device_name(std::move(name)) {}

XInputDevice::~XInputDevice() {
  std::cout << "销毁输入设备:[" + device_name + "]" << std::endl;
}
