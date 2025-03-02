#include "mixer.h"

#include <cmath>
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "../Sound.h"
#include "../sdl/xplayer.h"
#include "config/config.h"
#include "engin/util/utils.h"
#include "gpu/gl/shader/shader.h"
#include "log/colorful-log.h"

// 是否已初始化gl上下文
bool XAuidoMixer::isglinitialized = false;
std::unique_ptr<Shader> XAuidoMixer::glshader;

// 顶点着色器
const char *XAuidoMixer::vsource = R"(
#version 410 core


void main(){

}

)";

// 片段着色器
const char *XAuidoMixer::fsource = R"(
#version 410 core

void main(){

}

)";

XAuidoMixer::XAuidoMixer(XPlayer *player) : des_player(player) {
  XINFO("初始化混音器");
  unknown_prop.sound = nullptr;
  unknown_prop.playpos = -1;
  unknown_prop.loop = false;
  unknown_prop.paused = false;
  unknown_prop.volume = -1.0f;
  unknown_prop.speed = -1.0f;
  if (!isglinitialized) {
    XWARN("gl上下文未初始化");
    XINFO("正在初始化gl上下文");
    if (glfwInit()) {
      XINFO("glfw初始化成功");
      // 配置glfw参数
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
      // Apple平台前向适配
      XINFO("当前为Apple平台,启用opengl前向兼容");
      glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
      auto w = glfwCreateWindow(1, 1, "", nullptr, nullptr);
      glfwHideWindow(w);
      glfwPollEvents();
      if (w) {
        glfwMakeContextCurrent(w);
        if (glewInit() == GLEW_OK) {
          XINFO("glew初始化成功");
          XINFO("初始化opengl着色器");
          glshader = std::make_unique<Shader>(vsource, fsource);
          isglinitialized = true;
          // 终止glfw,防止未响应
          glfwTerminate();
        } else {
          XCRITICAL("glew初始化失败");
          glfwTerminate();
        }
      } else {
        XERROR("glfw窗体上下文初始化失败");
        glfwTerminate();
      }
    } else {
      XCRITICAL("glfw初始化失败");
    }
  }
}

XAuidoMixer::~XAuidoMixer() {
  XTRACE("析构[" + std::to_string(des_player->outdevice_index) + "]混音器");
}

// 添加音频轨道
void XAuidoMixer::add_orbit(const std::shared_ptr<XSound> &orbit) {
  // 加入音轨
  audio_orbits[orbit->handle] = orbit;
  // 初始化音轨属性
  auto &prop = orbit_properties[orbit->handle];
  prop.loop = false;
  prop.paused = false;
  prop.playpos = 0;
  prop.sound = orbit.get();
  prop.speed = 1.0f;
  prop.volume = 1.0f;
};
// 移除音频轨道
bool XAuidoMixer::remove_orbit(const std::shared_ptr<XSound> &orbit) {
  auto orbitit = audio_orbits.find(orbit->handle);
  if (orbitit == audio_orbits.end()) {
    XWARN("此混音器不存在音轨[" + orbit->name + "]");
    return false;
  }
  audio_orbits.erase(orbitit);
  auto propit = orbit_properties.find(orbit->handle);
  if (propit == orbit_properties.end()) {
    XWARN("此混音器不存在音轨[" + orbit->name + "]");
    return false;
  }
  orbit_properties.erase(propit);
  XINFO("已移除音轨[" + std::to_string(orbit->handle) + "]");
  return true;
};
// 设置循环标识
void XAuidoMixer::setloop(int audio_handle, bool isloop) {
  auto propit = orbit_properties.find(audio_handle);
  if (propit == orbit_properties.end()) {
    XWARN("此设备上不存在音轨--音频句柄[" + std::to_string(audio_handle) + "]");
    return;
  }
  propit->second.loop = isloop;
};
// 获取轨道属性
OrbitProps &XAuidoMixer::prop(int audio_handle) {
  auto propit = orbit_properties.find(audio_handle);
  if (propit == orbit_properties.end()) {
    XWARN("未找到音轨--音频句柄[" + std::to_string(audio_handle) + "]");
    return unknown_prop;
  }
  return propit->second;
};

