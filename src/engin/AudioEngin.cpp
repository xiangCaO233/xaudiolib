#include "AudioEngin.h"

#include <SDL.h>
#include <SDL_audio.h>

#include <filesystem>
#include <memory>
#include <string>

#include "config/config.h"
#include "engin/sdl/xplayer.h"
#include "logger/logger.h"

std::string XSound::unknown = "unknown";
std::string XSound::unknown_path = "unknown path";

// 引擎实现
int XAudioEngin::currentid = 0;

XAudioEngin::XAudioEngin() {}

XAudioEngin::~XAudioEngin() { shutdown(); }

std::unique_ptr<XAudioEngin> XAudioEngin::init() {
    LOG_DEBUG("初始化音频引擎");
    auto e = std::make_unique<XAudioEngin>();
    auto sdl_init_ret = SDL_Init(SDL_INIT_AUDIO);
    if (sdl_init_ret < 0) {
        LOG_ERROR("sdl初始化失败");
        return nullptr;
    }
    auto inputs = SDL_GetNumAudioDevices(true);
    if (inputs < 0) {
        LOG_ERROR("获取输入设备失败");
        return nullptr;
    }
    for (int i = 0; i < inputs; i++) {
        auto input_device_name = SDL_GetAudioDeviceName(i, true);
        auto device_name_str = std::string(input_device_name);
        auto inputdevice = std::make_shared<XInputDevice>(i, device_name_str);
        LOG_DEBUG("检测到输入设备" + device_name_str);
        e->inputdevice_indicies.insert({device_name_str, i});
        e->inputdevices.insert({i, inputdevice});
    }

    auto outputs = SDL_GetNumAudioDevices(false);
    if (outputs < 0) {
        LOG_ERROR("获取输出设备失败");
        return nullptr;
    }
    for (int i = 0; i < outputs; i++) {
        auto output_device_name = SDL_GetAudioDeviceName(i, false);
        auto device_name_str = std::string(output_device_name);
        auto outdevice = std::make_shared<XOutputDevice>(i, device_name_str);
        LOG_DEBUG("检测到输出设备:" + device_name_str);
        e->outdevice_indicies.insert({device_name_str, i});
        e->outdevices.insert({i, outdevice});
    }
    return e;
}

void XAudioEngin::shutdown() { SDL_Quit(); }

int XAudioEngin::load(const std::string &audio) {
    std::filesystem::path path(audio);
    auto extension = path.extension().string();
    auto p = std::filesystem::absolute(path).string();
    auto name = path.filename().string();
    auto handelit = handles.find(name);
    if (handelit != handles.end()) {
        LOG_WARN("载入音频[" + audio + "]失败,音频已载入过,句柄[" +
                 std::to_string(handelit->second) + "]");
        return currentid;
    } else {
        LOG_DEBUG("正在打开音频[" + audio + "]");
        // 添加句柄
        AVFormatContext *format = nullptr;
        if (avformat_open_input(&format, audio.c_str(), nullptr, nullptr) >=
            0) {
            LOG_INFO("打开[" + audio + "]成功,句柄[" +
                     std::to_string(currentid) + "]");
            // 初始化
            // 包装为智能指针
            auto audioformat = std::shared_ptr<AVFormatContext>(format);

            auto sound = std::make_shared<XSound>(currentid, name, p,
                                                  audioformat, 1.0f, 0.3f);
            audios.insert({currentid, sound});
            handles[name] = currentid;

            // 获取编解码器
            auto codecit = audio_codecs.find(extension);
            // 获取流信息
            if (avformat_find_stream_info(format, nullptr) < 0) {
                LOG_ERROR("获取流信息失败");
                // 清理前面塞入的数据
                handelit = handles.find(name);
                handles.erase(handelit);
                auto audioit = audios.find(currentid);
                audios.erase(audioit);

                return -1;
            }
            int streamindex = -1;
            if (codecit == audio_codecs.end()) {
                LOG_WARN("未找到[" + extension + "]编解码器");
                streamindex = av_find_best_stream(format, AVMEDIA_TYPE_AUDIO,
                                                  -1, -1, nullptr, -1);
                // 直接在表中分配
                audio_codecs.emplace(
                    extension,
                    std::pair<XAudioDecoder, XAudioEncoder>(
                        {format->streams[streamindex]->codecpar->codec_id,
                         format->streams[streamindex]->codecpar->codec_id}));
            } else {
                LOG_INFO("找到解码器:[" + extension + "]");
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
                handelit = handles.find(name);
                handles.erase(handelit);
                auto audioit = audios.find(currentid);
                audios.erase(audioit);
                audio_codecs.erase(codecit);

                return -1;
            }
        } else {
            LOG_ERROR("打开[" + audio + "]失败,请检查文件");
            return -1;
        }
    }

    return currentid++;
};
void XAudioEngin::unload(const std::string &audio) {
    // 卸载音频
    auto handelit = handles.find(audio);
    if (handelit != handles.end()) {
        unload(handelit->second);
    } else {
        LOG_WARN("未加载过音频[" + audio + "]");
    }
};

