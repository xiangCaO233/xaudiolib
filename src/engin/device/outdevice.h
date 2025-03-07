#ifndef X_AUDIO_OUTDEVICE_H
#define X_AUDIO_OUTDEVICE_H

#include <memory>
#include <string>

class XPlayer;

class XOutputDevice {
 public:
  float device_volume{1.0f};
  // 设备sdlid(仅索引)
  int sdl_id;
  // 设备名称
  std::string device_name;
  // 播放器
  std::shared_ptr<XPlayer> player;

  // 构造XOutputDevice
  XOutputDevice(int id, std::string &name);
  // 析构XOutputDevice
  virtual ~XOutputDevice();

  // 创建一个位于该设备的播放器
  bool creat_player();

  // 设置设备音量
  void set_device_volume(float volume);
};

#endif  // X_AUDIO_OUTDEVICE_H
