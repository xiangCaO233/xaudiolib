#include "Sound.h"

#include <utility>

#include "logger/logger.h"

XSound::XSound(int h, std::string n, std::string p,
               std::shared_ptr<AVFormatContext> f)
    : handle(h),
      name(std::move(n)),
      path(std::move(p)),
      audio_format(std::move(f)) {
  LOG_TRACE("XSound初始化");
}
// 调整位置(按帧)
void XSound::locateframe(size_t frameindex) {
  // TODO(xiang 2024-12-24): 实现按帧定位播放位置
}
// 调整位置(按采样)
void XSound::locatesample(size_t sampleindex) {
  // TODO(xiang 2024-12-24): 实现按采样定位播放位置
}
// 调整位置(按时间)
void XSound::locatetime(size_t milliseconds) {
  // TODO(xiang 2024-12-24): 实现按时间直接定位播放位置
}