void XAudioEngin::unload(int id) {
    // 使用id卸载音频
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
const std::string &XAudioEngin::name(int id) {
    auto it = audios.find(id);
    if (it != audios.end()) return it->second->name;
    return XSound::unknown;
};
// 获取音频路径
const std::string &XAudioEngin ::path(int id) {
    auto it = audios.find(id);
    if (it != audios.end()) return it->second->path;
    return XSound::unknown_path;
};

// 设置音频当前播放到的位置
void XAudioEngin::pos(const std::string &auido, int64_t time){
    // TODO(xiang 2024-12-15): 设置音频播放位置
};
void XAudioEngin::pos(int id, int64_t time){
    // TODO(xiang 2024-12-15): 设置音频播放位置
};

// 获取音量
float XAudioEngin::volume(const std::string &audio) {
    auto handelit = handles.find(audio);
    if (handelit != handles.end())
        return volume(handelit->second);
    else
        return -1.0f;
};
float XAudioEngin::volume(int id) {
    auto it = audios.find(id);
    if (it != audios.end())
        return it->second->volume;
    else
        return -1.0f;
};
// 设置音量
void XAudioEngin::setVolume(const std::string &audio, float v) {
    auto handelit = handles.find(audio);
    if (handelit != handles.end())
        setVolume(handelit->second, v);
    else
        LOG_WARN("设置失败,[" + audio + "]不存在");
};
void XAudioEngin::setVolume(int id, float v) {
    auto audioit = audios.find(id);
    if (audioit != audios.end()) {
        audioit->second->volume = v;
        LOG_INFO("已将句柄[" + std::to_string(id) + "]音量设置为[" +
                 std::to_string(v) + "]");
    } else
        LOG_WARN("句柄[" + std::to_string(id) + "]不存在");
};
// 设置全局音量
void XAudioEngin::setGlobalVolume(float volume) {
    if (volume >= 0 && volume <= 1.0f)
        gVolume = volume;
    else
        LOG_WARN("取消设置,音量只能介于[0.0]-[1.0],当前设置[" +
                 std::to_string(volume) + "]");
};

// 播放句柄
void XAudioEngin::play(int device_index, int audio_id, bool loop) {
    auto outdeviceit = outdevices.find(device_index);
    if (outdeviceit == outdevices.end()) {
        LOG_ERROR("输出设备索引[" + std::to_string(device_index) + "]无设备");
        return;
    }
    auto audioit = audios.find(audio_id);
    if (audioit == audios.end()) {
        LOG_ERROR("句柄[" + std::to_string(audio_id) + "]不存在,播放失败");
        return;
    }
    loopflags[audio_id] = loop;
    // TODO(xiang 2024-12-16): 多设备播放同一音频可能出现问题
    auto mixer_audioit = mixer.audio_orbits.find(audio_id);
    if (mixer_audioit == mixer.audio_orbits.end()) {
        // 加入混音器
        mixer.audio_orbits.insert({audio_id, audioit->second});
        LOG_INFO("添加播放音频句柄[" + std::to_string(audio_id) + "]");
    } else {
        LOG_INFO("继续播放句柄[" + std::to_string(audio_id) + "]");
    }
    // 继续播放
    audioit->second->pauseflag = false;
};
void XAudioEngin::play(const std::string &devicename, int audio_id, bool loop) {
    auto device_indexit = outdevice_indicies.find(devicename);
    if (device_indexit == outdevice_indicies.end()) {
        LOG_ERROR("设备[" + devicename + "]不存在");
        return;
    }
    play(device_indexit->second, audio_id, loop);
};
void XAudioEngin::play(int device_index, const std::string &audioname,
                       bool loop) {
    auto handelit = handles.find(audioname);
    if (handelit == handles.end()) {
        LOG_ERROR("不存在音频[" + audioname + "]");
        return;
    }
    play(device_index, handelit->second, loop);
};
void XAudioEngin::play(const std::string &devicename,
                       const std::string &audioname, bool loop) {
    auto device_indexit = outdevice_indicies.find(devicename);
    if (device_indexit == outdevice_indicies.end()) {
        LOG_ERROR("设备[" + devicename + "]不存在");
        return;
    }
    play(device_indexit->second, audioname, loop);
};

// 暂停音频句柄
void XAudioEngin::pause(int device_index, int audio_id) {
    auto outdeviceit = outdevices.find(device_index);
    if (outdeviceit == outdevices.end()) {
        LOG_ERROR("输出设备索引[" + std::to_string(device_index) + "]无设备");
        return;
    }
    auto audioit = audios.find(audio_id);
    if (audioit == audios.end()) {
        LOG_ERROR("暂停失败,句柄[" + std::to_string(audio_id) + "]不存在");
        return;
    }
    // 暂停对应的音频
    audioit->second->pauseflag = true;
    LOG_INFO("已暂停句柄[" + std::to_string(audio_id) + "]");
};
void XAudioEngin::pause(const std::string &devicename, int audio_id) {
    auto device_indexit = outdevice_indicies.find(devicename);
    if (device_indexit == outdevice_indicies.end()) {
        LOG_ERROR("设备[" + devicename + "]不存在");
        return;
    }
    pause(device_indexit->second, audio_id);
};
void XAudioEngin::pause(int device_index, const std::string &audioname) {
    auto handelit = handles.find(audioname);
    if (handelit == handles.end()) {
        LOG_ERROR("不存在音频[" + audioname + "]");
        return;
    }
    pause(device_index, handelit->second);
};
void XAudioEngin::pause(const std::string &devicename,
                        const std::string &audioname) {
    auto device_indexit = outdevice_indicies.find(devicename);
    if (device_indexit == outdevice_indicies.end()) {
        LOG_ERROR("设备[" + devicename + "]不存在");
        return;
    }
    pause(device_indexit->second, audioname);
};
// 终止音频句柄
void XAudioEngin::stop(int audio_id) {
    auto loopflagit = loopflags.find(audio_id);
    if (loopflagit == loopflags.end()) {
        LOG_WARN("音频句柄[" + std::to_string(audio_id) + "]未播放");
        return;
    }
    // 移除此标识
    loopflags.erase(loopflagit);
    // TODO(xiang 2024-12-16): 从混音器移除
};
