#include <iostream>
#include <string>

#include "api/XAudioManager.h"
#include "logger/logger.h"

int main(int argc, char *argv[]) {
    std::cout << "xaudios!" << std::endl;

    auto manager = XAudioManager::newmanager();
    std::string audio_path = "../resources/Tensions - スキャンダル.mp3";
    std::string audio_path2 = "aaaa.mp3";
    manager->load(audio_path);
    manager->load(audio_path2);
    manager->unload(audio_path);

    LOG_TRACE(manager->audio_path(0));
    LOG_TRACE(manager->audio_name(0));

    return 0;
}
