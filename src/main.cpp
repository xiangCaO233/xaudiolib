#include <glog/logging.h>

#include <iostream>
#include <string>
#include <vector>

#include "api/XAudioManager.h"
#include "log/colorful-log.h"

int main(int argc, char* argv[]) {
  GLogger::init(argv[0]);

  LOGINFO << argv[0] << "启动";
  LOG(WARNING) << "warn";
  LOG(ERROR) << "error";
  LOG(FATAL) << "error";
  auto manager = XAudioManager::newmanager();

  std::vector<std::string> audio_paths;
#ifdef _WIN32
#endif  //_WIN32

#ifdef __linux__
  audio_paths.push_back("../resources/Tensions - スキャンダル.mp3");
  audio_paths.push_back(
      "../resources/HoneyWorks,かぴ - ヒロインとアイドル (feat. かぴ).flac");
  audio_paths.push_back("../resources/combobreak.wav");
  audio_paths.push_back("../resources/New story.mp3");
  audio_paths.push_back(
      "../resources/結城アイラ - どんな星空よりも、どんな思い出よりも.flac");
  audio_paths.push_back("../resources/ゆよゆっぺ,meola,巡音ルカ - Palette.mp3");
  audio_paths.push_back("../resources/鬼頭明里,Neko Hacker - 刹那の誓い.flac");

  audio_paths.push_back("../resources/Tensions - スキャンダル.mp3");
  audio_paths.push_back(
      "../resources/HoneyWorks,かぴ - ヒロインとアイドル (feat. かぴ).flac");
  audio_paths.push_back("../resources/combobreak.wav");
  audio_paths.push_back("../resources/New story.mp3");
  audio_paths.push_back(
      "../resources/結城アイラ - どんな星空よりも、どんな思い出よりも.flac");
  audio_paths.push_back("../resources/ゆよゆっぺ,meola,巡音ルカ - Palette.mp3");
  audio_paths.push_back("../resources/音莉飴 - りんごと君.ncm");
  audio_paths.push_back("../resources/花鋏キョウ - Behavior.ncm");
  audio_paths.push_back("../resources/233.mp3");

#endif  //__linux__
#ifdef __APPLE__
  audio_paths.push_back("../resources/Tensions - スキャンダル.mp3");
  audio_paths.push_back(
      "../resources/HoneyWorks,かぴ - ヒロインとアイドル (feat. かぴ).flac");
  audio_paths.push_back("../resources/combobreak.wav");
  audio_paths.push_back("../resources/New story.mp3");
  audio_paths.push_back(
      "../resources/結城アイラ - どんな星空よりも、どんな思い出よりも.flac");
  audio_paths.push_back("../resources/ゆよゆっぺ,meola,巡音ルカ - Palette.mp3");
  audio_paths.push_back("../resources/鬼頭明里,Neko Hacker - 刹那の誓い.flac");

  audio_paths.push_back("../resources/Tensions - スキャンダル.mp3");
  audio_paths.push_back(
      "../resources/HoneyWorks,かぴ - ヒロインとアイドル (feat. かぴ).flac");
  audio_paths.push_back("../resources/combobreak.wav");
  audio_paths.push_back("../resources/New story.mp3");
  audio_paths.push_back(
      "../resources/結城アイラ - どんな星空よりも、どんな思い出よりも.flac");
  audio_paths.push_back("../resources/ゆよゆっぺ,meola,巡音ルカ - Palette.mp3");
  audio_paths.push_back("../resources/音莉飴 - りんごと君.ncm");
  audio_paths.push_back("../resources/鬼頭明里,Neko Hacker - 刹那の誓い.flac");
  audio_paths.push_back("../resources/花鋏キョウ - Behavior.ncm");
  audio_paths.push_back("../resources/233.mp3");
#endif  //__APPLE__

  // 载入音频
  for (auto& var : audio_paths) {
    manager->load(var);
  }

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
          std::cout << "播放[" + std::to_string(deviceid) + "]设备上的[" +
                           std::to_string(audioid) + "]音频"
                    << std::endl;
          manager->play(deviceid, audioid, true);
          break;
        }
          // enum Operate { STOPAUDIO, STOPDEVICE, SETPOS, QUIT,
          // PAUSE, RESUME };
        case SETPOS: {
          std::cout << "设置[" + std::to_string(deviceid) + "]设备上的[" +
                           std::to_string(audioid) + "]音频到60000ms位置"
                    << std::endl;
          manager->set_audio_time(deviceid, audioid, 60000);
          break;
        }
        case STOPAUDIO: {
          std::cout << "移除[" + std::to_string(deviceid) + "]设备上的[" +
                           std::to_string(audioid) + "]音频"
                    << std::endl;
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
          std::cout << "暂停[" + std::to_string(deviceid) + "]设备上的[" +
                           std::to_string(audioid) + "]音频"
                    << std::endl;
          manager->pause(deviceid, audioid);
          break;
        }
        case RESUME: {
          std::cout << "恢复[" + std::to_string(deviceid) + "]设备上的[" +
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
  for (int i = 0; i < 10; i++) {
    manager->unload(i);
  }

  GLogger::destroy();
  return 0;
}
