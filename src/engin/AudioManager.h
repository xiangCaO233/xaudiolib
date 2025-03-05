#ifndef X_AUDIO_MANAGER_H
#define X_AUDIO_MANAGER_H

#include "AudioEngin.h"

class XAudioManager {
 private:
  // 引擎(唯一)
  std::unique_ptr<XAudioEngin> engin;

 public:
  // 构造XAudioManager音频管理器
  XAudioManager();
  // 析构XAudioManager
  virtual ~XAudioManager();

  static std::shared_ptr<XAudioManager> newmanager();
  // 启用日志
  void enableLoggin() const;
  // 禁用日志
  void disableLoggin() const;
  // 0-trace,1-debug,2-info,3-warning,4-error,5-critical
  void setLogginLevel(int level) const;
  // 载入音频
  int loadaudio(const std::string &audio, std::string &loaded_audio_name);
  // 卸载音频
  void unloadaudio(const std::string &audio);
  void unloadaudio(int id);

  // 获取音频名
  const std::string &get_audio_name(int id);

  // 获取音频路径
  const std::string &get_audio_path(int id);

  // 设置音频当前播放到的位置
  void set_audio_current_pos(int device_id, int id, int64_t time);
  void set_audio_current_pos(int device_id, const std::string &auido,
                             int64_t time);
  void set_audio_current_pos(const std::string &device, int id, int64_t time);
  void set_audio_current_pos(const std::string &device,
                             const std::string &auido, int64_t time);

  // 获取音频音量
  float getVolume(int deviceid, int id);
  float getVolume(int deviceid, const std::string &audioname);
  float getVolume(const std::string &devicename, int id);
  float getVolume(const std::string &devicename, const std::string &audio);

  // 设置音频音量
  void setAudioVolume(int deviceid, int id, float v);
  void setAudioVolume(int deviceid, const std::string &audioname, float v);
  void setAudioVolume(const std::string &devicename, int id, float v);
  void setAudioVolume(const std::string &devicename, const std::string &audio,
                      float v);

  // 全局播放速度(变调)
  float getDevicePlaySpeed(int device_index) const;
  void setDevicePlaySpeed(int device_index, float speed);
  // 全局音量
  float getGlobalVolume() const;
  void setGlobalAudioVolume(float volume);

  // 播放暂停恢复停止音频
  void playAudio(int device_index, int id, bool isloop);
  void playAudio(const std::string &device, const std::string &audioname,
                 bool isloop);
  void playAudio(int device_index, const std::string &audioname, bool isloop);
  void playAudio(const std::string &device, int id, bool isloop);

  void pauseAudio(int device_index, int id);
  void pauseAudio(const std::string &device, const std::string &audioname);
  void pauseAudio(int device_index, const std::string &audioname);
  void pauseAudio(const std::string &device, int id);

  void resumeAudio(int device_index, int id);
  void resumeAudio(const std::string &device, const std::string &audioname);
  void resumeAudio(int device_index, const std::string &audioname);
  void resumeAudio(const std::string &device, int id);

  void stopAudio(int device_index, int id);
  void stopAudio(const std::string &device, const std::string &audioname);
  void stopAudio(int device_index, const std::string &audioname);
  void stopAudio(const std::string &device, int id);

  // 设备是否暂停
  bool isDevicePause(int device_id);
  bool isDevicePause(const std::string &devicename);

  // 播放暂停停止设备上的播放器
  void pauseDevice(int device_id);
  void pauseDevice(const std::string &devicename);

  void resumeDevice(int device_id);
  void resumeDevice(const std::string &devicename);

  void stopDevice(int device_id);
  void stopDevice(const std::string &devicename);

  std::unordered_map<std::string, int> *get_handles();
  std::unordered_map<int, std::shared_ptr<XSound>> *get_audios();
  std::unordered_map<int, std::shared_ptr<XOutputDevice>> *get_outdevices();
  std::unordered_map<std::string, int> *get_outdevice_indicies();
  std::unordered_map<int, std::shared_ptr<XInputDevice>> *get_inputdevices();
  std::unordered_map<std::string, int> *get_inputdevice_indices();
};

#endif  // X_AUDIO_MANAGER_H
