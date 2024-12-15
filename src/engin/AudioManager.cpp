#include "AudioManager.h"

#include <libavformat/avformat.h>

#include <filesystem>
#include <memory>
#include <string>

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
        LOG_DEBUG("打开音频[" + audio + "]");
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
            audio_paths[id] = std::filesystem::absolute(path).string();
            audio_names[id] = path.filename().string();
            audio_speeds[id] = 1.0f;
            audio_volumes[id] = 1.0f;
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

        audio_handles.erase(handelit, handelit);
        LOG_INFO("已卸载[" + audio + "],句柄[" +
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

        audio_names.erase(nameit, nameit);

        auto pathit = audio_paths.find(handelit->second);
        audio_paths.erase(pathit, pathit);

        LOG_INFO("已卸载[" + nameit->second + "],句柄[" + std::to_string(id) +
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
