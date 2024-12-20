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
void XSound::locateframe(size_t frameindex) {}
// 调整位置(按采样)
void XSound::locatesample(size_t sampleindex) {}
// 调整位置(按时间)
void XSound::locatetime(size_t milliseconds) {}
