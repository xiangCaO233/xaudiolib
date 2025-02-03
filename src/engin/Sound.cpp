#include "Sound.h"

#include <utility>

#include "log/colorful-log.h"

extern "C" {
#include <libavutil/mem.h>
}

XSound::XSound(int h, std::string n, std::string p, AVFormatContext *f)
    : handle(h), name(std::move(n)), path(std::move(p)), audio_format(f) {
  XTRACE("XSound初始化");
}
XSound::~XSound() { av_free(audio_format); }
// 调整位置(按帧)
void XSound::locateframe(size_t frameindex) const {
  // TODO(xiang 2024-12-24): 实现按帧定位播放位置
}
// 调整位置(按采样)
void XSound::locatesample(size_t sampleindex) const {
  // TODO(xiang 2024-12-24): 实现按采样定位播放位置
}
// 调整位置(按时间)
void XSound::locatetime(size_t milliseconds) const {
  // TODO(xiang 2024-12-24): 实现按时间直接定位播放位置
}
