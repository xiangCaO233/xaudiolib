#ifndef X_AUDIO_PLAYER_H
#define X_AUDIO_PLAYER_H

#include <SDL_audio.h>

#include <memory>
#include <thread>

#include "engin/mix/mixer.h"
#include "rbuffer.h"

class XOutputDevice;

class XPlayer {
    // 播放线程运行状态
    bool running;
    // 播放暂停状态
    bool paused;
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
    // sdl音频规范(期望)
    SDL_AudioSpec desired_spec{};
    // 此播放器绑定的混音器
    std::shared_ptr<XAuidoMixer> mixer;
    // 输出设备索引
    int outdevice_index{-1};
    // 播放设备
    SDL_AudioDeviceID device_id{};

    friend XAudioEngin;
    friend XAuidoMixer;
    friend XOutputDevice;

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
    void push_data(const uint32_t* data, size_t size);

    // sdl播放回调函数
    static void audio_callback(void* userdata, uint8_t* stream, int len);
};

#endif  // X_AUDIO_PLAYER_H
