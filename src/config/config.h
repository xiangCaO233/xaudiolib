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

class Config {
    // 音频传输方式
    static int audio_transfer_method;
    // 音频解码线程数(max128)
    static int decode_thread_count;
    // 混音方式
    static int mix_method;
    // GPU混音时的环形缓冲区大小
    static int gpu_buffer_size;
    // 播放音频的缓冲区大小
    static int play_buffer_size;

    friend XAudioEngin;

   public:
    // 构造Config
    Config(std::string &config_file);
    // 析构Config
    virtual ~Config();

    void save();
};

#endif  // X_CONFIG_H
