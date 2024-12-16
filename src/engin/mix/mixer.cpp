#include "mixer.h"

#include <cstring>
#include <string>

#include "../Sound.h"
#include "../sdl/xplayer.h"
#include "config/config.h"
#include "logger/logger.h"

XAuidoMixer::XAuidoMixer(XPlayer* player) : des_player(player) {}

XAuidoMixer::~XAuidoMixer() = default;

void XAuidoMixer::send_pcm_thread() {
    while (des_player->running) {
        LOG_DEBUG("等待更新请求");
        // 取得锁
        std::unique_lock<std::mutex> lock(des_player->buffer_mutex);
        // 等待播放器请求数据
        des_player->mixercv.wait(lock, [this]() {
            // 播放器为非暂停状态并在请求更新时或者播放器关闭了的时候解开线程锁
            return (!des_player->paused && des_player->isrequested) ||
                   !des_player->running;
        });

        // 在等待途中播放器突然关闭了
        if (!des_player->running) break;

        for (auto& orbit : audio_orbits) {
            auto audio = orbit.second;
            if (!audio->pauseflag) {
                // 获取音源数据
                auto srcdata = audio->pcm_data.data();
                size_t audiodata_size = Config::mix_buffer_size / 2;
                auto audiodata = new float[audiodata_size];

                // 导入pcm数据
                std::memcpy(audiodata, srcdata + audio->playpos,
                            audiodata_size);
                LOG_DEBUG("当前音频位置:[" + std::to_string(audio->playpos) +
                          "]");
                // 更新播放位置
                audio->playpos += audiodata_size;
                // 推送数据
                LOG_DEBUG("推送pcm数据");
                des_player->push_data(audiodata, audiodata_size);
                LOG_DEBUG("已推送pcm数据[" + std::to_string(audiodata_size) +
                          "]bytes");
                // 清理临时导出的音频数据
                delete[] audiodata;
                // 更新读位置并唤起可能暂停了的播放线程
                des_player->rbuffer.readpos =
                    des_player->rbuffer.writepos - audiodata_size;
                LOG_DEBUG("唤起播放线程");
                des_player->cv.notify_all();
            }
        }

        // 传输音频数据
        // float* audio_data = generate_audio_data();
        // size_t audio_size = get_audio_size(audio_data);
        // 推送数据到播放器的环形缓冲区
        // des_player->push_data(audio_data, audio_size);

        // 重置请求标志
        des_player->isrequested = false;
    }
}
