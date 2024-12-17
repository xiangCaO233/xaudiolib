#include <sys/cdefs.h>

#include <iostream>
#include <string>

#include "engin/AudioManager.h"

int main(int argc, char* argv[]) {
    auto manager = XAudioManager::newmanager();

    std::string audio_path = "../resources/Tensions - スキャンダル.mp3";
    std::string audio_path2 =
        "../resources/HoneyWorks,かぴ - ヒロインとアイドル (feat. かぴ).flac";
    std::string audio_path3 = "../resources/combobreak.wav";
    std::string audio_path4 = "../resources/New story.mp3";

    // 载入音频
    manager->loadaudio(audio_path);
    manager->loadaudio(audio_path2);
    manager->loadaudio(audio_path3);
    manager->loadaudio(audio_path2);
    manager->loadaudio(audio_path4);
    manager->loadaudio(audio_path3);

    std::string devicename = "External Headphones";

    bool quit{false};
    while (true) {
        std::cout << "输入句柄以播放" << std::endl;
        std::cout << "输入q以退出" << std::endl;
        switch (std::cin.get()) {
            case 'q': {
                quit = true;
                break;
            }
            case 's': {
                manager->set_audio_current_pos(1, 120000);
                break;
            }

            case 'p': {
                if (manager->isDevicePause(devicename)) {
                    manager->resumeDevice(devicename);
                } else {
                    manager->pauseDevice(devicename);
                }
                break;
            }
            case '0': {
                manager->playAudio(devicename, 0, false);
                break;
            }
            case '1': {
                manager->playAudio(devicename, 1, false);
                break;
            }
            case '2': {
                manager->playAudio(devicename, 2, false);
                break;
            }
            case '3': {
                manager->playAudio(devicename, 3, false);
                break;
            }
            case '4': {
                manager->playAudio(devicename, 4, false);
                break;
            }
            case '5': {
                manager->playAudio(devicename, 5, false);
                break;
            }
            case '6': {
                manager->playAudio(devicename, 6, false);
                break;
            }
        }
        if (quit) {
            break;
        }
    }

    // 卸载音频
    manager->unloadaudio(0);
    manager->unloadaudio(1);
    manager->unloadaudio(2);
    manager->unloadaudio(3);

    return 0;
}
