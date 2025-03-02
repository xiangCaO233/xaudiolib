#include <filesystem>

#include "log/colorful-log.h"
#ifdef _WIN32
#include <windows.h>
#endif  //_WIN32

#include <iostream>
#include <string>

#include "engin/AudioManager.h"

int main(int argc, char* argv[]) {
#ifdef _WIN32
  SetConsoleOutputCP(CP_UTF8);
  SetConsoleCP(65001);
  std::setlocale(LC_ALL, ".UTF-8");
#endif  //_WIN32
  auto manager = XAudioManager::newmanager();

  // 载入音频
  auto respath = std::filesystem::path("../resources/");
  XINFO("respath:" + respath.string());
  for (const auto& entry : std::filesystem::directory_iterator(respath)) {
    manager->loadaudio(entry.path().string());
  }

  enum class Operate {
    PLAY,
    STOPAUDIO,
    STOPDEVICE,
    SETPOS,
    QUIT,
    PAUSE,
    SPEED,
    RESUME
  };
  bool quit{false};
  while (true) {
    std::cout << "输入指令[o](操作)[d](设备)[a](音频)" << std::endl;
    std::string line;
    std::getline(std::cin, line);
    if (line.size() != 3) {
      continue;
    } else {
      int deviceid = -1;
      int audioid = -1;
      Operate o;

      for (int i = 0; i < 3; i++) {
        switch (line.at(i)) {
          case 'm': {
            o = Operate::STOPAUDIO;
            break;
          }
          case 'a': {
            o = Operate::PLAY;
            break;
          }
          case 'c': {
            o = Operate::SPEED;
            break;
          }
          case 'q': {
            quit = true;
            o = Operate::QUIT;
            break;
          }
          case 'k': {
            o = Operate::STOPDEVICE;
            break;
          }
          case 'r': {
            o = Operate::RESUME;
            break;
          }
          case 's': {
            o = Operate::SETPOS;
            break;
          }
          case 'p': {
            o = Operate::PAUSE;
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
          default:
            break;
        }
      }

      std::cout << "操作: " << line << std::endl;
      switch (o) {
        case Operate::QUIT: {
          std::cout << "退出" << std::endl;
          break;
        }
        case Operate::PLAY: {
          std::cout << "播放[" + std::to_string(deviceid) + "]设备上的[" +
                           std::to_string(audioid) + "]音频"
                    << std::endl;
          manager->playAudio(deviceid, audioid, false);
          break;
        }
        case Operate::SPEED: {
          std::cout << "更改播放器[" + std::to_string(deviceid) + "]速度为[" +
                           std::to_string(audioid) + "x0.2]x"
                    << std::endl;
          manager->setDevicePlaySpeed(deviceid, audioid * 0.2);
          break;
        }
        case Operate::SETPOS: {
          std::cout << "设置[" + std::to_string(deviceid) + "]设备上的[" +
                           std::to_string(audioid) + "]音频到60000ms位置"
                    << std::endl;
          manager->set_audio_current_pos(deviceid, audioid, 60000);
          break;
        }
        case Operate::STOPAUDIO: {
          std::cout << "移除[" + std::to_string(deviceid) + "]设备上的[" +
                           std::to_string(audioid) + "]音频"
                    << std::endl;
          manager->stopAudio(deviceid, audioid);
          break;
        }
        case Operate::STOPDEVICE: {
          std::cout << "停止[" + std::to_string(deviceid) + "]设备"
                    << std::endl;
          manager->stopDevice(deviceid);
          break;
        }
        case Operate::PAUSE: {
          std::cout << "暂停[" + std::to_string(deviceid) + "]设备上的[" +
                           std::to_string(audioid) + "]音频"
                    << std::endl;
          manager->pauseAudio(deviceid, audioid);
          break;
        }
        case Operate::RESUME: {
          std::cout << "恢复[" + std::to_string(deviceid) + "]设备上的[" +
                           std::to_string(audioid) + "]音频"
                    << std::endl;
          manager->resumeAudio(deviceid, audioid);
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
    manager->unloadaudio(i);
  }

  return 0;
}
