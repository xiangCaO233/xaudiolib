#include "mixer.h"

#include <algorithm>
#include <cmath>
#include <memory>
#include <string>
#include <vector>

#include "../Sound.h"
#include "../sdl/xplayer.h"
#include "config/config.h"
#include "logger/logger.h"

// 顶点着色器源代码
const char* XAuidoMixer::vertexshader_source = R"(
#version 410 core

void main() {
	
}

)";

// 片段着色器源代码
const char* XAuidoMixer::fragmentshader_source = R"(
#version 410 core

void main() {
	
}

)";

XAuidoMixer::XAuidoMixer(XPlayer* player) : des_player(player) {
  LOG_TRACE("初始化混音器");
  unknown_prop.sound = nullptr;
  unknown_prop.playpos = -1;
  unknown_prop.loop = false;
  unknown_prop.paused = false;
  unknown_prop.volume = -1.0f;
  unknown_prop.speed = -1.0f;
  // if (Config::mix_method == GPU_MIX_BY_OPENGL) {
  //     // 使用opengl api进行并行混音
  //     LOG_DEBUG("当前使用OpenGL进行混音");
  //     shader = new Shader(vertexshader_source, fragmentshader_source,
  //     true);
  // }
}

XAuidoMixer::~XAuidoMixer() {
  LOG_TRACE("析构[" + std::to_string(des_player->outdevice_index) + "]混音器");
}

// 添加音频轨道
void XAuidoMixer::add_orbit(std::shared_ptr<XSound>& orbit) {
  // 加入音轨
  audio_orbits[orbit->handle] = orbit;
  // 初始化音轨属性
  auto& prop = orbit_properties[orbit->handle];
  prop.loop = false;
  prop.paused = false;
  prop.playpos = 0;
  prop.sound = orbit.get();
  prop.speed = 1.0f;
  prop.volume = 1.0f;
};
// 移除音频轨道
bool XAuidoMixer::remove_orbit(std::shared_ptr<XSound>& orbit) {
  auto orbitit = audio_orbits.find(orbit->handle);
  if (orbitit == audio_orbits.end()) {
    LOG_WARN("此混音器不存在音轨[" + orbit->name + "]");
    return false;
  }
  audio_orbits.erase(orbitit);
  auto propit = orbit_properties.find(orbit->handle);
  if (propit == orbit_properties.end()) {
    LOG_WARN("此混音器不存在音轨[" + orbit->name + "]");
    return false;
  }
  orbit_properties.erase(propit);
  LOG_INFO("已移除音轨[" + std::to_string(orbit->handle) + "]");
  return true;
};
// 设置循环标识
void XAuidoMixer::setloop(int audio_handle, bool isloop) {
  auto propit = orbit_properties.find(audio_handle);
  if (propit == orbit_properties.end()) {
    LOG_WARN("此设备上不存在音轨--音频句柄[" + std::to_string(audio_handle) +
             "]");
    return;
  }
  propit->second.loop = isloop;
};
// 获取轨道属性
OrbitProps& XAuidoMixer::prop(int audio_handle) {
  auto propit = orbit_properties.find(audio_handle);
  if (propit == orbit_properties.end()) {
    LOG_WARN("未找到音轨--音频句柄[" + std::to_string(audio_handle) + "]");
    return unknown_prop;
  }
  return propit->second;
};

void XAuidoMixer::send_pcm_thread() {
  // 混音线程函数
  while (des_player->running) {
    // LOG_DEBUG("混音线程运行...");
    std::unique_lock<std::mutex> lock(des_player->mix_mutex);
    // LOG_DEBUG("等待播放器请求数据");
    // 等待播放器请求数据
    des_player->mixercv.wait(lock, [this]() {
      // 等待数据请求或播放器停止
      return (!des_player->paused && des_player->isrequested) ||
             !des_player->running;
    });

    // 播放器停止则混音线程也立刻停止
    if (!des_player->running) break;
    // 每次发送数据量
    auto size = int(floorf((float)Config::mix_buffer_size / 3.0f));
    // 混合数据
    std::vector<float> mixed_pcm(size, 0);
    // 记录需要混合的音频
    auto sounds = std::vector<std::shared_ptr<XSound>>();

    bool shouldplay = false;
    for (auto& audioit : audio_orbits) {
      auto& audio = audioit.second;
      auto& p = prop(audio->handle);
      if (!p.paused) {
        // LOG_DEBUG("检测到需要播放的音频");
        shouldplay = true;
        // 加入待混音列表
        sounds.push_back(audio);
        if (des_player->paused) {
          // 需要播放,恢复播放线程
          des_player->resume();
        }
      }
    }
    if (!shouldplay) {
      // 没有需要播放的音频了
      LOG_DEBUG("全部音频播放结束,已暂停");
      des_player->rbuffer.write(0.0f, des_player->rbuffer.available());
      des_player->pause();
    } else {
      mix(sounds, mixed_pcm, des_player->global_volume);
      //   写入缓冲区
      des_player->rbuffer.write(mixed_pcm.data(), size);
    }
    des_player->isrequested = false;
  }
}
// 混合音频
void XAuidoMixer::mix(std::vector<std::shared_ptr<XSound>>& src_sounds,
                      std::vector<float>& mixed_pcm, float global_volume) {
  // LOG_DEBUG("开始混音");
  size_t des_size = mixed_pcm.size();

  for (auto& audio : src_sounds) {
    auto& p = prop(audio->handle);

    if (!p.sound) {
      LOG_ERROR("音频轨道属性出错");
      // 暂时静音
      for (auto& var : mixed_pcm) {
        var = 0.0f;
      }
      return;
    }
    auto& playpos = p.playpos;
    // 结束检查
    if (playpos >= audio->pcm_data.size()) {
      // 检查结尾
      LOG_TRACE("[" + std::to_string(audio->handle) + "]播放结束");
      playpos = 0;
      auto& loop = p.loop;
      if (!loop) loop = true;
    }

    // 混合音频到目标
    for (int i = 0; i < des_size; ++i) {
      auto scale = std::log(global_volume);
      if (playpos + i < audio->pcm_data.size()) {
        // TODO(tan 2024-12-24): 获取音频数据并实现变速写入混音结果
        // 相加所有的采样(限制最大值)
        mixed_pcm[i] = std::clamp(
            mixed_pcm[i] + audio->pcm_data[p.playpos + i] * p.volume * scale,
            -0.9f, 0.9f);
      }
    }
    // for (int i = 0; i < 50; i++) {
    //     std::cout << mixed_pcm[i] << ',';
    // }
    // std::cout << std::endl;

    if (playpos + des_size >= audio->pcm_data.size()) {
      // 修正结尾
      playpos = audio->pcm_data.size();
    } else {
      playpos += des_size;
    }
  }
}
