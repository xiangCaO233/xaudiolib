#ifndef X_AUDIO_ORBIT_H
#define X_AUDIO_ORBIT_H

#include <memory>
#include <vector>

class XSound;

class PlayposCallBack {
 public:
  virtual ~PlayposCallBack() = default;
  virtual void playpos_call(double playpos) = 0;
};

class XAudioOrbit {
 public:
  std::shared_ptr<XSound> sound;
  // 回调列表
  std::vector<std::shared_ptr<PlayposCallBack>> playpos_callbacks;
  // 播放指针
  double playpos{0.0};
  // 轨道音量
  float volume{1.0f};
  // 轨道指针播放速度
  float speed{0.5f};
  // 轨道暂停标识
  bool paused{false};
  // 轨道循环标识
  bool loop{false};

  // 构造XAudioOrbit
  explicit XAudioOrbit(std::shared_ptr<XSound> audio = nullptr);
  // 析构XAudioOrbit
  ~XAudioOrbit();

  // 添加播放位置回调
  void add_playpos_callback(std::shared_ptr<PlayposCallBack> callback);
  // 移除回调
  void remove_playpos_callback(std::shared_ptr<PlayposCallBack> callback);
};

#endif  // X_AUDIO_ORBIT_H
