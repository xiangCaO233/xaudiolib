#ifndef API_X_AUDIO_ENGIN_H
#define API_X_AUDIO_ENGIN_H

#if defined(__APPLE__)
// 苹果coreaudio库
#include <AudioToolbox/AudioToolbox.h>
#include <CoreAudio/CoreAudio.h>
#elif defined(__linux__)
#elif defined(_WIN32)
#elif defined(_WIN64)
#elif defined(__ANDROID__)
#else
#endif

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}

class XAudioEngin {
   public:
};

#endif  // API_X_AUDIO_ENGIN_H
