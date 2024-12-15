#include <iostream>
#include <string>
#if defined(__APPLE__)
#include <AudioToolbox/AudioToolbox.h>
#include <CoreAudio/CoreAudio.h>
#elif defined(__linux__)
#elif defined(_WIN32)
#elif defined(_WIN64)
#elif defined(__ANDROID__)
#else
#endif

#include "api/XAudioManager.h"
#include "logger/logger.h"

int main(int argc, char *argv[]) {
    std::cout << "xaudios!" << std::endl;
    auto manager = XAudioManager::newmanager();
    std::string audio_path = "testaudio.mp3";
    manager->load(audio_path);
    manager->unload(audio_path);

    return 0;
}
