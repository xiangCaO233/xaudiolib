#include "mixer.h"

#include <string>

#include "../Sound.h"
#include "../sdl/xplayer.h"
#include "config/config.h"
#include "logger/logger.h"

XAuidoMixer::XAuidoMixer(XPlayer* player) : des_player(player) {}

XAuidoMixer::~XAuidoMixer() = default;

void XAuidoMixer::send_pcm_thread() {
    // 混音线程函数
    while (des_player->running) {
        LOG_DEBUG("混音线程运行...");
        std::unique_lock<std::mutex> lock(des_player->mix_mutex);
        // 等待播放器请求数据
        des_player->mixercv.wait(lock, [this]() {
            // 等待数据请求或播放器停止
            return (!des_player->paused && des_player->isrequested) ||
                   !des_player->running;
        });
        // 播放器停止则混音线程也立刻停止
        if (!des_player->running) break;
        LOG_DEBUG("推送数据");
    }
}
