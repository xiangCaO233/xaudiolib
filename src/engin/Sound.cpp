#include "Sound.h"

#include "logger/logger.h"

std::string XSound::unknown = "unknown";
std::string XSound::unknown_path = "unknown path";
XSound::XSound(int h, std::string n, std::string p,
               std::shared_ptr<AVFormatContext> f, float s, float vm)
    : handle(h),
      playpos(0),
      pauseflag(false),
      name(n),
      path(p),
      audio_format(f),
      speed(s),
      volume(vm) {
    LOG_TRACE("XSound初始化");
};
// 调整位置(按帧)
void XSound::locateframe(size_t frameindex){};
// 调整位置(按采样)
void XSound::locatesample(size_t sampleindex){};
// 调整位置(按时间)
void XSound::locatetime(size_t milliseconds){};
