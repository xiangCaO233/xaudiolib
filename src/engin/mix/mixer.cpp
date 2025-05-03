#include "mixer.h"

#include <rubberband/RubberBandStretcher.h>

#include <algorithm>
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
    XINFO("gl已初始化过");
  }
}

XAuidoMixer::~XAuidoMixer() {}

// 添加音频轨道
void XAuidoMixer::add_orbit(const std::shared_ptr<XAudioOrbit> &orbit) {
  audio_orbits[orbit->sound].emplace_back(orbit);
  // XDEBUG("已添加音轨:[" + std::to_string(orbit->sound->handle) + ":" +
  //        orbit->sound->name + "]");
  // XDEBUG("音轨信息:[paused:" + std::to_string(orbit->paused) + "]");
  // XDEBUG("播放器信息:[paused:" + std::to_string(des_player->paused) + "]");

  if (!orbit->paused && des_player->paused) {
    des_player->resume();
  }
};

// 移除音频轨道
bool XAuidoMixer::remove_orbit(const std::shared_ptr<XAudioOrbit> &orbit) {
  auto orbit_list_it = audio_orbits.find(orbit->sound);
  if (orbit_list_it == audio_orbits.end()) {
    XWARN("此混音器不存在音源[" + orbit->sound->name + "]");
    return false;
  }

  auto orbitit = std::find(orbit_list_it->second.begin(),
                           orbit_list_it->second.end(), orbit);
  if (orbitit == orbit_list_it->second.end()) {
    XWARN("此音源不存在音轨[" + orbit->sound->name + "]");
    return false;
  }

  orbit_list_it->second.erase(orbitit);
  XINFO("已移除音轨[" + std::to_string(orbit->sound->handle) + ":" +
        orbit->sound->name + "]");

  // 检查是否应该继续播放
  bool shouldplay = false;
  for (const auto &[sound, orbits] : audio_orbits) {
    for (const auto &orbit : orbits) {
      if (!orbit->paused) {
        shouldplay = true;
        break;
      }
    }
    if (shouldplay) {
      break;
    }
  }
  if (!shouldplay) {
    des_player->pause();
  }

  return true;
};

bool XAuidoMixer::remove_orbit(const std::shared_ptr<XSound> &sound) {
  auto orbit_list_it = audio_orbits.find(sound);
  if (orbit_list_it != audio_orbits.end()) {
    audio_orbits.erase(orbit_list_it);
    return true;
  } else {
    return false;
  }
};

// 设置循环标识
void XAuidoMixer::setloop(int audio_handle, bool isloop) {
  for (auto &[sound, orbits] : audio_orbits) {
    if (sound->handle == audio_handle) {
      for (const auto &orbit : orbits) {
        orbit->loop = isloop;
      }
      return;
    }
  }
  XWARN("此混音器不存在音轨[" + std::to_string(audio_handle) + "]");
  return;
};

