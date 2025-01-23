#include "xplayer.h"

#include <SDL_audio.h>

#include <cmath>
#include <cstring>
#include <iostream>
#include <memory>
#include <mutex>
#include <ostream>
#include <string>
#include <thread>

#include "config/config.h"

XPlayer::XPlayer()
    : paused(false), running(false), rbuffer(Config::mix_buffer_size) {
  std::cout << "初始化播放器" << std::endl;
  // sdl配置
  // 播放采样率
  desired_spec.freq = Config::samplerate;
  // 浮点数据型(自动转换字节序大小端)
  desired_spec.format = AUDIO_F32;
  // 声道数
  desired_spec.channels = Config::channel;
  // 播放缓冲区大小
  desired_spec.samples = Config::play_buffer_size;
  // 设置回调
  desired_spec.callback = &XPlayer::sdl_audio_callback;
  // 用户数据
  desired_spec.userdata = this;
  mixer = std::make_unique<XAuidoMixer>(this);
}

XPlayer::~XPlayer() {
  // 确保资源释放
  stop();
  std::cout << "析构[" + std::to_string(outdevice_index) + "]设备播放器"
            << std::endl;
}
// 设置设备索引
void XPlayer::set_device_index(int device_index) {
  outdevice_index = device_index;
};

void XPlayer::player_thread() {
  // 打开设备
  device_id = SDL_OpenAudioDevice(SDL_GetAudioDeviceName(outdevice_index, 0), 0,
                                  &desired_spec, &obtained_spec, 0);
  if (!device_id) {
    auto error = SDL_GetError();
    std::cout << std::string("启动设备时出错,请检查SDL设备索引,当前为[") +
                     std::to_string(outdevice_index) + "]"
              << std::endl;
    std::cout << error << std::endl;
    return;
  } else {
    std::cout << "成功打开设备[" + std::to_string(outdevice_index) + "]"
              << std::endl;
  }
  // 开始播放
  SDL_PauseAudioDevice(device_id, 0);

  while (running) {
    {
      // LOG_DEBUG("播放器已暂停,等待恢复");
      // LOG_DEBUG("播放标识[" + std::to_string(running) + "]");
      // LOG_DEBUG("暂停标识[" + std::to_string(paused) + "]");
      //    暂停在此处等待
      std::unique_lock<std::mutex> pauselock(player_mutex);
      cv.wait(pauselock, [this]() { return !paused || !running; });
    }
    // LOG_DEBUG("播放器播放中,等待暂停或终止");
    // LOG_DEBUG("播放标识[" + std::to_string(running) + "]");
    // LOG_DEBUG("暂停标识[" + std::to_string(paused) + "]");
    //   LOG_DEBUG("等待数据或继续播放");
    //   播放中在此等待
    //   等待数据推送或暂停恢复
    std::unique_lock<std::mutex> lock(player_mutex);
    cv.wait(lock, [this]() { return paused || !running; });
  }

  SDL_CloseAudioDevice(device_id);
}

// 开始
void XPlayer::start() {
  // 防止重复启动
  if (running) return;
  if (outdevice_index < 0) {
    std::cout << "尚未选择设备,播放器启动失败" << std::endl;
    return;
  }
  running = true;
  // 启动线程
  std::cout << "启动播放线程..." << std::endl;
  sdl_playthread = std::thread(&XPlayer::player_thread, this);
  sdl_playthread.detach();

  std::cout << "启动混音线程..." << std::endl;
  mixer->mixthread = std::thread(&XAuidoMixer::send_pcm_thread, mixer.get());
  mixer->mixthread.detach();
};
// 终止
void XPlayer::stop() {
  // 停止播放器
  if (!running) return;
  running = false;
  // 唤起线程
  cv.notify_all();
  mixercv.notify_all();
  // 等待线程正常结束
  if (sdl_playthread.joinable()) sdl_playthread.join();
  // 暂停sdl设备
  SDL_PauseAudioDevice(device_id, 1);
  std::cout << "播放线程结束" << std::endl;
  if (mixer->mixthread.joinable()) mixer->mixthread.join();
  std::cout << "混音线程结束" << std::endl;
};
// 暂停
void XPlayer::pause() {
  // 暂停
  paused = true;
  cv.notify_all();
};
// 继续
void XPlayer::resume() {
  // 继续
  paused = false;
  // 唤起线程
  cv.notify_all();
  mixercv.notify_all();
};
void XPlayer::set_player_volume(float v) {
  if (v >= 0 && v <= 1.0f) {
    global_volume = v;
  } else {
    std::cout << "非法音量值:[" + std::to_string(v) + "]" << std::endl;
  }
}

// sdl播放回调函数
void XPlayer::sdl_audio_callback(void *userdata, uint8_t *stream, int len) {
  // 运行于播放线程
  // 转换回播放器对象
  auto player = static_cast<XPlayer *>(userdata);
  auto &rbuffer = player->rbuffer;
  // LOG_DEBUG("SDL读取音频数据...");
  // LOG_DEBUG("SDL请求长度:[" + std::to_string(len) + "]");
  // LOG_DEBUG("当前环形缓冲区-->{readpos:[" + std::to_string(rbuffer.readpos)
  // +
  //           "]::writepos:[" + std::to_string(rbuffer.writepos) + "]}");
  if (rbuffer.readable() <= int(floorf(Config::mix_buffer_size / 3.0))) {
    // 数据即将使用完,请求更新
    player->isrequested = true;
    // LOG_DEBUG("请求数据");
    // LOG_DEBUG("当前缓冲区剩余:[" + std::to_string(rbuffer.readable()) +
    //          "]");
    player->mixercv.notify_all();
  }
  // SDL请求样本数
  size_t numSamples = len / sizeof(float);
  float *audiopcm;
  // 从环形缓冲区读取音频数据
  rbuffer.read(audiopcm, numSamples);
  if (!audiopcm) {
    // LOG_DEBUG("播放静音");
    // 读取失败时填充0
    std::memset(stream, 0, len);
    return;
  }
  // 写入sdl回调数据
  std::memcpy(stream, audiopcm, numSamples * sizeof(uint32_t));
};
