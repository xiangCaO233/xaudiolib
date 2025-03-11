#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

class XAudioEngin;
class XOutputDevice;
class XAudioManager;
class XPlayer;
class Shader;
struct AVFormatContext;

enum class transfertype;
enum class mixtype;
enum class channels;
enum class sampleratetype;

class Config {
 public:
  // 音频传输方式
  static transfertype audio_transfer_method;
  // 音频解码线程数(max128)
  static int decode_thread_count;
  // 混音方式
  static mixtype mix_method;
  // 混音处理时的环形缓冲区大小(设备性能越低这个需要越大)
  // 越大时可能会增加延迟
  static int mix_buffer_size;
  // 播放音频时的声道数
  static channels channel;
  // 播放音频时的采样率
  static sampleratetype samplerate;
  // 播放音频的缓冲区大小
  static int play_buffer_size;

  // 保存路径
  static std::string config_file_path;

  // static void to_json(json& j);
  // static void from_json(const json& j);

  // 载入配置
  static void load();

  // 保存配置
  static void save();
};

namespace xutil {
int64_t plannerpcmpos2milliseconds(size_t plannerpcmpos, int pcmsamplerate);
size_t milliseconds2plannerpcmpos(int64_t milliseconds, int pcmsamplerate);
}  // namespace xutil

class ringbuffer {
  // 缓冲区本体
  std::shared_ptr<float[]> buffer;
  // 读取缓冲区
  std::shared_ptr<float[]> readbuffer;
  // 缓冲区大小（包含1个空闲单元）
  size_t buffersize;
  // 读取指针
  size_t readpos;
  // 写入指针
  size_t writepos;

 public:
  explicit ringbuffer(size_t size);
  ~ringbuffer();

  // 可读数据量
  [[nodiscard]] size_t readable() const;
  // 可写入容量
  [[nodiscard]] size_t available() const;
  // 写数据
  bool write(const float *data, size_t size);
  bool write(float value, size_t size);
  // 读数据
  bool read(float *&data, size_t size);
};

class XSound {
 public:
  // 句柄(id)
  int handle;
  // pcm数据(分声道)
  std::vector<std::vector<float>> pcm;
  // 音频文件名
  const std::string name;
  // 音频路径
  const std::string path;
  // 音频格式
  AVFormatContext *audio_format;

  // 构造XSound
  XSound(int h, std::string n, std::string p, AVFormatContext *f);
  // 析构XSound
  virtual ~XSound();

  // 获取位置(按帧)
  size_t locateframe(size_t frameindex) const;
  // 获取位置(按采样)
  size_t locatesample(size_t sampleindex) const;
  // 获取位置(按时间)
  size_t locatetime(size_t milliseconds) const;
  // 获取音频数据大小
  size_t get_pcm_data_size() const;
};

class PlayposCallBack {
 public:
  virtual ~PlayposCallBack() = default;
  virtual void playpos_call(double playpos) = 0;
};

class XAudioOrbit {
 public:
  std::shared_ptr<XSound> sound;
  // 回调列表
  std::vector<std::shared_ptr<PlayposCallBack>> playpos_callbacks;
  // 播放指针
  double playpos{0.0};
  // 轨道音量
  float volume{1.0f};
  // 轨道指针播放速度
  float speed{1.0f};
  // 轨道暂停标识
  bool paused{false};
  // 轨道循环标识
  bool loop{false};

