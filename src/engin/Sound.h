#ifndef X_AUDIO_SOUND_H
#define X_AUDIO_SOUND_H

#include <string>
#include <vector>

class XAudioManager;
class XAudioEngin;
class XAuidoMixer;

struct AVFormatContext;

class XSound {
  // 句柄(id)
  int handle;
  // pcm声音数据
  std::vector<float> pcm_data;
  // 缓存数据
  std::vector<float> temp_data;
  // 音频文件名
  const std::string name;
  // 音频路径
  const std::string path;
  // 音频格式
  AVFormatContext *audio_format;

  friend XAudioManager;
  friend XAudioEngin;
  friend XAuidoMixer;
  friend XAudioManager;
  friend XAudioEngin;
  friend XAuidoMixer;

 public:
  // 构造XSound
  XSound(int h, std::string n, std::string p, AVFormatContext *f);
  // 析构XSound
  virtual ~XSound();

  // 调整位置(按帧)
  void locateframe(size_t frameindex) const;
  // 调整位置(按采样)
  void locatesample(size_t sampleindex) const;
  // 调整位置(按时间)
  void locatetime(size_t milliseconds) const;
};

#endif  // X_AUDIO_SOUND_H
