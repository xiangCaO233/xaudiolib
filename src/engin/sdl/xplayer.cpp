#include "xplayer.h"

#include <memory>
#include <string>

#include "config/config.h"
#include "logger/logger.h"

XPlayer::XPlayer() : paused(false), running(false) {
    // 环形缓冲区配置
    // 初始化内存
    rbuffer.bufferhead =
        std::shared_ptr<float>(new float[Config::mix_buffer_size]);
    // 缓冲区大小
    rbuffer.buffersize = Config::mix_buffer_size;
    // 读写位置
    rbuffer.readpos = 0;
    rbuffer.writepos = 0;

    // sdl配置
    // 播放采样率
    desired_spec.freq = Config::samplerate;
    // 浮点数据型(自动转换字节序大小端)
    desired_spec.format = AUDIO_F32SYS;
    // 声道数
    desired_spec.channels = Config::channel;
    // 播放缓冲区大小
    desired_spec.samples = Config::play_buffer_size;
    // 设置回调
    desired_spec.callback = &XPlayer::audio_callback;
    // 用户数据
    desired_spec.userdata = this;

    // 混音器初始化
    mixer = std::make_shared<XAuidoMixer>(this);
}

XPlayer::~XPlayer() {
    // 确保资源释放
    stop();
}
// 设置设备索引
void XPlayer::set_device_index(int device_index) {
    outdevice_index = device_index;
};

// 开始
void XPlayer::start() {
    // TODO(xiang 2024-12-16): 启动播放器
    // 防止重复启动
    if (running) return;
    if (outdevice_index < 0) {
        LOG_ERROR("尚未选择设备,播放器启动失败");
        return;
    }
    running = true;
    LOG_DEBUG("启动播放线程...");
    // 启动线程
    sdl_playthread = std::thread([this]() {
        // 打开设备
        device_id =
            SDL_OpenAudioDevice(SDL_GetAudioDeviceName(outdevice_index, 0), 0,
                                &desired_spec, &obtained_spec, 0);
        if (!device_id) {
            auto error = SDL_GetError();
            LOG_ERROR("启动设备时出错,请检查SDL设备索引,当前为[" +
                      std::to_string(outdevice_index) + "]");
            LOG_ERROR(error);
            return;
        } else {
            LOG_INFO("成功打开设备[" + std::to_string(outdevice_index) + "]");
        }
        // 开始播放
        SDL_PauseAudioDevice(device_id, 0);
        while (running) {
            // LOG_DEBUG("等待数据或继续播放");
            // 等待数据推送或暂停恢复
            std::unique_lock<std::mutex> lock(buffer_mutex);
            cv.wait(lock, [this]() { return !paused || !running; });
        }
        SDL_CloseAudioDevice(device_id);
    });
    sdl_playthread.detach();
    LOG_DEBUG("启动混音线程");
    mixer->mixthread = std::thread(&XAuidoMixer::send_pcm_thread, mixer.get());
    mixer->mixthread.detach();
};
// 终止
void XPlayer::stop() {
    // TODO(xiang 2024-12-16): 停止播放器
    if (!running) return;
    running = false;
    // 唤起线程
    cv.notify_all();
    mixercv.notify_all();
    // 等待线程正常结束
    if (sdl_playthread.joinable()) sdl_playthread.join();
    LOG_DEBUG("播放线程结束");
    if (mixer->mixthread.joinable()) mixer->mixthread.join();
    LOG_DEBUG("混音线程结束");
};
// 暂停
void XPlayer::pause() {
    // TODO(xiang 2024-12-16): 暂停
    paused = true;
};
// 继续
void XPlayer::resume() {
    // TODO(xiang 2024-12-16): 继续
    paused = false;
    // 唤起线程
    cv.notify_all();
    mixercv.notify_all();
};

// 推送数据到环形缓冲区
void XPlayer::push_data(const float* data, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        // 写数据
        rbuffer.bufferhead.get()[rbuffer.writepos] = data[i];
        // 更新写指针
        rbuffer.writepos = (rbuffer.writepos + 1) % rbuffer.buffersize;
        // 如果写指针追上读指针,丢弃最老的数据
        if (rbuffer.writepos == rbuffer.readpos)
            rbuffer.readpos = (rbuffer.readpos + 1) % rbuffer.buffersize;
    }
};

// sdl播放回调函数
void XPlayer::audio_callback(void* userdata, uint8_t* stream, int len) {
    // 转换回播放器对象
    auto player = static_cast<XPlayer*>(userdata);
    // 暂停或者缓冲区无数据,填充静音数据
    if (player->paused || player->rbuffer.readpos == player->rbuffer.writepos) {
        std::memset(stream, 0, len);
        player->isrequested = true;
        player->mixercv.notify_all();
        return;
    }
    // 获取缓冲区数据
    int samples_requested = len / sizeof(float);
    float* output = reinterpret_cast<float*>(stream);
    for (int i = 0; i < samples_requested; ++i) {
        if (player->rbuffer.readpos == player->rbuffer.writepos) {
            // 缓冲区写完,输出静音
            output[i] = 0.0f;
        } else {
            output[i] =
                player->rbuffer.bufferhead.get()[player->rbuffer.readpos];
            player->rbuffer.readpos =
                (player->rbuffer.readpos + 1) % player->rbuffer.buffersize;
        }
    }
    LOG_DEBUG("检查缓冲区");
    // 如果数据量少于一半，唤起混音器继续提供数据
    if ((player->rbuffer.writepos - player->rbuffer.readpos +
         player->rbuffer.buffersize) %
            player->rbuffer.buffersize <
        player->rbuffer.buffersize / 2) {
        player->isrequested = true;
        player->mixercv.notify_all();
    }
};
