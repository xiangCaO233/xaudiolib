#include "AudioManager.h"

#include <libavformat/avformat.h>
#include <libavutil/avutil.h>

#include <filesystem>
#include <memory>
#include <string>
#include <utility>

#include "logger/logger.h"

XAudioManager::XAudioManager() {
    XLogger::init();
    audio_names[-1] = "unknown";
    audio_paths[-1] = "unknown path";
}

XAudioManager::~XAudioManager() {}

std::shared_ptr<XAudioManager> XAudioManager::newmanager() {
    return std::make_shared<XAudioManager>();
}

int XAudioManager::loadaudio(const std::string &audio) {
    // TODO(xiang 2024-12-15): 实现载入音频
    static int id{0};
    auto handelit = audio_handles.find(audio);
    if (handelit != audio_handles.end()) {
        LOG_WARN("载入音频[" + audio + "]失败,音频已载入过,句柄[" +
                 std::to_string(id) + "]已存在");
    } else {
        LOG_DEBUG("正在打开音频[" + audio + "]");
        // 添加句柄
        AVFormatContext *format = nullptr;
        if (avformat_open_input(&format, audio.c_str(), nullptr, nullptr) >=
            0) {
            LOG_INFO("打开[" + audio + "]成功,句柄[" + std::to_string(id) +
                     "]");
            // 初始化信息
            audio_handles[audio] = id;
            // 包装为智能指针
            audio_formats[id] = std::shared_ptr<AVFormatContext>(format);
            std::filesystem::path path(audio);
            auto extension = path.extension().string();
            audio_paths[id] = std::filesystem::absolute(path).string();
            audio_names[id] = path.filename().string();
            audio_speeds[id] = 1.0f;
            audio_volumes[id] = 0.3f;
            auto codecit = audio_codecs.find(extension);
            // 获取流信息
            if (avformat_find_stream_info(format, nullptr) < 0) {
                LOG_ERROR("获取流信息失败");
                // TODO(xiang 2024-12-15): 清理前面塞入的数据
                return -1;
            }
            int streamindex = -1;
            if (codecit == audio_codecs.end()) {
                LOG_TRACE("未找到[" + extension + "]解码器");
                streamindex = av_find_best_stream(format, AVMEDIA_TYPE_AUDIO,
                                                  -1, -1, nullptr, -1);
                // 直接在哈希表中分配
                audio_codecs.emplace(
                    extension,
                    std::pair<XAudioDecoder, XAudioEncoder>(
                        {format->streams[streamindex]->codecpar->codec_id,
                         format->streams[streamindex]->codecpar->codec_id}));
            } else {
                LOG_TRACE("找到解码器:[" + extension + "]");
            }
            if (streamindex == -1)
                streamindex = av_find_best_stream(format, AVMEDIA_TYPE_AUDIO,
                                                  -1, -1, nullptr, -1);
            codecit = audio_codecs.find(extension);
            // 解码数据(直接填充到哈希表所处内存中)
            if (codecit->second.first.decode_audio(
                    audio_formats[id], streamindex, audio_pcm_datas[id]) >= 0) {
                LOG_INFO("解码[" + audio_names[id] + "]成功");
                LOG_INFO("音频数据大小:[" +
                         std::to_string(audio_pcm_datas[id].size()) + "]bytes");
            } else {
                LOG_ERROR("解码出现问题");
                // TODO(xiang 2024-12-15): 清除前部分填充的数据
                return -1;
            }

        } else {
            LOG_ERROR("打开[" + audio + "]失败,请检查文件");
            return -1;
        }
    }

    return id++;
};
void XAudioManager::unloadaudio(const std::string &audio) {
    // TODO(xiang 2024-12-15): 实现卸载音频
    auto handelit = audio_handles.find(audio);
    if (handelit != audio_handles.end()) {
        auto formatit = audio_formats.find(handelit->second);
        // 移除format记录
        audio_formats.erase(formatit, formatit);

        auto nameit = audio_names.find(handelit->second);
        audio_names.erase(nameit, nameit);

        auto pathit = audio_paths.find(handelit->second);
        audio_paths.erase(pathit, pathit);

        auto speedit = audio_speeds.find(handelit->second);
        audio_speeds.erase(speedit, speedit);

        auto volumeit = audio_volumes.find(handelit->second);
        audio_volumes.erase(volumeit, volumeit);

        auto pcmdatait = audio_pcm_datas.find(handelit->second);
        audio_pcm_datas.erase(pcmdatait, pcmdatait);

        audio_handles.erase(handelit, handelit);
        LOG_WARN("已卸载[" + audio + "],句柄[" +
                 std::to_string(handelit->second) + "]");
    } else {
        LOG_WARN("未加载过音频[" + audio + "]");
    }
};
void XAudioManager::unloadaudio(int id) {
    // TODO(xiang 2024-12-15): 实现使用id卸载音频
    LOG_DEBUG("卸载音频[" + std::to_string(id) + "]");
    auto nameit = audio_names.find(id);
    if (nameit != audio_names.end()) {
        auto formatit = audio_formats.find(id);
        // 移除format记录
        audio_formats.erase(formatit, formatit);

        auto speedit = audio_speeds.find(id);
        audio_speeds.erase(speedit, speedit);

        auto volumeit = audio_volumes.find(id);
        audio_volumes.erase(volumeit, volumeit);

        auto handelit = audio_handles.find(nameit->second);
        audio_handles.erase(handelit, handelit);

        auto pcmdatait = audio_pcm_datas.find(id);
        audio_pcm_datas.erase(pcmdatait, pcmdatait);

        audio_names.erase(nameit, nameit);

        auto pathit = audio_paths.find(id);
        audio_paths.erase(pathit, pathit);

        LOG_WARN("已卸载[" + nameit->second + "],句柄[" + std::to_string(id) +
                 "]");
    } else {
        LOG_WARN("音频句柄[" + std::to_string(id) + "]不存在");
    }
};
// 获取音频名
const std::string &XAudioManager::get_audio_name(int id) {
    auto nameit = audio_names.find(id);
    if (nameit != audio_names.end()) return nameit->second;
    return audio_names[-1];
};
// 获取音频路径
const std::string &XAudioManager ::get_audio_path(int id) {
    auto pathit = audio_paths.find(id);
    if (pathit != audio_paths.end()) return pathit->second;
    return audio_paths[-1];
};
// 设置音频当前播放到的位置
void XAudioManager::set_audio_current_pos(const std::string &auido,
                                          int64_t time){
    // TODO(xiang 2024-12-15): 设置音频播放位置
};
void XAudioManager::set_audio_current_pos(int id, int64_t time){
    // TODO(xiang 2024-12-15): 设置音频播放位置
};

// 获取音量
float XAudioManager::getVolume(const std::string &audio) {
    auto handelit = audio_handles.find(audio);
    if (handelit != audio_handles.end())
        return audio_volumes[handelit->second];
    else
        return -1.0f;
};
float XAudioManager::getVolume(int id) {
    auto volumeit = audio_volumes.find(id);
    if (volumeit != audio_volumes.end())
        return volumeit->second;
    else
        return -1.0f;
};
// 设置音量
void XAudioManager::setAudioVolume(const std::string &audio){};
void XAudioManager::setAudioVolume(int id){};
