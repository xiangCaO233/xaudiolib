#include "Sound.h"

#include <iostream>
#include <utility>

XSound::XSound(int h, std::string n, std::string p,
               std::shared_ptr<AVFormatContext> f)
    : handle(h), name(std::move(n)), path(std::move(p)),
      audio_format(std::move(f)) {
  std::cout << "XSound初始化" << std::endl;
}
// 调整位置(按帧)
void XSound::locateframe(size_t frameindex) {}
// 调整位置(按采样)
void XSound::locatesample(size_t sampleindex) {}
// 调整位置(按时间)
void XSound::locatetime(size_t milliseconds) {}
