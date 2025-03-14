#include "mixer.h"

#include <rubberband/RubberBandStretcher.h>

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
  XTRACE("初始化混音器");
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
          XINFO("混音器创建成功");
        } else {
          XCRITICAL("glew初始化失败");
          XERROR("混音器创建失败");
          glfwTerminate();
        }
      } else {
        XERROR("glfw窗体上下文初始化失败");
        XERROR("混音器创建失败");
        glfwTerminate();
      }
    } else {
      XCRITICAL("glfw初始化失败");
      XERROR("混音器创建失败");
    }
  } else {
    XWARN("gl已初始化过");
  }
}

XAuidoMixer::~XAuidoMixer() {
  XTRACE("析构[" + std::to_string(des_player->outdevice_index) + "]混音器");
}

// 添加音频轨道
void XAuidoMixer::add_orbit(const std::shared_ptr<XAudioOrbit> &orbit) {
  audio_orbits.try_emplace(orbit->sound->handle, orbit);
  XDEBUG("已添加音轨:[" + std::to_string(orbit->sound->handle) + ":" +
         orbit->sound->name + "]");
  XDEBUG("音轨信息:[paused:" + std::to_string(orbit->paused) + "]");
  XDEBUG("播放器信息:[paused:" + std::to_string(des_player->paused) + "]");

  if (!orbit->paused && des_player->paused) {
    des_player->resume();
  }
};
// 移除音频轨道
bool XAuidoMixer::remove_orbit(const std::shared_ptr<XAudioOrbit> &orbit) {
  auto orbitit = audio_orbits.find(orbit->sound->handle);
  if (orbitit == audio_orbits.end()) {
    XWARN("此混音器不存在音轨[" + orbit->sound->name + "]");
    return false;
  }
  audio_orbits.erase(orbitit);

  XINFO("已移除音轨[" + std::to_string(orbit->sound->handle) + ":" +
        orbit->sound->name + "]");

  // 检查是否应该继续播放
  bool shouldplay = false;
  for (const auto &[handle, orbit] : audio_orbits) {
    if (!orbit->paused) {
      shouldplay = true;
      break;
    }
  }
  if (!shouldplay) {
    des_player->pause();
  }

  return true;
};
bool XAuidoMixer::remove_orbit(const std::shared_ptr<XSound> &audio) {
  auto orbitit = audio_orbits.find(audio->handle);
  if (orbitit == audio_orbits.end()) {
    XWARN("此混音器不存在音轨[" + audio->name + "]");
    return false;
  }
  audio_orbits.erase(orbitit);

  XINFO("已移除音轨[" + std::to_string(audio->handle) + ":" + audio->name +
        "]");
  return true;
};

