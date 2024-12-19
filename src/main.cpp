#include <engin/util/ncm.h>

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
    std::string audio_path5 =
        "../resources/結城アイラ - どんな星空よりも、どんな思い出よりも.flac";
    std::string audio_path6 =
        "../resources/ゆよゆっぺ,meola,巡音ルカ - Palette.mp3";
    std::string audio_path7 = "../resources/音莉飴 - りんごと君.ncm";
    std::string audio_path8 =
        "../resources/鬼頭明里,Neko Hacker - 刹那の誓い.flac";
    std::string audio_path9 = "../resources/花鋏キョウ - Behavior.ncm";

    // 载入音频
    manager->load(audio_path);
    manager->load(audio_path2);
    manager->load(audio_path3);
    manager->load(audio_path2);
    manager->load(audio_path2);
    manager->load(audio_path5);
    manager->load(audio_path6);
    manager->load(audio_path5);
    manager->load(audio_path7);
    manager->load(audio_path4);
    manager->load(audio_path3);
    manager->load(audio_path8);
    manager->load(audio_path6);
    manager->load(audio_path9);

    std::string devicename = "External Headphones";

    enum Operate { PLAY, STOPAUDIO, STOPDEVICE, SETPOS, QUIT, PAUSE, RESUME };
    bool quit{false};
    while (true) {
        std::cout << "输入指令[o](操作)[d](设备)[a](音频)" << std::endl;
        std::string line;
        std::getline(std::cin, line);
        if (line.size() != 3) {
            continue;
        } else {
            bool isdevicename = false;
            int deviceid = -1;
            bool isaudioname = false;
            int audioid = -1;
            Operate o;

            for (int i = 0; i < 3; i++) {
                switch (line.at(i)) {
                    case 'm': {
                        o = STOPAUDIO;
                        break;
                    }
                    case 'a': {
                        o = PLAY;
                        break;
                    }
                    case 'q': {
                        quit = true;
                        o = QUIT;
                        break;
                    }
                    case 'k': {
                        o = STOPDEVICE;
                        break;
                    }
                    case 'r': {
                        o = RESUME;
                        break;
                    }
                    case 's': {
                        o = SETPOS;
                        break;
                    }
                    case 'p': {
                        o = PAUSE;
                        break;
                    }
                    case '0': {
                        if (i == 1) {
                            deviceid = 0;
                        }
                        if (i == 2) {
                            audioid = 0;
                        }
                        break;
                    }
                    case '1': {
                        if (i == 1) {
                            deviceid = 1;
                        }
                        if (i == 2) {
                            audioid = 1;
                        }
                        break;
                    }
                    case '2': {
                        if (i == 1) {
                            deviceid = 2;
                        }
                        if (i == 2) {
                            audioid = 2;
                        }
                        break;
                    }
                    case '3': {
                        if (i == 1) {
                            deviceid = 3;
                        }
                        if (i == 2) {
                            audioid = 3;
                        }
                        break;
                    }
                    case '4': {
                        if (i == 1) {
                            deviceid = 4;
                        }
                        if (i == 2) {
                            audioid = 4;
                        }
                        break;
                    }
                    case '5': {
                        if (i == 1) {
                            deviceid = 5;
                        }
                        if (i == 2) {
                            audioid = 5;
                        }
                        break;
                    }
                    case '6': {
                        if (i == 1) {
                            deviceid = 6;
                        }
                        if (i == 2) {
                            audioid = 6;
                        }
                        break;
                    }
                    case '7': {
                        if (i == 1) {
                            deviceid = 7;
                        }
                        if (i == 2) {
                            audioid = 7;
                        }
                        break;
                    }
                    case '8': {
                        if (i == 1) {
                            deviceid = 8;
                        }
                        if (i == 2) {
                            audioid = 8;
                        }
                        break;
                    }
                    case '9': {
                        if (i == 1) {
                            deviceid = 9;
                        }
                        if (i == 2) {
                            audioid = 9;
                        }
                        break;
                    }
                }
            }

            std::cout << "操作: " << line << std::endl;
            switch (o) {
                case QUIT: {
                    std::cout << "退出" << std::endl;
                    break;
                }
                case PLAY: {
                    std::cout
                        << "播放[" + std::to_string(deviceid) + "]设备上的[" +
                               std::to_string(audioid) + "]音频"
                        << std::endl;
                    manager->play(deviceid, audioid, true);
                    break;
                }
                    // enum Operate { STOPAUDIO, STOPDEVICE, SETPOS, QUIT,
                    // PAUSE, RESUME };
                case SETPOS: {
                    std::cout
                        << "设置[" + std::to_string(deviceid) + "]设备上的[" +
                               std::to_string(audioid) + "]音频到60000ms位置"
                        << std::endl;
                    manager->set_audio_time(deviceid, audioid, 60000);
                    break;
                }
                case STOPAUDIO: {
                    manager->stop(deviceid, audioid);
                    break;
                }
                case STOPDEVICE: {
                    std::cout << "停止[" + std::to_string(deviceid) + "]设备"
                              << std::endl;
                    manager->stop(deviceid);
                    break;
                }
                case PAUSE: {
                    std::cout
                        << "暂停[" + std::to_string(deviceid) + "]设备上的[" +
                               std::to_string(audioid) + "]音频"
                        << std::endl;
                    manager->pause(deviceid, audioid);
                    break;
                }
                case RESUME: {
                    std::cout
                        << "恢复[" + std::to_string(deviceid) + "]设备上的[" +
                               std::to_string(audioid) + "]音频"
                        << std::endl;
                    manager->resume(deviceid, audioid);
                    break;
                }
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
    manager->unload(4);
    manager->unload(5);
    manager->unload(6);
    manager->unload(7);
    manager->unload(8);
    manager->unload(9);

    return 0;
}
