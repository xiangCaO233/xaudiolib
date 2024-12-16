#ifndef X_CONFIG_H
#define X_CONFIG_H

#include <string>

/*
 *缓存式
 */
#define AUDIO_DATA_BUFFERED 1
/*
 *流式
 */
#define AUDIO_DATA_STREAMED 2

/*
 *单声道
 */
#define AUDIO_SINGLE_CHANNEL 1
/*
 *单声道
 */
#define AUDIO_DOUBLE_CHANNEL 2

/*
 *奈奎斯特采样率标准(内存最小)
 */
#define NYQUIST_SAMPLERATE 44100

/*
 *数字音视频采样率标准(保证动态范围,后期处理方便)
 */
#define AVMEDIA_SAMPLERATE 48000

/*
 * 直接在sdl回调函数中进行混音
 */
#define CPU_MIX_BY_SDL 1
/*
 * 在OpenAL中进行混音
 */
#define CPU_MIX_BY_OPENAL 2
/*
 * 在OpenCL中进行混音
 */
#define GPU_MIX_BY_OPENCL 3
/*
 * 在OpenGL中进行混音
 */
#define GPU_MIX_BY_OPENGL 4
/*
 * 在Vulkan中进行混音
 */
#define GPU_MIX_BY_VULKAN 5

class XAudioEngin;
class XPlayer;
class XAuidoMixer;
class XAudioEncoder;
class XAudioDecoder;

class Config {
    // 音频传输方式
    const static int audio_transfer_method;
    // 音频解码线程数(max128)
    const static int decode_thread_count;
    // 混音方式
    const static int mix_method;
    // 混音处理时的环形缓冲区大小(设备性能越低这个需要越大)
    // 越大时可能会增加延迟
    const static int mix_buffer_size;
    // 播放音频时的声道数
    const static int channel;
    // 播放音频时的采样率
    const static int samplerate;
    // 播放音频的缓冲区大小
    const static int play_buffer_size;

    // 保存路径
    static std::string config_file_path;

    friend XAudioEngin;
    friend XPlayer;
    friend XAuidoMixer;
    friend XAudioEncoder;
    friend XAudioDecoder;

    // 载入配置
    static void load(const std::string &config_file);

    // 保存配置
    static void save();
};

#endif  // X_CONFIG_H
