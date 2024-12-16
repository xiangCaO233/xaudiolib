#include "indevice.h"

XInputDevice::XInputDevice(int id, std::string name)
    : sdl_id(id), device_name(name) {}

XInputDevice::~XInputDevice() {}
