#ifndef X_AUDIO_SOUND_H
#define X_AUDIO_SOUND_H

#include <string>
#include <vector>

struct AVFormatContext;

class XSound {
 public:
  // 句柄(id)
  int handle;
  // pcm数据(分声道)
  std::vector<std::vector<float>> pcm;
  // 音频文件名
  const std::string name;
  // 音频路径
  const std::string path;
  // 音频格式
  AVFormatContext *audio_format;

  // 构造XSound
  XSound(int h, std::string n, std::string p, AVFormatContext *f);
  // 析构XSound
  virtual ~XSound();

  // 获取位置(按帧)
  size_t locateframe(size_t frameindex) const;
  // 获取位置(按采样)
  size_t locatesample(size_t sampleindex) const;
  // 获取位置(按时间)
  size_t locatetime(size_t milliseconds) const;
  // 获取音频数据大小
  size_t get_pcm_data_size() const;
};

#endif  // X_AUDIO_SOUND_H
