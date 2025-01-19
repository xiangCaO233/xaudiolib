#ifndef API_X_AUDIO_MANAGER_H
#define API_X_AUDIO_MANAGER_H

#include <cstdint>
#include <memory>
#include <string>

class XAudioManager {
 private:
  // 内部接口
  void loadaudio(const std::string &audio);
  void unloadaudio(const std::string &audio);
  void unloadaudio(int id);

  const std::string &get_audio_name(int id);
  const std::string &get_audio_path(int id);

  float getVolume(int deviceid, int id);
  float getVolume(int deviceid, const std::string &audioname);
  float getVolume(const std::string &devicename, int id);
  float getVolume(const std::string &devicename, const std::string &audio);

  void setAudioVolume(int deviceid, int id, float v);
  void setAudioVolume(int deviceid, const std::string &audioname, float v);
  void setAudioVolume(const std::string &devicename, int id, float v);
  void setAudioVolume(const std::string &devicename, const std::string &audio,
                      float v);

  float getGlobalVolume();
  void setGlobalAudioVolume(float volume);

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

  void set_audio_current_pos(int device_id, int id, int64_t time);
  void set_audio_current_pos(int device_id, const std::string &auido,
                             int64_t time);
  void set_audio_current_pos(const std::string &device, int id, int64_t time);
  void set_audio_current_pos(const std::string &device,
                             const std::string &auido, int64_t time);

  bool isDevicePause(int device_id);
  bool isDevicePause(const std::string &devicename);

  void pauseDevice(int device_id);
  void pauseDevice(const std::string &devicename);

  void resumeDevice(int device_id);
  void resumeDevice(const std::string &devicename);

  void stopDevice(int device_id);
  void stopDevice(const std::string &devicename);

 public:
  // 公开接口
  static std::shared_ptr<XAudioManager> newmanager();
  // 载入音频
  void load(const std::string &audio_path);

  // 卸载音频
  void unload(const std::string &audio_name);
  void unload(int audio_id);

  // 获取音频信息
  const std::string &audio_name(int audio_id);
  const std::string &audio_path(int audio_id);

  // 获取音频音量
  float volume(int deviceid, int id);
  float volume(int deviceid, const std::string &audioname);
  float volume(const std::string &devicename, int id);
  float volume(const std::string &devicename, const std::string &audio);

  // 设置音频音量
  void setVolume(int deviceid, int id, float v);
  void setVolume(int deviceid, const std::string &audioname, float v);
  void setVolume(const std::string &devicename, int id, float v);
  void setVolume(const std::string &devicename, const std::string &audio,
                 float v);

  // 获取全局音量
  float globalVolume();
  // 设置全局音量
  void setGlobalVolume(float volume);

  // 播放音频句柄
  void play(int device_index, int audio_id, bool loop);
  void play(const std::string &devicename, const std::string &audioname,
            bool loop);
  void play(int device_index, const std::string &audioname, bool loop);
  void play(const std::string &devicename, int audio_id, bool loop);

  // 暂停音频句柄
  void pause(int device_index, int audio_id);
  void pause(const std::string &devicename, const std::string &audioname);
  void pause(int device_index, const std::string &audioname);
  void pause(const std::string &devicename, int audio_id);

  // 恢复音频句柄
  void resume(int device_index, int audio_id);
  void resume(const std::string &devicename, const std::string &audioname);
  void resume(int device_index, const std::string &audioname);
  void resume(const std::string &devicename, int audio_id);

  // 停止音频句柄
  void stop(int device_index, int audio_id);
  void stop(const std::string &devicename, const std::string &audioname);
  void stop(int device_index, const std::string &audioname);
  void stop(const std::string &devicename, int audio_id);

  // 设置播放时间
  void set_audio_time(int device_id, int id, int64_t time);
  void set_audio_time(int device_id, const std::string &auido, int64_t time);
  void set_audio_time(const std::string &device, int id, int64_t time);
  void set_audio_time(const std::string &device, const std::string &auido,
                      int64_t time);

  // 获取设备播放器状态
  bool is_pause(int device_id);
  bool is_pause(const std::string &devicename);

  // 暂停播放器
  void pause(int device_id);
  void pause(const std::string &devicename);

  // 恢复播放器
  void resume(int device_id);
  void resume(const std::string &devicename);

  // 终止播放器
  void stop(int device_id);
  void stop(const std::string &devicename);
};

#endif  // API_X_AUDIO_MANAGER_H
