#include "AudioManager.h"

#include <filesystem>
#include <memory>
#include <string>
#include <utility>

#include "logger/logger.h"

std::string XSound::unknown = "unknown";
std::string XSound::unknown_path = "unknown path";

XAudioManager::XAudioManager() { XLogger::init(); }

XAudioManager::~XAudioManager() {}

std::shared_ptr<XAudioManager> XAudioManager::newmanager() {
    return std::make_shared<XAudioManager>();
}

int XAudioManager::loadaudio(const std::string &audio) {
    // TODO(xiang 2024-12-15): 实现载入音频
    static int id{0};
    std::filesystem::path path(audio);
    auto extension = path.extension().string();
    auto p = std::filesystem::absolute(path).string();
    auto name = path.filename().string();
    auto handelit = handles.find(name);
    if (handelit != handles.end()) {
        LOG_WARN("载入音频[" + audio + "]失败,音频已载入过,句柄[" +
                 std::to_string(handelit->second) + "]已存在");
        return id;
    } else {
        LOG_DEBUG("正在打开音频[" + audio + "]");
        // 添加句柄
        AVFormatContext *format = nullptr;
        if (avformat_open_input(&format, audio.c_str(), nullptr, nullptr) >=
            0) {
            LOG_INFO("打开[" + audio + "]成功,句柄[" + std::to_string(id) +
                     "]");
            // 初始化
            // 包装为智能指针
            auto audioformat = std::shared_ptr<AVFormatContext>(format);

            auto sound =
                std::make_shared<XSound>(id, name, p, audioformat, 1.0f, 0.3f);
            audios.insert({id, sound});
            handles[name] = id;

            // 获取编解码器
            auto codecit = audio_codecs.find(extension);
            // 获取流信息
            if (avformat_find_stream_info(format, nullptr) < 0) {
                LOG_ERROR("获取流信息失败");
                // TODO(xiang 2024-12-15): 清理前面塞入的数据
                return -1;
            }
            int streamindex = -1;
            if (codecit == audio_codecs.end()) {
                LOG_TRACE("未找到[" + extension + "]编解码器");
                streamindex = av_find_best_stream(format, AVMEDIA_TYPE_AUDIO,
                                                  -1, -1, nullptr, -1);
                // 直接在表中分配
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
            // 解码数据(直接填充到表所处内存中)
            if (codecit->second.first.decode_audio(
                    sound->audio_format, streamindex, sound->pcm_data) >= 0) {
                LOG_INFO("解码[" + sound->name + "]成功");
                LOG_INFO("音频数据大小:[" +
                         std::to_string(sound->pcm_data.size()) + "]bytes");
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
    auto handelit = handles.find(audio);
    if (handelit != handles.end()) {
        unloadaudio(handelit->second);
    } else {
        LOG_WARN("未加载过音频[" + audio + "]");
    }
};

void XAudioManager::unloadaudio(int id) {
    // TODO(xiang 2024-12-15): 实现使用id卸载音频
    auto audioit = audios.find(id);
    if (audioit != audios.end()) {
        // 删除句柄
        auto handelit = handles.find(audioit->second->name);
        handles.erase(handelit);
        LOG_INFO("已删除[" + audioit->second->name + "],句柄:[" +
                 std::to_string(id) + "]");
        // 删除音频
        audios.erase(audioit);
    } else {
        LOG_WARN("音频句柄[" + std::to_string(id) + "]不存在");
    }
};
// 获取音频名
const std::string &XAudioManager::get_audio_name(int id) {
    auto it = audios.find(id);
    if (it != audios.end()) return it->second->name;
    return XSound::unknown;
};
// 获取音频路径
const std::string &XAudioManager ::get_audio_path(int id) {
    auto it = audios.find(id);
    if (it != audios.end()) return it->second->path;
    return XSound::unknown_path;
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
    auto handelit = handles.find(audio);
    if (handelit != handles.end())
        return getVolume(handelit->second);
    else
        return -1.0f;
};
float XAudioManager::getVolume(int id) {
    auto it = audios.find(id);
    if (it != audios.end())
        return it->second->volume;
    else
        return -1.0f;
};
// 设置音量
void XAudioManager::setAudioVolume(const std::string &audio){};
void XAudioManager::setAudioVolume(int id){};
