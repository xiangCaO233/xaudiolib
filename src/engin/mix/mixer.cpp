#include "mixer.h"

#include <cmath>
#include <memory>
#include <string>
#include <vector>

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
}

void XAuidoMixer::send_pcm_thread() {
    // 混音线程函数
    while (des_player->running) {
        // LOG_DEBUG("混音线程运行...");
        std::unique_lock<std::mutex> lock(des_player->mix_mutex);
        // LOG_DEBUG("等待播放器请求数据");
        // 等待播放器请求数据
        des_player->mixercv.wait(lock, [this]() {
            // 等待数据请求或播放器停止
            return (!des_player->paused && des_player->isrequested) ||
                   !des_player->running;
        });

        // 播放器停止则混音线程也立刻停止
        if (!des_player->running) break;
        // 每次发送数据量
        auto size = int(floorf((float)Config::mix_buffer_size / 3.0f));
        // 混合数据
        std::vector<float> mixed_pcm(size, 0);
        // 记录需要混合的音频
        auto sounds = std::vector<std::shared_ptr<XSound>>();

        bool shouldplay = false;
        for (auto& audioit : audio_orbits) {
            auto& audio = audioit.second;
            if (!audio->pauseflag) {
                // LOG_DEBUG("检测到需要播放的音频");
                shouldplay = true;
                // 加入待混音列表
                sounds.push_back(audio);
                if (des_player->paused) {
                    // 需要播放,恢复播放线程
                    des_player->resume();
                }
            }
        }
        if (!shouldplay) {
            // 没有需要播放的音频了
            LOG_DEBUG("全部音频播放结束,已暂停");
            des_player->rbuffer.write(0.0f, des_player->rbuffer.available());
            des_player->pause();
        } else {
            mix(sounds, mixed_pcm, des_player->global_volume);
            //   写入缓冲区
            des_player->rbuffer.write(mixed_pcm.data(), size);
        }
        des_player->isrequested = false;
    }
}
// 混合音频
void XAuidoMixer::mix(std::vector<std::shared_ptr<XSound>>& src_sounds,
                      std::vector<float>& mixed_pcm, float global_volume) {
    // LOG_DEBUG("开始混音");
    size_t des_size = mixed_pcm.size();

    for (auto& audio : src_sounds) {
        // 结束检查
        if (audio->playpos >= audio->pcm_data.size()) {
            // 检查结尾
            LOG_TRACE("[" + std::to_string(audio->handle) + "]播放结束");
            audio->playpos = 0;
            auto loopit = audio_loopflags.find(audio->handle);
            if (loopit == audio_loopflags.end()) {
                // 检查循环标识
                LOG_WARN("缺少[" + std::to_string(audio->handle) + "]循环标识");
                des_player->pause();
                continue;
            } else if (!loopit->second)
                audio->pauseflag = true;
        }

        // 混合音频到目标
        for (int i = 0; i < des_size; ++i) {
            if (audio->playpos + i < audio->pcm_data.size()) {
                // 相加所有的采样
                mixed_pcm[i] += audio->pcm_data[audio->playpos + i] *
                                audio->volume * global_volume;
            }
        }

        if (audio->playpos + des_size >= audio->pcm_data.size()) {
            // 修正结尾
            audio->playpos = audio->pcm_data.size();
        } else {
            audio->playpos += des_size;
        }
    }
}
