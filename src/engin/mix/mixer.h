#ifndef X_AUDIO_MIXER_H
#define X_AUDIO_MIXER_H

#include <memory>
#include <thread>
#include <unordered_map>
#include <vector>

#include "../../gpu/gl/shader/shader.h"

class XSound;
class XPlayer;
class XAudioEngin;

struct OrbitProps {
  XSound* sound;
  // 播放指针
  size_t playpos;
  // 轨道音量
  float volume;
  // 轨道指针播放速度
  float speed;
  // 轨道暂停标识
  bool paused;
  // 轨道循环标识
  bool loop;
};

class XAuidoMixer {
  // 混音线程
  std::thread mixthread;
  // 是否已初始化gl上下文
  static bool isglinitialized;
  // 着色器(opengl)
  static Shader* glshader;
  // 着色器源代码
  static const char *vsource, *fsource;
  // 目标播放器指针(仅传递方便访问,不释放,其他地方已管理)
  XPlayer* des_player;
  // 全部音轨(音频句柄-音频)
  std::unordered_map<int, std::shared_ptr<XSound>> audio_orbits;
  // 轨道属性(句柄-属性)
  std::unordered_map<int, OrbitProps> orbit_properties;
  // 未知属性
  OrbitProps unknown_prop;
  // 着色器程序
  // Shader* shader;
  // 顶点着色器源代码
  static const char* vertexshader_source;
  // 片段着色器源代码
  static const char* fragmentshader_source;

  friend XAudioEngin;
  friend XPlayer;

  // 添加音频轨道
  void add_orbit(std::shared_ptr<XSound>& orbit);
  // 移除音频轨道
  bool remove_orbit(std::shared_ptr<XSound>& orbit);
  // 设置循环标识
  void setloop(int audio_handle, bool isloop);

  // 获取轨道属性
  OrbitProps& prop(int audio_handle);

  // 混合音频
  void mix(std::vector<std::shared_ptr<XSound>>& src_sounds,
           std::vector<float>& mixed_pcm, float global_volume);
  // 向播放器发送数据的线程函数
  void send_pcm_thread();

 public:
  // 构造XAuidoMixer
  XAuidoMixer(XPlayer* player);
  // 析构XAuidoMixer
  virtual ~XAuidoMixer();
};

#endif  // X_AUDIO_MIXER_H
