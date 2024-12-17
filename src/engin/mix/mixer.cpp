#include "mixer.h"

#include "../Sound.h"
#include "../sdl/xplayer.h"
#include "config/config.h"
#include "logger/logger.h"

XAuidoMixer::XAuidoMixer(XPlayer* player) : des_player(player) {}

XAuidoMixer::~XAuidoMixer() = default;

void XAuidoMixer::send_pcm_thread() {
    // 混音线程函数
    while (des_player->running) {
        // LOG_DEBUG("混音线程运行...");
        std::unique_lock<std::mutex> lock(des_player->mix_mutex);
        // 等待播放器请求数据
        des_player->mixercv.wait(lock, [this]() {
            // 等待数据请求或播放器停止
            return (!des_player->paused && des_player->isrequested) ||
                   !des_player->running;
        });

        // 播放器停止则混音线程也立刻停止
        if (!des_player->running) break;
        for (auto& audioit : audio_orbits) {
            auto& audio = audioit.second;
            if (!audio->pauseflag) {
                auto size = int(floorf(Config::mix_buffer_size / 3.0f));
                LOG_DEBUG("推送数据[" + std::to_string(size * 4) + "]bytes");
                // 写入数据到环形缓冲区
                des_player->rbuffer.write(
                    audio->pcm_data.data() + audio->playpos, size);
                LOG_DEBUG("推送数据完成");
                audio->playpos += size;
                des_player->cv.notify_all();
            }
        }
        des_player->isrequested = false;
    }
}
