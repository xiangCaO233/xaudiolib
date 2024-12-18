#include "mixer.h"

#include <iostream>
#include <list>
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
};

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
        auto size = int(floorf(Config::mix_buffer_size / 3.0f));
        // 混合数据
        std::vector<uint32_t> mixed_pcm(size, 0);
        // 使用链表记录需要混合的音频
        auto sounds = std::vector<std::shared_ptr<XSound>>();

        bool shouldplay = false;
        for (auto& audioit : audio_orbits) {
            auto& audio = audioit.second;
            if (!audio->pauseflag) {
                // LOG_DEBUG("检测到需要播放的音频");
                shouldplay = true;
                // 加入待混音列表
                sounds.push_back(audio);
                // if (audio->playpos >= audio->pcm_data.size()) {
                //     // 检查结尾
                //     LOG_TRACE("[" + std::to_string(audio->handle) +
                //               "]播放结束");
                //     audio->playpos = 0;
                //     auto loopit = audio_loopflags.find(audioit.first);
                //     if (loopit == audio_loopflags.end()) {
                //         // 检查循环标识
                //         LOG_WARN("缺少[" + std::to_string(audioit.first) +
                //                  "]循环标识");
                //         des_player->pause();
                //         continue;
                //     } else if (!loopit->second)
                //         audio->pauseflag = true;
                // }
                //// 对齐结尾
                // if (audio->playpos + size >= audio->pcm_data.size())
                //     size = audio->pcm_data.size() - audio->playpos;
                //// LOG_DEBUG("写入数据[" + std::to_string(size * 4) +
                ////"]bytes"); /  写入数据到环形缓冲区
                // des_player->rbuffer.write(
                //     audio->pcm_data.data() + audio->playpos, size);
                //// LOG_DEBUG("当前播放到[" + std::to_string(audio->playpos) +
                //// "]"); LOG_DEBUG("推送数据完成");
                // audio->playpos += size;

                if (des_player->paused) {
                    // 需要播放,恢复播放线程
                    des_player->resume();
                }
            }
        }
        if (!shouldplay) {
            // 没有需要播放的音频了
            LOG_DEBUG("全部音频播放结束,已暂停");
            des_player->pause();
        } else {
            mix(sounds, mixed_pcm, des_player->global_volume);
            //   写入缓冲区
            des_player->rbuffer.write(mixed_pcm.data(), size);
        }
        des_player->isrequested = false;
    }
}

class Compressor {
   private:
    float threshold;  // 压缩阈值
    float ratio;      // 压缩比率
    float attack;     // 攻击时间
    float release;    // 释放时间
    float lastGain;   // 上一帧的增益（简化版本）
   public:
    Compressor(float threshold, float ratio, float attack, float release)
        : threshold(threshold),
          ratio(ratio),
          attack(attack),
          release(release),
          lastGain(1.0f) {}
    // 应用压缩
    void process(std::vector<float>& signal) {
        for (size_t i = 0; i < signal.size(); ++i) {
            float sample = signal[i];
            float absSample = std::fabs(sample);

            // 判断是否超过阈值
            if (absSample > threshold) {
                // 计算压缩后的增益
                float gain = std::pow(absSample / threshold, 1.0f - ratio);
                sample *= gain;
            }

            // 更新增益
            signal[i] = sample;
        }
    }
};
void mixAudioStreams(const std::vector<std::vector<float>>& audioStreams,
                     std::vector<float>& mixedAudio,
                     const std::vector<float>& weights) {
    size_t numSamples = audioStreams[0].size();
    mixedAudio.resize(numSamples, 0.0f);  // 初始化混合音频

    for (size_t i = 0; i < audioStreams.size(); ++i) {
        const std::vector<float>& stream = audioStreams[i];
        float weight = weights[i];

        for (size_t j = 0; j < numSamples; ++j) {
            mixedAudio[j] += stream[j] * weight;
        }
    }
}

// 混合音频
void XAuidoMixer::mix(std::vector<std::shared_ptr<XSound>>& src_sounds,
                      std::vector<uint32_t>& mixed_pcm, float global_volume) {
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
                // 简单相加所有的采样
                mixed_pcm[i] += audio->pcm_data[audio->playpos + i];
                // mixed_pcm[i] = std::min<uint32_t>(
                //     mixed_pcm[i] + static_cast<uint32_t>(
                //                        audio->pcm_data[audio->playpos + i]),
                //     UINT32_MAX);
            }
        }

        if (audio->playpos + des_size >= audio->pcm_data.size()) {
            // 修正结尾
            audio->playpos = audio->pcm_data.size();
        } else {
            audio->playpos += des_size;
        }
    }
};
