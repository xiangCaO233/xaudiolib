#ifndef X_AUDIO_PLAYER_H
#define X_AUDIO_PLAYER_H

#include <SDL_audio.h>

#include <memory>
#include <thread>

#include "engin/mix/mixer.h"

struct ringbuffer {
    // 缓冲区物理头
    std::shared_ptr<float> bufferhead;
    // 缓冲区写入位置
    int writepos;
    // 缓冲区读取位置
    int readpos;
    // 缓冲区大小
    int buffersize;
};

class XPlayer {
    // 播放线程运行状态
    std::atomic<bool> running;
    // 播放暂停状态
    std::atomic<bool> paused;
    // sdl播放线程
    std::thread sdl_playthread;
    // 缓冲区互斥锁
    std::mutex buffer_mutex;
    // 条件变量,通知数据更新
    std::condition_variable cv;
    // 环形音频处理缓冲区
    ringbuffer rbuffer{};
    // sdl音频规范(期望)
    SDL_AudioSpec desired_spec{};
    // 此播放器绑定的混音器
    std::shared_ptr<XAuidoMixer> mixer;
    // 输出设备索引
    int outdevice_index{-1};
    // 播放设备
    SDL_AudioDeviceID device_id{};

    friend XAudioEngin;

   public:
    // 构造XPlayer
    XPlayer();
    // 析构XPlayer
    virtual ~XPlayer();

    // sdl音频规范(实际)
    SDL_AudioSpec obtained_spec{};

    // 设置设备索引
    void set_device_index(int device_index);
    // 开始
    void start();
    // 终止
    void stop();
    // 暂停
    void pause();
    // 继续
    void resume();

    // 推送数据到环形缓冲区
    void push_data(const float* data, size_t size);

    // sdl播放回调函数
    static void audio_callback(void* userdata, uint8_t* stream, int len);
};

#endif  // X_AUDIO_PLAYER_H
