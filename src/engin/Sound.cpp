#include "Sound.h"

#include <iostream>
#include <utility>

extern "C"{
	#include <libavutil/mem.h>
}

XSound::XSound(int h, std::string n, std::string p,
	AVFormatContext *f)
	: handle(h),
	name(std::move(n)),
	path(std::move(p)),
	audio_format(f) {
	std::cout << "XSound初始化" << std::endl;
}
XSound::~XSound(){
	av_free(audio_format);
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
