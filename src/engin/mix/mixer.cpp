#include "mixer.h"

#include <string>

#include "../Sound.h"
#include "../sdl/xplayer.h"
#include "config/config.h"
#include "logger/logger.h"

XAuidoMixer::XAuidoMixer(XPlayer* player) : des_player(player) {
    LOG_TRACE("初始化混音器");
}

XAuidoMixer::~XAuidoMixer() {
    LOG_TRACE("析构[" + std::to_string(des_player->outdevice_index) +
              "]混音器");
};

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

        bool shouldplay = false;
        for (auto& audioit : audio_orbits) {
            auto& audio = audioit.second;
            if (!audio->pauseflag) {
                shouldplay = true;
                if (des_player->paused)
                    // 需要播放,恢复播放线程
                    des_player->resume();
                auto size = int(floorf(Config::mix_buffer_size / 3.0f));
                if (audio->playpos >= audio->pcm_data.size()) {
                    // 检查结尾
                    LOG_TRACE("[" + std::to_string(audio->handle) +
                              "]播放结束");
                    audio->playpos = 0;
                    auto loopit = audio_loopflags.find(audioit.first);
                    if (loopit == audio_loopflags.end()) {
                        // 检查循环标识
                        LOG_WARN("缺少[" + std::to_string(audioit.first) +
                                 "]循环标识");
                        des_player->pause();
                        continue;
                    } else if (!loopit->second)
                        audio->pauseflag = true;
                }
                // 对齐结尾
                if (audio->playpos + size >= audio->pcm_data.size())
                    size = audio->pcm_data.size() - audio->playpos;
                // LOG_DEBUG("推送数据[" + std::to_string(size * 4) + "]bytes");
                //  写入数据到环形缓冲区
                des_player->rbuffer.write(
                    audio->pcm_data.data() + audio->playpos, size);
                LOG_DEBUG("当前播放到[" + std::to_string(audio->playpos) + "]");
                // LOG_DEBUG("推送数据完成");
                audio->playpos += size;
                des_player->cv.notify_all();
            }
        }
        // TODO(xiang 2024-12-18): 最后一个音频播放完时会重复暂停播放器
        if (!shouldplay) {
            // 没有需要播放的音频了
            LOG_DEBUG("全部音频播放结束,已暂停");
            des_player->pause();
        }
        des_player->isrequested = false;
    }
}
