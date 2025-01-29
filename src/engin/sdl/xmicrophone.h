#ifndef X_AUDIO_MICROPHONE
#define X_AUDIO_MICROPHONE

#include <thread>

class XMicrophone {
  // 录制线程
  std::thread recording_thread;
  // TODO(xiang 2024-12-24): 实现麦克风

 public:
  // 构造XMicrophone
  XMicrophone();
  // 析构XMicrophone
  virtual ~XMicrophone();
};

#endif  // X_AUDIO_MICROPHONE