void XAuidoMixer::send_pcm_thread() {
  // 混音线程函数
  while (des_player->running) {
    // LOG_DEBUG("混音线程运行...");
    auto lock = std::unique_lock<std::mutex>(des_player->mix_mutex);
    // LOG_DEBUG("等待播放器请求数据");
    // 等待播放器请求数据

    // 等待时发送回调信号
    for (const auto &[sound, orbits] : audio_orbits) {
      for (const auto &audio_orbit : orbits) {
        if (audio_orbit->paused) continue;
        for (const auto &callback : audio_orbit->playpos_callbacks) {
          callback->playpos_call(audio_orbit->playpos);
        }
      }
    }

    des_player->mixercv.wait(lock, [this]() {
      // 等待数据请求或播放器停止
      return (!des_player->paused && des_player->isrequested) ||
             !des_player->running;
    });

    // 播放器停止则混音线程也立刻停止
    if (!des_player->running) break;
    // 每次发送数据量
    auto size = int(floorf((float)x::Config::mix_buffer_size / 3.0f));
    // 混合数据
    std::vector<float> mixed_pcm(size, 0);
    // 记录需要混合的音频
    auto sounds = std::vector<std::shared_ptr<XAudioOrbit>>();

    bool shouldplay = false;
    for (const auto &[sound, orbits] : audio_orbits) {
      for (const auto &audio : orbits) {
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
  size_t des_size = mixed_pcm.size() / static_cast<int>(x::Config::channel);

  // 扩充大小
  for (auto i = pcms.size(); i < src_sounds.size(); i++) {
    // 添加音频
    pcms.emplace_back();
    // 添加各个声道
    for (int i = 0; i < static_cast<int>(x::Config::channel); i++) {
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
        // 未设置循环时播放完成自动删除轨道
        audio->paused = true;
        if (audio->autoremove) {
          remove_orbit(audio);
        }
      }
    }

    // 获取变速属性
    auto speed = audio->speed;
    // 取出音频数据:
    /*
     * speed = 0.5时,取0.5 x des_size
     * speed = 2时,取2 x des_size
     * 最终拉伸到des_size
     */

    // 向上取整
    double actrual_play_data_size = std::ceil(speed * (double)des_size);

    // 为每个声道调整数组大小
    for (int j = 0; j < static_cast<int>(x::Config::channel); j++) {
      pcms[i][j].resize(des_size);
    }

    // 转移每个声道的数据
    if (speed == 1.0) {
      for (int k = 0; k < static_cast<int>(x::Config::channel); k++) {
        for (int j = 0; j < actrual_play_data_size; j++) {
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

    } else {
      if (speed < 1.0) {
        // 样本数比原来少了,依旧取des_size进行分析拉伸(居中),取结果中的input_data_size
        std::vector<std::vector<float>> temp_pcms(
            static_cast<int>(x::Config::channel), std::vector<float>(des_size));

        // 实际取样开始位置
        auto startpos = playpos - (des_size - actrual_play_data_size) / 2.0;
        // XWARN("实际音频取样开始位置[" + std::to_string(startpos) + "]");

        for (int k = 0; k < static_cast<int>(x::Config::channel); k++) {
          for (int j = 0; j < des_size; j++) {
            if (startpos + j < (double)audio->sound->pcm[k].size() &&
                startpos + j > 0) {
              // 第i个音频第k个声道第j个数据
              temp_pcms[k][j] =
                  audio->sound->pcm[k][(int)(startpos + j)] * audio->volume;
            } else {
              // 超过结尾或开头填充0
              temp_pcms[k][j] = 0;
            }
          }
        }
        // speed小于1时,总是使用des_size
        auto pre_stretch_size = temp_pcms[0].size();
        // XWARN("拉伸前音频样本数[" + std::to_string(pre_stretch_size) + "]");
        // 例:
        // 0.5x(原数据内容:-分析用,不播放;;+实际播放的样本-最终需要取出的数据)
        // ---++++++---
        // 变成
        // ------++++++++++++------
        // 将音频数据拉伸到指定时长
        stretch(temp_pcms, 1.0 / speed * pre_stretch_size);
        // 从结果中取出指定位置的数据
        auto stretch_res_size = temp_pcms[0].size();
        // XWARN("拉伸后音频样本数[" + std::to_string(stretch_res_size) + "]");
        auto pos_in_sample_pcm = (des_size - actrual_play_data_size) / 2.0;
        // XWARN("播放位置(在拉伸前的样本中)[" +
        // std::to_string(pos_in_sample_pcm)
        // +
        //       "]");
        auto pos_in_temppcm = (stretch_res_size - des_size) / 2.0;
        // XWARN("播放位置(在拉伸后的样本中)[" + std::to_string(pos_in_temppcm)
        // +
        //       "]");
        for (int k = 0; k < static_cast<int>(x::Config::channel); k++) {
          for (int j = 0; j < des_size; j++) {
            // 第i个音频第k个声道第j个数据
            pcms[i][k][j] =
                temp_pcms[k][(int)(pos_in_temppcm + j)] * audio->volume;
          }
        }
      } else {
        // 样本数比原来多了,取actrual_play_data_size进行分析拉伸,取结果中的全部
        std::vector<std::vector<float>> temp_pcms(
            static_cast<int>(x::Config::channel),
            std::vector<float>(actrual_play_data_size));
        for (int k = 0; k < static_cast<int>(x::Config::channel); k++) {
          for (int j = 0; j < actrual_play_data_size; j++) {
            if (playpos + j < (double)audio->sound->pcm[k].size() &&
                playpos + j > 0) {
              // 第i个音频第k个声道第j个数据
              temp_pcms[k][j] =
                  audio->sound->pcm[k][(int)(playpos + j)] * audio->volume;
            } else {
              // 超过结尾或开头填充0
              temp_pcms[k][j] = 0;
            }
          }
        }
        // 将音频数据拉伸到指定时长
        stretch(temp_pcms, 1.0 / speed * temp_pcms[0].size());
        // 从结果中取出指定位置的数据
        for (int k = 0; k < static_cast<int>(x::Config::channel); k++) {
          for (int j = 0; j < des_size; j++) {
            // 第i个音频第k个声道第j个数据
            pcms[i][k][j] = temp_pcms[k][j] * audio->volume;
          }
        }
      }
    }
    playpos += actrual_play_data_size;

    // 修正结尾
    if (playpos > (double)audio->sound->pcm[0].size()) {
      playpos = (double)audio->sound->pcm[0].size();
    }

    auto t = xutil::plannerpcmpos2milliseconds(
        (size_t)playpos, static_cast<int>(x::Config::samplerate));
    XINFO("[" + std::to_string(audio->sound->handle) + ":" +
          audio->sound->name + "]:当前播放位置:[" + std::to_string(t) + "ms]");
  }
  mix_pcmdata(mixed_pcm, global_volume);
}

void XAuidoMixer::mix_pcmdata(std::vector<float> &mixed_pcm,
                              float global_volume) {
  for (size_t i = 0; i < mixed_pcm.size(); i++) {
    // 混音
    for (auto &pcm : pcms) {
      if (i < (pcm[0].size() * pcm.size())) {
        // 交错写入混音数据行
        mixed_pcm[i] += pcm[i % 2][i / 2] * global_volume;
      } else {
        mixed_pcm[i] += 0;
      }
    }
  }
}

void XAuidoMixer::stretch(std::vector<std::vector<float>> &pcm,
                          size_t des_size) {
  // 无需要处理的数据,直接返回
  if (pcm.empty() || pcm[0].empty() || des_size == 0) return;

  // 原始数据大小(单声道)
  const size_t original_size = pcm[0].size();
  // 声道数量
  const size_t num_channels = pcm.size();
  // 原始大小与目标大小相同,拉伸倍率为1.0,不作处理,直接返回
  if (original_size == des_size) return;

  // 计算实际需要拉伸的倍率
  const float ratio =
      static_cast<float>(des_size) / static_cast<float>(original_size);

  // 构造拉伸器
  RubberBand::RubberBandStretcher stretcher(
      static_cast<int>(x::Config::samplerate), static_cast<int>(num_channels),
      RubberBand::RubberBandStretcher::OptionEngineFiner |  // R3引擎,时间精确
          RubberBand::RubberBandStretcher::
              OptionProcessOffline |  // 离线处理,一次完成全部数据
          RubberBand::RubberBandStretcher::
              OptionChannelsTogether |  // 所有声道一起分析
          RubberBand::RubberBandStretcher::
              OptionWindowStandard |                             // 短窗口,提速
          RubberBand::RubberBandStretcher::OptionThreadingAuto,  // 自动选择线程
      ratio, des_player->global_pitch);

  // 转化为rubberband需要的数组指针
  // 准备输入数据指针
  std::vector<const float *> input_ptrs(num_channels);
  for (size_t ch = 0; ch < num_channels; ++ch) {
    input_ptrs[ch] = pcm[ch].data();
  }
  // 学习样本
  stretcher.study(input_ptrs.data(), original_size, true);
  // 一次性提交所有数据
  stretcher.process(input_ptrs.data(), original_size, true);

  // 准备输出缓冲区
  // 定义拉伸后的数据行
  std::vector<std::vector<float>> stretched_pcm(num_channels,
                                                std::vector<float>(des_size));
  std::vector<float *> output_ptrs(num_channels);
  for (size_t ch = 0; ch < num_channels; ++ch) {
    output_ptrs[ch] = stretched_pcm[ch].data();
  }
  // 4. 正确处理可能的多次retrieve
  size_t retrieved = 0;
  while (retrieved < des_size) {
    int avail = stretcher.available();
    if (avail <= 0) break;

    size_t toRetrieve =
        std::min(static_cast<size_t>(avail), des_size - retrieved);
    int actual = stretcher.retrieve(output_ptrs.data(), toRetrieve);
    if (actual <= 0) break;

    // 移动指针
    for (size_t ch = 0; ch < num_channels; ++ch) {
      output_ptrs[ch] += actual;
    }
    retrieved += actual;
  }

  // 替换原始数据
  pcm.swap(stretched_pcm);
}