// 设置循环标识
void XAuidoMixer::setloop(int audio_handle, bool isloop) {
  auto orbitit = audio_orbits.find(audio_handle);
  if (orbitit == audio_orbits.end()) {
    XWARN("此混音器不存在音轨[" + std::to_string(audio_handle) + "]");
    return;
  }
  orbitit->second->loop = isloop;
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
    auto sounds = std::vector<std::shared_ptr<XAudioOrbit>>();

    bool shouldplay = false;
    for (const auto &[handle, audio] : audio_orbits) {
      if (!audio->paused) {
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
      XINFO("全部音频播放结束,mixer已暂停");
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
void XAuidoMixer::mix(
    const std::vector<std::shared_ptr<XAudioOrbit>> &src_sounds,
    std::vector<float> &mixed_pcm, float global_volume) {
  // LOG_DEBUG("开始混音");
  // 目标数据大小(sdl需求的交错存储的数据)
  size_t des_size = mixed_pcm.size() / static_cast<int>(Config::channel);

  // 扩充大小
  for (auto i = pcms.size(); i < src_sounds.size(); i++) {
    // 添加音频
    pcms.emplace_back();
    // 添加各个声道
    for (int i = 0; i < static_cast<int>(Config::channel); i++) {
      pcms.back().emplace_back();
      // 为每个声道预分配空间(至少四倍-最低0.25x倍速)
      pcms.back().back().reserve(4 * des_size);
    }
  }

  if (pcms.size() > src_sounds.size()) {
    // 移除多余
    pcms.erase(pcms.end() - (pcms.size() - src_sounds.size()), pcms.end());
  }

  for (int i = 0; i < src_sounds.size(); i++) {
    auto &audio = src_sounds[i];
    // 检查属性
    if (!audio->sound) {
      XERROR("音频轨道属性出错");
      // 暂时静音
      std::fill(mixed_pcm.begin(), mixed_pcm.end(), 0.0f);
      return;
    }

    // 当前音轨播放位置
    auto &playpos = audio->playpos;

    // 结束检查(检查一个声道即可)
    if (playpos >= audio->sound->pcm[0].size()) {
      // 检查结尾
      XTRACE("[" + std::to_string(audio->sound->handle) + "]播放结束");
      if (audio->loop) {
        playpos = 0;
      } else {
        audio->paused = true;
      }
    }

    // 获取变速属性
    auto speed = audio->speed;
    // 取出音频数据:
    /*
     * speed = 0.5时,取0.5 x des_size
     * speed = 2时,取2 x des_size
     */
    double input_data_size = speed * (double)des_size;

    // 为每个声道调整数组大小
    for (int j = 0; j < static_cast<int>(Config::channel); j++) {
      pcms[i][j].resize((int)input_data_size);
    }

    // 转移每个声道的数据
    for (int k = 0; k < static_cast<int>(Config::channel); k++) {
      for (int j = 0; j < input_data_size; j++) {
        if (playpos + j < (double)audio->sound->pcm[k].size()) {
          // 第i个音频第k个声道第j个数据
          pcms[i][k][j] =
              audio->sound->pcm[k][(int)(playpos + j)] * audio->volume;
        } else {
          // 超过结尾填充0
          pcms[i][k][j] = 0;
        }
      }
    }
    playpos += input_data_size;

    // 修正结尾
    if (playpos > (double)audio->sound->pcm[0].size()) {
      playpos = (double)audio->sound->pcm[0].size();
    }

    auto t = xutil::plannerpcmpos2milliseconds(
        (size_t)playpos, static_cast<int>(Config::samplerate));
    XINFO("[" + std::to_string(audio->sound->handle) + ":" +
          audio->sound->name + "]:当前播放位置:[" + std::to_string(t) + "ms]");

    for (const auto &callback : audio->playpos_callbacks) {
      callback->playpos_call(playpos);
    }
  }
  mix_pcmdata(mixed_pcm, global_volume);
}

void XAuidoMixer::mix_pcmdata(std::vector<float> &mixed_pcm,
                              float global_volume) {
  for (auto &pcm : pcms) {
    // 拉伸
    stretch(pcm, mixed_pcm.size());
  }
  for (size_t i = 0; i < mixed_pcm.size(); i++) {
    // 混音
    for (auto &pcm : pcms) {
      // 交错写入混音数据行
      mixed_pcm[i] += pcm[i % 2][i / 2] * global_volume;
    }
  }
}

void XAuidoMixer::stretch(std::vector<std::vector<float>> &pcm,
                          size_t des_size) {
  // TODO(xiang 2025-03-02): 实现拉伸
  float *pcmdata_ptr[pcm.size()];
  for (int i = 0; i < pcm.size(); i++) {
    pcmdata_ptr[i] = pcm[i].data();
  }
  /*
   *
   * RubberBandStretcher(size_t sampleRate,
   *                     size_t channels,
   *                     Options options = DefaultOptions,
   *                     double initialTimeRatio = 1.0,
   *                     double initialPitchScale = 1.0);
   */
  RubberBand::RubberBandStretcher s(
      static_cast<int>(Config::samplerate), static_cast<int>(Config::channel),
      RubberBand::RubberBandStretcher::OptionProcessRealTime, 1.0);
}
