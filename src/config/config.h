#ifndef X_CONFIG_H
#define X_CONFIG_H

#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

enum class transfertype {
  /*
   *缓存式
   */
  AUDIO_DATA_BUFFERED = 1,
  /*
   *流式
   */
  AUDIO_DATA_STREAMED = 2
};

enum class channels {
  /*
   *单声道
   */
  AUDIO_SINGLE_CHANNEL = 1,
  /*
   *双声道
   */
  AUDIO_DOUBLE_CHANNEL = 2
};

enum class sampleratetype {
  /*
   *奈奎斯特采样率标准(内存最小)
   */
  NYQUIST_SAMPLERATE = 44100,
  /*
   *数字音视频采样率标准(保证动态范围,后期处理方便)
   */
  AVMEDIA_SAMPLERATE = 48000,
  /*
   *无损采样
   */
  LOSSLESS_SAMPLERATE = 96000
};

enum class mixtype {
  /*
   * 在CPU中进行混音
   */
  CPU_MIX = 1,
  /*
   * 在OpenCL中进行混音
   */
  GPU_MIX_BY_OPENCL = 2,
  /*
   * 在OpenGL中进行混音
   */
  GPU_MIX_BY_OPENGL = 3
};

class XAudioEngin;
class XPlayer;
class XAuidoMixer;
class XAudioEncoder;
class XAudioDecoder;

class Config {
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

  friend XAudioEngin;
  friend XPlayer;
  friend XAuidoMixer;
  friend XAudioEncoder;
  friend XAudioDecoder;
  static void to_json(json& j);
  static void from_json(const json& j);

  // 载入配置
  static void load();

  // 保存配置
  static void save();
};

#endif  // X_CONFIG_H
