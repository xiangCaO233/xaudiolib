#ifndef X_AUDIO_ORBIT_H
#define X_AUDIO_ORBIT_H

#include <memory>

class XSound;

class XAudioOrbit {
 public:
  std::shared_ptr<XSound> sound;
  // 播放指针
  double playpos{0.0};
  // 轨道音量
  float volume{1.0f};
  // 轨道指针播放速度
  float speed{1.0f};
  // 轨道暂停标识
  bool paused{false};
  // 轨道循环标识
  bool loop{false};

  // 构造XAudioOrbit
  explicit XAudioOrbit(std::shared_ptr<XSound> audio = nullptr)
      : sound(audio){};
  // 析构XAudioOrbit
  ~XAudioOrbit() = default;
};

#endif  // X_AUDIO_ORBIT_H