void XAuidoMixer::send_pcm_thread() {
  // 混音线程函数
  while (des_player->running) {
    // LOG_DEBUG("混音线程运行...");
    auto lock = std::unique_lock<std::mutex>(des_player->mix_mutex);
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
    for (const auto &[handle, audio] : audio_orbits) {
      const auto &p = prop(audio->handle);
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
      XINFO("全部音频播放结束,已暂停");
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
void XAuidoMixer::mix(const std::vector<std::shared_ptr<XSound>> &src_sounds,
                      std::vector<float> &mixed_pcm, float global_volume) {
  // LOG_DEBUG("开始混音");
  // 目标数据大小
  size_t des_size = mixed_pcm.size();
  // 扩充大小
  for (auto i = src_pcms.size(); i < src_sounds.size(); i++) {
    src_pcms.emplace_back();
    src_pcms.back().reserve(10 * Config::mix_buffer_size);
  }
  if (src_pcms.size() > src_sounds.size()) {
    // 移除多余
    src_pcms.erase(src_pcms.end() - (src_pcms.size() - src_sounds.size()),
                   src_pcms.end());
  }

  for (int i = 0; i < src_sounds.size(); i++) {
    auto &audio = src_sounds[i];
    auto &p = prop(audio->handle);
    // 检查属性
    if (!p.sound) {
      XERROR("音频轨道属性出错");
      // 暂时静音
      std::fill(mixed_pcm.begin(), mixed_pcm.end(), 0.0f);
      return;
    }
    // 当前音轨播放位置
    auto &playpos = p.playpos;
    // 结束检查
    if (playpos >= audio->pcm_data.size()) {
      // 检查结尾
      XTRACE("[" + std::to_string(audio->handle) + "]播放结束");
      playpos = 0;
      auto &loop = p.loop;

      if (!loop) loop = true;
    }

    // 获取变速属性
    auto speed = p.speed;
    // 取出音频数据:
    /*
     * speed = 0.5时,取0.5 x des_size
     * speed = 2时,取2 x des_size
     */
    for (int j = 0; j < speed * (double)des_size; j++) {
      auto currentpos = std::floor(playpos+=1.0);
      if (currentpos <= (double)audio->pcm_data.size()) {
        src_pcms[i][j] = audio->pcm_data[(int)currentpos] * p.volume;
      } else {
        src_pcms[i][j] = 0;
      }
    }
    auto t = xutil::pcmpos2milliseconds(
        (size_t)playpos, static_cast<int>(Config::samplerate), 2);
    XINFO("[" + std::to_string(audio->handle) + ":" + audio->name +
          "]:当前播放位置:[" + std::to_string(t) + "ms]");
    // 修正结尾
    if (playpos > (double)audio->pcm_data.size()) {
      playpos = (double)audio->pcm_data.size();
    }
  }
  mix_pcmdata(mixed_pcm, global_volume);
}

void XAuidoMixer::mix_pcmdata(std::vector<float> &mixed_pcm,
                              float global_volume) {
  for (auto &pcm : src_pcms) {
    // 重采样
    resample(pcm, mixed_pcm.size());
  }
  for (size_t i = 0; i < mixed_pcm.size(); i++) {
    // 混音
    for (auto &pcm : src_pcms) {
      mixed_pcm[i] += pcm[i] * global_volume;
    }
  }
}

void XAuidoMixer::resample(std::vector<float> &pcm, size_t des_size) {
  // TODO(xiang 2025-03-02): 实现重采样
  int n = pcm.size();
  // 若原始大小与目标相同，则直接返回
  if (n == des_size) {
    return;
  }
  // 若原始为空，则直接扩容并填 0
  if (n == 0) {
    pcm.resize(des_size, 0.0f);
    return;
  }
  // 当目标长度为1时，直接保留第一个元素即可
  if (des_size == 1) {
    pcm.resize(1);
    return;
  }

  // 复制原始数据，防止在赋值过程中数据被覆盖
  std::vector<float> original(pcm.begin(), pcm.end());

  // 确保 pcm 的 size 至少能容纳 des_size 个元素（容量已足够时，resize
  // 不会触发内存重新分配）
  pcm.resize(des_size);

  // 对于每个目标下标 i，根据原数据通过线性插值计算新值
  for (int i = 0; i < des_size; ++i) {
    float t = i * (n - 1.0f) / (des_size - 1.0f);
    int idx = static_cast<int>(std::floor(t));
    int next = std::min(idx + 1, n - 1);
    float alpha = t - idx;
    pcm[i] = (1 - alpha) * original[idx] + alpha * original[next];
  }
}