  // 构造XAudioOrbit
  explicit XAudioOrbit(std::shared_ptr<XSound> audio = nullptr);
  // 析构XAudioOrbit
  ~XAudioOrbit() = default;
  // 添加播放位置回调
  void add_playpos_callback(std::shared_ptr<PlayposCallBack> callback);
  // 移除回调
  void remove_playpos_callback(std::shared_ptr<PlayposCallBack> callback);
};

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
  static const char *vsource;
  static const char *fsource;
  // 目标播放器指针(仅传递方便访问,不释放,其他地方已管理)
  XPlayer *des_player;
  // 未知音轨
  XAudioOrbit unknown_orbit;
  // 全部音轨的原始数据-Planner
  std::vector<std::vector<std::vector<float>>> pcms;
  // 着色器程序
  // Shader* shader;
  // 顶点着色器源代码
  static const char *vertexshader_source;
  // 片段着色器源代码
  static const char *fragmentshader_source;

  // 混合音频
  void mix(const std::vector<std::shared_ptr<XAudioOrbit>> &src_sounds,
           std::vector<float> &mixed_pcm, float global_volume);
  void mix_pcmdata(std::vector<float> &mixed_pcm, float global_volume);
  // Planner存储的数据拉伸
  void stretch(std::vector<std::vector<float>> &pcm, size_t des_size);
  // 向播放器发送数据的线程函数
  void send_pcm_thread();
  // 添加音频轨道
  void add_orbit(const std::shared_ptr<XAudioOrbit> &sound);
  // 移除音频轨道
  bool remove_orbit(const std::shared_ptr<XAudioOrbit> &sound);
  bool remove_orbit(const std::shared_ptr<XSound> &sound);
  // 设置循环标识
  void setloop(int audio_handle, bool isloop);
  // 构造XAuidoMixer
  explicit XAuidoMixer(XPlayer *player);
  // 析构XAuidoMixer
  virtual ~XAuidoMixer();
};

class XInputDevice {
 public:
  // 设备名称
  std::string device_name;
  // 设备sdlid
  int sdl_id;

  // 构造XInputDevice
  XInputDevice(int id, std::string &name);
  // 析构XInputDevice
  virtual ~XInputDevice();
};

class XPlayer {
 public:
  // 构造XPlayer
  XPlayer();
  // 析构XPlayer
  virtual ~XPlayer();
  // 播放线程运行状态
  bool running;
  // 播放暂停状态
  bool paused;
  // 全局音量
  float global_volume;
  // 全局速度
  float global_speed{1.0f};
  // 数据请求状态
  bool isrequested{false};
  // 混音互斥锁
  std::mutex mix_mutex;
  // 条件变量,通知混音器请求数据更新
  std::condition_variable mixercv;
  // 缓冲区互斥锁
  std::mutex player_mutex;
  // 条件变量,通知数据更新
  std::condition_variable cv;
  // sdl播放线程
  std::thread sdl_playthread;
  // 环形音频处理缓冲区
  ringbuffer rbuffer;
  // 此播放器绑定的混音器
  std::unique_ptr<XAuidoMixer> mixer;
  // 输出设备索引
  int outdevice_index{-1};

  // sdl音频规范(实际)
  void *obtained_spec{};
  // sdl音频规范(期望)
  void *desired_spec{};
  // 播放设备
  void *device_id{};

  void player_thread();

  // 设置设备索引
  void set_device_index(int device_index);
  void set_player_volume(float v);

  // 开始
  void start();
  // 终止
  void stop();
  // 暂停
  void pause();
  // 继续
  void resume();
  // 更改全局播放速度(变调)
  void ratio(float speed);

  // sdl播放回调函数
  static void sdl_audio_callback(void *userdata, uint8_t *stream, int len);
};

class XOutputDevice {
 public:
  // 设备sdlid(仅索引)
  int sdl_id;
  // 设备名称
  std::string device_name;
  // 播放器
  std::shared_ptr<XPlayer> player;

  // 构造XOutputDevice
  XOutputDevice(int id, std::string &name);
  // 析构XOutputDevice
  virtual ~XOutputDevice();

  // 创建一个位于该设备的播放器
  bool creat_player();
};

class XAudioManager {
 public:
  // 构造XAudioManager音频管理器
  XAudioManager();
  // 析构XAudioManager
  virtual ~XAudioManager();

  // 引擎(唯一)
  std::unique_ptr<XAudioEngin> engin;

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

#endif  // AUDIO_MANAGER_H
