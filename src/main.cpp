#include <iostream>
#include <string>

#include "api/XAudioManager.h"
#include "logger/logger.h"

int main(int argc, char *argv[]) {
    auto manager = XAudioManager::newmanager();
    std::string audio_path = "../resources/Tensions - スキャンダル.mp3";
    std::string audio_path2 =
        "../resources/HoneyWorks,かぴ - ヒロインとアイドル (feat. かぴ).flac";
    std::string audio_path3 = "../resources/combobreak.wav";
    std::string audio_path4 = "../resources/New story.mp3";
    // 载入音频
    manager->load(audio_path);
    manager->load(audio_path2);
    manager->load(audio_path3);
    manager->load(audio_path4);

    manager->unload(audio_path);

    return 0;
}
