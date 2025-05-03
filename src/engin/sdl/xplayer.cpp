#include "xplayer.h"

#include <SDL_audio.h>

#include <cmath>
#include <cstdint>
#include <cstring>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "../Sound.h"
#include "../mix/AudioOrbit.h"
#include "config/config.h"
#include "log/colorful-log.h"

XPlayer::XPlayer()
    : running(false), paused(false), rbuffer(x::Config::mix_buffer_size) {
  XTRACE("初始化播放器");
  desired_spec = new SDL_AudioSpec;
  obtained_spec = new SDL_AudioSpec;
  device_id = new SDL_AudioDeviceID;
  // sdl配置
  // 播放采样率
  (*(SDL_AudioSpec *)desired_spec).freq =
      static_cast<int>(x::Config::samplerate);
  // 浮点数据型(自动转换字节序大小端)
  (*(SDL_AudioSpec *)desired_spec).format = AUDIO_F32;
  // 声道数
  (*(SDL_AudioSpec *)desired_spec).channels =
      static_cast<uint8_t>(x::Config::channel);
  // 播放缓冲区大小
  (*(SDL_AudioSpec *)desired_spec).samples =
      static_cast<uint16_t>(x::Config::play_buffer_size);
  // 设置回调
  (*(SDL_AudioSpec *)desired_spec).callback = &XPlayer::sdl_audio_callback;
  // 用户数据
  (*(SDL_AudioSpec *)desired_spec).userdata = this;

  mixer = std::make_unique<XAuidoMixer>(this);
  XINFO("初始化播放器完成");
}

XPlayer::~XPlayer() {
  // 确保资源释放
  stop();
  delete (SDL_AudioSpec *)desired_spec;
  delete (SDL_AudioSpec *)obtained_spec;
  delete (SDL_AudioDeviceID *)device_id;
}
// 设置设备索引
void XPlayer::set_device_index(int device_index) {
  outdevice_index = device_index;
};

void XPlayer::player_thread() {
  // 打开设备
  auto did = SDL_OpenAudioDevice(SDL_GetAudioDeviceName(outdevice_index, 0), 0,
                                 (SDL_AudioSpec *)desired_spec,
                                 (SDL_AudioSpec *)obtained_spec, 0);
  *((SDL_AudioDeviceID *)device_id) = did;

  if (!device_id) {
    auto error = SDL_GetError();
    XERROR(std::string("启动设备时出错,请检查SDL设备索引,当前为[") +
           std::to_string(outdevice_index) + "]");
    XERROR(error);
    return;
  } else {
    XINFO("成功打开设备[" + std::to_string(outdevice_index) + "]");
  }
  // 开始播放
  SDL_PauseAudioDevice(*(SDL_AudioDeviceID *)device_id, 0);

  while (running) {
    {
      // LOG_DEBUG("播放器已暂停,等待恢复");
      // LOG_DEBUG("播放标识[" + std::to_string(running) + "]");
      // LOG_DEBUG("暂停标识[" + std::to_string(paused) + "]");
      //    暂停在此处等待
      auto pauselock = std::unique_lock<std::mutex>(player_mutex);
      cv.wait(pauselock, [this]() { return !paused || !running; });
    }
    // LOG_DEBUG("播放器播放中,等待暂停或终止");
    // LOG_DEBUG("播放标识[" + std::to_string(running) + "]");
    // LOG_DEBUG("暂停标识[" + std::to_string(paused) + "]");
    //   LOG_DEBUG("等待数据或继续播放");
    //   播放中在此等待
    //   等待数据推送或暂停恢复
    auto lock = std::unique_lock<std::mutex>(player_mutex);
    cv.wait(lock, [this]() { return paused || !running; });
  }

  SDL_CloseAudioDevice(*(SDL_AudioDeviceID *)device_id);
}

// 开始
void XPlayer::start() {
  // 防止重复启动
  if (running) return;
  if (outdevice_index < 0) {
    XWARN("尚未选择设备,播放器启动失败");
    return;
  }
  running = true;
  // 启动线程
  XINFO("启动播放线程...");
  sdl_playthread = std::thread(&XPlayer::player_thread, this);
  sdl_playthread.detach();

  XINFO("启动混音线程...");
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
  SDL_PauseAudioDevice(*(SDL_AudioDeviceID *)device_id, 1);
  if (mixer->mixthread.joinable()) mixer->mixthread.join();
}
// 暂停
void XPlayer::pause() {
  // 暂停
  paused = true;
  cv.notify_all();
}
// 继续
void XPlayer::resume() {
  // 继续
  paused = false;
  // 唤起线程
  cv.notify_all();
  mixercv.notify_all();
}
// 更改全局播放速度(变调)
void XPlayer::ratio(float speed) {
  static bool is_ratio_alting{false};
  while (is_ratio_alting) {
    std::this_thread::yield();
  }
  is_ratio_alting = true;
  pause();
  stop();
  // 修改播放采样率
  (*(SDL_AudioSpec *)desired_spec).freq =
      static_cast<int>(x::Config::samplerate) * speed;
  global_speed = speed;
  start();
  resume();
  is_ratio_alting = false;
}
// 更改全局音调
void XPlayer::pitch(float pth) {
  if (pth >= 0.2 && pth <= 2.0) {
    global_pitch = pth;
  }
}
// 设置播放器音量
void XPlayer::set_player_volume(float v) {
  if (v >= 0 && v <= 1.0f) {
    global_volume = v;
  } else {
    XWARN("非法音量值:[" + std::to_string(v) + "]");
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
  if (rbuffer.readable() <=
      int(floorf((float)x::Config::mix_buffer_size / 3.0f))) {
    // 数据即将使用完,请求更新
    player->isrequested = true;
    // XDEBUG("请求数据");
    // XDEBUG("当前缓冲区剩余:[" + std::to_string(rbuffer.readable()) + "]");
    player->mixercv.notify_all();
  }
  // SDL请求样本数
  size_t numSamples = len / sizeof(float);
  float *audiopcm;
  // 从环形缓冲区读取音频数据
  rbuffer.read(audiopcm, numSamples);
  // 直接在sdl播放回调填充更新立即音轨
  std::vector<std::shared_ptr<XAudioOrbit>> remove_list;
  for (const auto &[sound, immorbits] : player->mixer->immediate_orbits) {
    for (int i = 0; i < numSamples; ++i) {
      for (const auto &orbit : immorbits) {
        for (const auto &channel_pcm : sound->pcm) {
          if (orbit->playpos + i / 2 < sound->pcm[0].size()) {
            audiopcm[i] += sound->pcm[int(orbit->playpos + i) % 2]
                                     [int(orbit->playpos + i) / 2] *
                           player->global_volume * orbit->volume;
          } else {
            // 轨道播放完了
            remove_list.emplace_back(orbit);
          }
        }
        // 更新播放位置
        orbit->playpos += numSamples;
      }
    }
  }
  // 清理播放完的立即轨道
  for (const auto &orbit : remove_list) player->mixer->remove_orbit(orbit);

  if (!audiopcm) {
    // LOG_DEBUG("播放静音");
    // 读取失败时填充0
    std::memset(stream, 0, len);
    return;
  }
  // 写入sdl回调数据
  std::memcpy(stream, audiopcm, numSamples * sizeof(uint32_t));
};
