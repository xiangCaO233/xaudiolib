#ifndef X_AUDIO_ENGIN_H
#define X_AUDIO_ENGIN_H

#include <memory>

#include "sdl/xplayer.h"
#if defined(__APPLE__)
// 苹果coreaudio库
#include <AudioToolbox/AudioToolbox.h>
#include <CoreAudio/CoreAudio.h>
#elif defined(__linux__)
#elif defined(_WIN32)
#elif defined(_WIN64)
#elif defined(__ANDROID__)
#else
#endif

#include <unordered_map>

#include "Sound.h"
#include "codec/decoder.h"
#include "codec/encoder.h"
#include "device/indevice.h"
#include "device/outdevice.h"
#include "mix/mixer.h"

class XAudioEngin {
  // 引擎接口
  // 当前注册到的id
  static int currentid;

  // 后缀名-编解码器池
  std::unordered_map<std::string, std::pair<std::shared_ptr<XAudioDecoder>,
                                            std::shared_ptr<XAudioEncoder>>>
      audio_codecs;

  // 音频句柄池
  std::unordered_map<std::string, int> handles;
  // 音频池{int-sound}
  std::unordered_map<int, std::shared_ptr<XSound>> audios;
  // 设备列表
  std::unordered_map<int, std::shared_ptr<XOutputDevice>> outdevices;
  std::unordered_map<std::string, int> outdevice_indicies;

  std::unordered_map<int, std::shared_ptr<XInputDevice>> inputdevices;
  std::unordered_map<std::string, int> inputdevice_indicies;

  // 全局音量
  float gVolume{0.2f};

  // 载入音频
  int load(const std::string &audio, std::string &loaded_audio_name);

  // 卸载音频
  void unload(const std::string &audio);
  void unload(int id);

  // 检查音频是否加载
  bool is_audio_loaded(const std::string &audioname, int &handle);
  bool is_audio_loaded(int audio_handle, std::shared_ptr<XSound> &res);
  // 检查设备是否存在
  bool is_indevice_exist(const std::string &devicename, int &device_index);
  bool is_indevice_exist(int device_index, std::shared_ptr<XInputDevice> &res);
  bool is_outdevice_exist(const std::string &devicename, int &device_index);
  bool is_outdevice_exist(int device_index,
                          std::shared_ptr<XOutputDevice> &res);

  // 获取音频名
  const std::string &audio_name(int id);
  // 获取音频路径
  const std::string &audio_path(int id);
  // 获取设备名
  const std::string &input_device_name(int id);
  const std::string &output_device_name(int id);

  // 获得音频句柄
  int audio_handle(const std::string &name);
  // 获得设备索引
  int input_device_index(const std::string &name);
  int output_device_index(const std::string &name);

  // 设置音频当前播放到的位置
  void pos(int deviceid, int id, int64_t time);

  // 获取音频音量
  float volume(int deviceid, int id);

  // 设置音频音量
  void setVolume(int deviceid, int id, float v);

  // 设置全局音量
  void setGlobalVolume(float volume);

  // 播放
  void play(int device_index, int audio_id, bool loop);

  // 暂停
  void pause(int device_index, int audio_id);

  // 恢复
  void resume(int device_index, int audio_id);

  // 终止
  void stop(int device_index, int audio_id);

  // 获取设备播放器状态
  bool is_pause(int device_id);

  // 暂停播放器
  void pause(int device_id);

  // 恢复播放器
  void resume(int device_id);

  // 播放暂停停止设备上的播放器
  void pause_device(int device_id);

  // 恢复设备上暂停的播放器
  void resume_device(int device_id);

  // 停止设备上的播放器
  void stop_player(int device_id);

  friend XAudioManager;

 public:
  // 创建XAudioEngin 引擎
  XAudioEngin();
  virtual ~XAudioEngin();
  // 初始化引擎
  static std::unique_ptr<XAudioEngin> init();
  // 关闭引擎
  static void shutdown();
};

#endif  // X_AUDIO_ENGIN_H
