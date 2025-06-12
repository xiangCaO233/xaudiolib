#ifndef X_AUDIO_INDEVICE
#define X_AUDIO_INDEVICE

#include <string>

class XInputDevice {
 public:
  // 设备名称
  std::string device_name;
  // 设备sdlid
  int sdl_id;

  // 构造XInputDevice
  XInputDevice(int id, std::string &name);
  // 析构XInputDevice
  virtual ~XInputDevice();
};

#endif  // X_AUDIO_INDEVICE
