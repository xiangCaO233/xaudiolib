#include "outdevice.h"

XOutputDevice::XOutputDevice(int id, std::string name)
    : sdl_id(id), device_name(name) {}

XOutputDevice::~XOutputDevice() {}
