#include <iostream>
#include <string>

#include "api/XAudioManager.h"

int main(int argc, char* argv[]) {
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
    manager->load(audio_path2);
    manager->load(audio_path4);
    manager->load(audio_path3);

    std::string devicename = "External Headphones";

    bool quit{false};
    while (true) {
        std::cout << "输入句柄以播放" << std::endl;
        std::cout << "输入q以退出" << std::endl;
        switch (std::cin.get()) {
            case 'm': {
            }
            case 'q': {
                quit = true;
                break;
            }
            case 'k': {
                manager->stop(devicename);
                break;
            }
            case 's': {
                manager->set_audio_time(1, 120000);
                break;
            }
            case 'p': {
                if (manager->is_pause(devicename)) {
                    manager->resume(devicename);
                } else {
                    manager->pause(devicename);
                }
                break;
            }
            case '0': {
                manager->play(devicename, 0, false);
                break;
            }
            case '1': {
                manager->play(devicename, 1, false);
                break;
            }
            case '2': {
                manager->play(devicename, 2, false);
                break;
            }
            case '3': {
                manager->play(devicename, 3, false);
                break;
            }
            case '4': {
                manager->play(devicename, 4, false);
                break;
            }
            case '5': {
                manager->play(devicename, 5, false);
                break;
            }
            case '6': {
                manager->play(devicename, 6, false);
                break;
            }
        }
        if (quit) {
            break;
        }
    }

    // 卸载音频
    manager->unload(0);
    manager->unload(1);
    manager->unload(2);
    manager->unload(3);

    return 0;
}
