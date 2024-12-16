#ifndef X_AUDIO_OUTDEVICE_H
#define X_AUDIO_OUTDEVICE_H

#include <string>

class XAudioEngin;

class XOutputDevice {
    // 设备名称
    std::string device_name;
    // 设备sdlid
    int sdl_id;

    friend XAudioEngin;

   public:
    // 构造XOutputDevice
    XOutputDevice(int id, std::string name);
    // 析构XOutputDevice
    virtual ~XOutputDevice();
};

#endif  // X_AUDIO_OUTDEVICE_H
