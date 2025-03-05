#ifndef X_AUDIO_OUTDEVICE_H
#define X_AUDIO_OUTDEVICE_H

#include <memory>
#include <string>

class XAudioEngin;
class XAudioManager;
class XPlayer;

class XOutputDevice {
  // 设备sdlid(仅索引)
  int sdl_id;
  // 播放器
  std::shared_ptr<XPlayer> player;

  friend XAudioEngin;
  friend XAudioManager;

 public:
  // 设备名称
  std::string device_name;
  // 构造XOutputDevice
  XOutputDevice(int id, std::string name);
  // 析构XOutputDevice
  virtual ~XOutputDevice();

  // 创建一个位于该设备的播放器
  bool creat_player();
};

#endif  // X_AUDIO_OUTDEVICE_H
