#ifndef X_AUDIO_MIXER_H
#define X_AUDIO_MIXER_H

#include <memory>
#include <thread>
#include <vector>

#include "engin/mix/AudioOrbit.h"

extern "C" {
#include <libavutil/opt.h>
#include <libswresample/swresample.h>
}

#include "../../gpu/gl/shader/shader.h"

class XPlayer;

class XAuidoMixer {
 public:
  // 全部音轨(句柄-轨道)
  std::unordered_map<int, std::shared_ptr<XAudioOrbit>> audio_orbits;
  // 混音线程
  std::thread mixthread;
  // 是否已初始化gl上下文
  static bool isglinitialized;
  // 着色器(opengl)
  static std::unique_ptr<Shader> glshader;
  // 着色器源代码
  static const char* vsource;
  static const char* fsource;
  // 目标播放器指针(仅传递方便访问,不释放,其他地方已管理)
  XPlayer* des_player;
  // 未知音轨
  XAudioOrbit unknown_orbit;
  // 全部音轨的原始数据-Planner
  // index1:音频索引;index2:声道索引;index3:数据索引
  std::vector<std::vector<std::vector<float>>> pcms;
  // 着色器程序
  // Shader* shader;
  // 顶点着色器源代码
  static const char* vertexshader_source;
  // 片段着色器源代码
  static const char* fragmentshader_source;

  // 混合音频
  void mix(const std::vector<std::shared_ptr<XAudioOrbit>>& src_sounds,
           std::vector<float>& mixed_pcm, float global_volume);
  void mix_pcmdata(std::vector<float>& mixed_pcm, float global_volume);
  // Planner存储的数据拉伸
  void stretch(std::vector<std::vector<float>>& pcm, size_t des_size);
  // 向播放器发送数据的线程函数
  void send_pcm_thread();
  // 添加音频轨道
  void add_orbit(const std::shared_ptr<XAudioOrbit>& sound);
  // 移除音频轨道
  bool remove_orbit(const std::shared_ptr<XAudioOrbit>& sound);
  bool remove_orbit(const std::shared_ptr<XSound>& sound);
  // 设置循环标识
  void setloop(int audio_handle, bool isloop);
  // 构造XAuidoMixer
  explicit XAuidoMixer(XPlayer* player);
  // 析构XAuidoMixer
  virtual ~XAuidoMixer();
};

#endif  // X_AUDIO_MIXER_H
