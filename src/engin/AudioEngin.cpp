#include "AudioEngin.h"

#include <SDL.h>
#include <SDL_audio.h>

#include <filesystem>
#include <memory>
#include <string>

#include "config/config.h"
#include "engin/mix/mixer.h"
#include "engin/util/ncm.h"
#include "logger/logger.h"
#include "util//utils.h"

// 引擎实现
int XAudioEngin::currentid = 0;

XAudioEngin::XAudioEngin() { LOG_TRACE("XAudioEngin初始化"); }

XAudioEngin::~XAudioEngin() {
    shutdown();
    LOG_TRACE("引擎已关闭");
}

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
    if (extension == ".ncm") {
        // 调用ncmdump
        std::string desdirpath;
        xutil::convert_music(audio, desdirpath);
        // 取最后一个音频输出
        for (const auto &entry :
             std::filesystem::directory_iterator(desdirpath)) {
            // 更新路径
            path = entry.path();
            extension = path.extension().string();
        }
    }
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
        if (avformat_open_input(&format, path.string().c_str(), nullptr,
                                nullptr) >= 0) {
            LOG_INFO("打开[" + audio + "]成功,句柄[" +
                     std::to_string(currentid) + "]");
            // 初始化
            // 包装为智能指针
            auto audioformat = std::shared_ptr<AVFormatContext>(format);

            auto sound =
                std::make_shared<XSound>(currentid, name, p, audioformat);
            audios.insert({currentid, sound});
            handles[name] = currentid;
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
            // 获取编解码器
            auto codecit = audio_codecs.find(extension);
            int streamindex = -1;
            if (codecit == audio_codecs.end()) {
                LOG_WARN("未找到[" + extension + "]编解码器");
                streamindex = av_find_best_stream(format, AVMEDIA_TYPE_AUDIO,
                                                  -1, -1, nullptr, -1);
                auto decoder = std::make_shared<XAudioDecoder>(
                    format->streams[streamindex]->codecpar->codec_id);
                auto encoder = std::make_shared<XAudioEncoder>(
                    format->streams[streamindex]->codecpar->codec_id);
                // 直接在表中分配
                audio_codecs.insert({extension, {decoder, encoder}});
            } else {
                LOG_INFO("找到解码器:[" + extension + "]");
            }
            if (streamindex == -1)
                streamindex = av_find_best_stream(format, AVMEDIA_TYPE_AUDIO,
                                                  -1, -1, nullptr, -1);
            codecit = audio_codecs.find(extension);
            // 解码数据(直接填充到表所处内存中)
            if (codecit->second.first->decode_audio(
                    sound->audio_format, streamindex, sound->pcm_data) >= 0) {
                LOG_INFO("解码[" + sound->name + "]成功");
                LOG_INFO("音频数据大小:[" +
                         std::to_string(sound->pcm_data.size()) + "]");
            } else {
                LOG_ERROR("解码出现问题");
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
}
void XAudioEngin::unload(const std::string &audio) {
    // 卸载音频
    auto handelit = handles.find(audio);
    if (handelit != handles.end()) {
        unload(handelit->second);
    } else {
        LOG_WARN("未加载过音频[" + audio + "]");
    }
}

void XAudioEngin::unload(int id) {
    // 使用id卸载音频
    auto audioit = audios.find(id);
    if (audioit != audios.end()) {
        // 删除句柄
        auto handelit = handles.find(audioit->second->name);
        handles.erase(handelit);
        // TODO(xiang 2024-12-19): 还需要删除所有设备中的此音轨
        LOG_INFO("已删除[" + audioit->second->name + "],句柄:[" +
                 std::to_string(id) + "]");
        // 删除音频
        audios.erase(audioit);
    } else {
        LOG_WARN("音频句柄[" + std::to_string(id) + "]不存在");
    }
}
// 获取音频名
const std::string &XAudioEngin::name(int id) {
    auto it = audios.find(id);
    if (it != audios.end()) return it->second->name;
    return XSound::unknown;
}
// 获取音频路径
const std::string &XAudioEngin ::path(int id) {
    auto it = audios.find(id);
    if (it != audios.end()) return it->second->path;
    return XSound::unknown_path;
}

// 获取音频音量
float XAudioEngin::volume(int deviceid, int id) {
    auto deviceit = outdevices.find(deviceid);
    if (deviceit == outdevices.end()) {
        LOG_ERROR("设备索引[" + std::to_string(deviceid) + "]不存在");
        return -1.0f;
    }
    auto audioit = audios.find(id);
    if (audioit == audios.end()) {
        LOG_ERROR("音频句柄[" + std::to_string(id) + "]不存在");
        return -1.0f;
    }
    auto &mixer = deviceit->second->player->mixer;
    // 检查混音器音轨
    auto &prop = mixer->prop(id);
    if (!prop.sound) {
        LOG_ERROR("此设备上不存在音轨句柄[" + std::to_string(id) + "]");
        return -1.0f;
    }
    return prop.volume;
};
float XAudioEngin::volume(const std::string &devicename, int id) {
    auto deviceindexit = outdevice_indicies.find(devicename);
    if (deviceindexit == outdevice_indicies.end()) {
        LOG_ERROR("设备[" + devicename + "]不存在");
        return -1.0f;
    }
    return volume(deviceindexit->second, id);
}
float XAudioEngin::volume(int deviceid, const std::string &audioname) {
    auto handelit = handles.find(audioname);
    if (handelit == handles.end()) {
        LOG_ERROR("不存在音频[" + audioname + "]");
        return -1.0f;
    }
    return volume(deviceid, handelit->second);
}
float XAudioEngin::volume(const std::string &devicename,
                          const std::string &audio) {
    auto deviceindexit = outdevice_indicies.find(devicename);
    if (deviceindexit == outdevice_indicies.end()) {
        LOG_ERROR("设备[" + devicename + "]不存在");
        return -1.0f;
    }
    return volume(deviceindexit->second, audio);
}

// 设置音频音量
void XAudioEngin::setVolume(int deviceid, int id, float v) {
    auto deviceit = outdevices.find(deviceid);
    if (deviceit == outdevices.end()) {
        LOG_ERROR("设备索引[" + std::to_string(deviceid) + "]不存在");
        return;
    }
    auto audioit = audios.find(id);
    if (audioit == audios.end()) {
        LOG_ERROR("音频句柄[" + std::to_string(id) + "]不存在");
        return;
    }
    auto &mixer = deviceit->second->player->mixer;
    // 检查混音器音轨
    auto &prop = mixer->prop(id);
    if (!prop.sound) {
        LOG_ERROR("此设备上不存在音轨句柄[" + std::to_string(id) + "]");
        return;
    }
    if (v >= 0 && v <= 1.0f) {
        prop.volume = v;
    } else {
        LOG_ERROR("设置音量失败,请检查音量值[" + std::to_string(v) + "]");
    }
}
void XAudioEngin::setVolume(int deviceid, const std::string &audioname,
                            float v) {
    auto handelit = handles.find(audioname);
    if (handelit == handles.end()) {
        LOG_ERROR("不存在音频[" + audioname + "]");
        return;
    }
    setVolume(deviceid, handelit->second, v);
}
void XAudioEngin::setVolume(const std::string &devicename, int id, float v) {
    auto device_indexit = outdevice_indicies.find(devicename);
    if (device_indexit == outdevice_indicies.end()) {
        LOG_ERROR("设备[" + devicename + "]不存在");
        return;
    }
    setVolume(device_indexit->second, id, v);
}
void XAudioEngin::setVolume(const std::string &devicename,
                            const std::string &audio, float v) {
    auto device_indexit = outdevice_indicies.find(devicename);
    if (device_indexit == outdevice_indicies.end()) {
        LOG_ERROR("设备[" + devicename + "]不存在");
        return;
    }
    setVolume(device_indexit->second, audio, v);
}

// 设置全局音量
void XAudioEngin::setGlobalVolume(float volume) {
    if (volume >= 0 && volume <= 1.0f)
        gVolume = volume;
    else
        LOG_WARN("取消设置,音量只能介于[0.0]-[1.0]之间,当前设置[" +
                 std::to_string(volume) + "]");
}

// 设置音频当前播放到的位置
void XAudioEngin::pos(int deviceid, int id, int64_t time) {
    auto deviceit = outdevices.find(deviceid);
    if (deviceit == outdevices.end()) {
        LOG_ERROR("设备索引[" + std::to_string(deviceid) + "]不存在");
        return;
    }
    auto audioit = audios.find(id);
    if (audioit == audios.end()) {
        LOG_ERROR("音频句柄[" + std::to_string(id) + "]不存在");
        return;
    }
    auto &outdevice = deviceit->second;
    auto &mixer = outdevice->player->mixer;
    // 检查混音器音轨
    auto &prop = mixer->prop(id);
    if (!prop.sound) {
        LOG_ERROR("设备[" + std::to_string(deviceid) + ":" +
                  outdevice->device_name + "]上不存在音轨句柄[" +
                  std::to_string(id) + "]");
        return;
    }
    auto pos =
        xutil::milliseconds2pcmpos(time, Config::samplerate, Config::channel);
    prop.playpos = pos;
    LOG_DEBUG("跳转[" + std::to_string(deviceid) + ":" +
              outdevice->device_name + "]设备上音频句柄[" + std::to_string(id) +
              "]到位置:[" + std::to_string(pos) + "]");
}
void XAudioEngin::pos(int deviceid, const std::string &audio, int64_t time) {
    auto handelit = handles.find(audio);
    if (handelit == handles.end()) {
        LOG_ERROR("不存在音频[" + audio + "]");
        return;
    }
    pos(deviceid, handelit->second, time);
};
void XAudioEngin::pos(const std::string &devicename, int id, int64_t time) {
    auto device_indexit = outdevice_indicies.find(devicename);
    if (device_indexit == outdevice_indicies.end()) {
        LOG_ERROR("设备[" + devicename + "]不存在");
        return;
    }
    pos(device_indexit->second, id, time);
};
void XAudioEngin::pos(const std::string &devicename,
                      const std::string &audioname, int64_t time) {
    auto device_indexit = outdevice_indicies.find(devicename);
    if (device_indexit == outdevice_indicies.end()) {
        LOG_ERROR("设备[" + devicename + "]不存在");
        return;
    }
    pos(device_indexit->second, audioname, time);
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
    auto &outdevice = outdeviceit->second;
    // 检查此设备对应的播放器
    auto &player = outdevice->player;
    if (!player) {
        // 不存在此设备的播放器
        // 初始化播放器并加入播放器表
        if (outdevice->creat_player()) {
            LOG_INFO("成功创建输出设备[" + std::to_string(device_index) + ":" +
                     outdevice->device_name + "]的播放器");
        } else {
            LOG_ERROR("创建播放器出错");
            return;
        }
        // 设置全局音量
        player->set_player_volume(gVolume);
        // 启动播放器
        player->start();
    } else {
        LOG_INFO("已找到输出设备[" + std::to_string(device_index) + ":" +
                 outdevice->device_name + "]上的播放器");
    }
    if (!player->running) {
        player->start();
        LOG_WARN("播放器为关闭状态,已重新启动");
    }
    // 找播放器绑定的混音器中是否存在此音频
    auto &mixer = player->mixer;
    auto &prop = mixer->prop(audio_id);
    if (!prop.sound) {
        LOG_INFO("[" + std::to_string(device_index) + ":" +
                 outdevice->device_name + "]设备音轨中不存在音频[" +
                 std::to_string(audio_id) + "]");
        // 不存在
        // 加入此音频
        mixer->add_orbit(audioit->second);
        LOG_INFO("已添加播放音频句柄[" + std::to_string(audio_id) + "]到[" +
                 std::to_string(device_index) + ":" + outdevice->device_name +
                 "]音轨");
    } else {
        if (prop.paused) {
            // 存在
            LOG_INFO("检测到音频暂停,继续播放句柄[" + std::to_string(audio_id) +
                     "]");
            // 更新音频暂停标识
            prop.paused = false;
        } else {
            LOG_WARN("音频已正在播放中");
        }
    }
    if (player->paused) {
        LOG_INFO("检测到[" + std::to_string(device_index) + ":" +
                 outdevice->device_name + "]设备播放器暂停,继续播放");
        player->resume();
    }
    // 更新循环标识([]运算符自动添加)
    prop.loop = loop;
}

void XAudioEngin::play(const std::string &devicename, int audio_id, bool loop) {
    auto device_indexit = outdevice_indicies.find(devicename);
    if (device_indexit == outdevice_indicies.end()) {
        LOG_ERROR("设备[" + devicename + "]不存在");
        return;
    }
    play(device_indexit->second, audio_id, loop);
}
void XAudioEngin::play(int device_index, const std::string &audioname,
                       bool loop) {
    auto handelit = handles.find(audioname);
    if (handelit == handles.end()) {
        LOG_ERROR("不存在音频[" + audioname + "]");
        return;
    }
    play(device_index, handelit->second, loop);
}
void XAudioEngin::play(const std::string &devicename,
                       const std::string &audioname, bool loop) {
    auto device_indexit = outdevice_indicies.find(devicename);
    if (device_indexit == outdevice_indicies.end()) {
        LOG_ERROR("设备[" + devicename + "]不存在");
        return;
    }
    play(device_indexit->second, audioname, loop);
}

// 暂停音频句柄
void XAudioEngin::pause(int device_index, int audio_id) {
    auto outdeviceit = outdevices.find(device_index);
    if (outdeviceit == outdevices.end()) {
        LOG_ERROR("输出设备索引[" + std::to_string(device_index) + "]无设备");
        return;
    }
    auto &outdevice = outdeviceit->second;
    auto &prop = outdevice->player->mixer->prop(audio_id);
    if (!prop.sound) {
        LOG_ERROR("暂停失败,设备[" + std::to_string(device_index) +
                  "]不存在句柄[" + std::to_string(audio_id) + "]");
        return;
    }
    // 暂停对应的音频
    prop.paused = true;
    LOG_INFO("已暂停设备[" + std::to_string(device_index) + ":" +
             outdevice->device_name + "]上的句柄[" + std::to_string(audio_id) +
             "]");
}
void XAudioEngin::pause(const std::string &devicename, int audio_id) {
    auto device_indexit = outdevice_indicies.find(devicename);
    if (device_indexit == outdevice_indicies.end()) {
        LOG_ERROR("设备[" + devicename + "]不存在");
        return;
    }
    pause(device_indexit->second, audio_id);
}
void XAudioEngin::pause(int device_index, const std::string &audioname) {
    auto handelit = handles.find(audioname);
    if (handelit == handles.end()) {
        LOG_ERROR("不存在音频[" + audioname + "]");
        return;
    }
    pause(device_index, handelit->second);
}
void XAudioEngin::pause(const std::string &devicename,
                        const std::string &audioname) {
    auto device_indexit = outdevice_indicies.find(devicename);
    if (device_indexit == outdevice_indicies.end()) {
        LOG_ERROR("设备[" + devicename + "]不存在");
        return;
    }
    pause(device_indexit->second, audioname);
}

// 恢复
void XAudioEngin::resume(int device_index, int audio_id) {
    auto outdeviceit = outdevices.find(device_index);
    if (outdeviceit == outdevices.end()) {
        LOG_ERROR("输出设备索引[" + std::to_string(device_index) + "]无设备");
        return;
    }
    auto &outdevice = outdeviceit->second;
    auto &prop = outdevice->player->mixer->prop(audio_id);
    if (!prop.sound) {
        LOG_ERROR("设备[" + std::to_string(device_index) + ":" +
                  outdevice->device_name + "]不存在此音频轨道");
        return;
    }
    // 恢复对应的音频
    prop.paused = false;
    if (outdeviceit->second->player) {
        if (outdeviceit->second->player->paused) {
            LOG_INFO("已恢复[" + std::to_string(device_index) + ":" +
                     outdevice->device_name + "]上的播放器");
            outdeviceit->second->player->resume();
        }
    }
    LOG_INFO("已恢复音频句柄[" + std::to_string(audio_id) + "]");
}
void XAudioEngin::resume(const std::string &devicename, int audio_id) {
    auto device_indexit = outdevice_indicies.find(devicename);
    if (device_indexit == outdevice_indicies.end()) {
        LOG_ERROR("设备[" + devicename + "]不存在");
        return;
    }
    resume(device_indexit->second, audio_id);
}
void XAudioEngin::resume(int device_index, const std::string &audioname) {
    auto handelit = handles.find(audioname);
    if (handelit == handles.end()) {
        LOG_ERROR("不存在音频[" + audioname + "]");
        return;
    }
    resume(device_index, handelit->second);
}
void XAudioEngin::resume(const std::string &devicename,
                         const std::string &audioname) {
    auto device_indexit = outdevice_indicies.find(devicename);
    if (device_indexit == outdevice_indicies.end()) {
        LOG_ERROR("设备[" + devicename + "]不存在");
        return;
    }
    resume(device_indexit->second, audioname);
}

// 终止音频
void XAudioEngin::stop(int device_index, int audio_id) {
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
    auto &outdevice = outdeviceit->second;
    if (outdevice->player) {
        auto &mixer = outdevice->player->mixer;
        // 移除此音轨
        if (mixer->remove_orbit(audioit->second)) {
            LOG_INFO("已移除[" + std::to_string(device_index) + ":" +
                     outdevice->device_name + "]设备上的音频句柄[" +
                     std::to_string(audio_id) + "]");
        } else {
            LOG_WARN("移除音轨失败");
        }
    } else {
        LOG_WARN("设备[" + std::to_string(device_index) + ":" +
                 outdevice->device_name + "]上还没有启动播放器");
    }
}
void XAudioEngin::stop(const std::string &devicename, int audio_id) {
    auto outdeviceindexit = outdevice_indicies.find(devicename);
    if (outdeviceindexit == outdevice_indicies.end()) {
        LOG_ERROR("不存在设备[" + devicename + "]");
        return;
    }
    stop(outdeviceindexit->second, audio_id);
}
void XAudioEngin::stop(int device_index, const std::string &audioname) {
    auto handelit = handles.find(audioname);
    if (handelit == handles.end()) {
        LOG_ERROR("不存在音频[" + audioname + "]");
        return;
    }
    stop(device_index, handelit->second);
}
void XAudioEngin::stop(const std::string &devicename,
                       const std::string &audioname) {
    auto outdeviceindexit = outdevice_indicies.find(devicename);
    if (outdeviceindexit == outdevice_indicies.end()) {
        LOG_ERROR("不存在设备[" + devicename + "]");
        return;
    }
    stop(outdeviceindexit->second, audioname);
}

// 获取设备播放器状态
bool XAudioEngin::is_pause(int device_id) {
    auto outdeviceit = outdevices.find(device_id);
    if (outdeviceit == outdevices.end()) {
        LOG_ERROR("不存在设备索引[" + std::to_string(device_id) + "]");
        return false;
    }
    return outdeviceit->second->player->paused;
}
bool XAudioEngin::is_pause(const std::string &devicename) {
    auto outdeviceindexit = outdevice_indicies.find(devicename);
    if (outdeviceindexit == outdevice_indicies.end()) {
        LOG_ERROR("不存在设备[" + devicename + "]");
        return false;
    }
    return is_pause(outdeviceindexit->second);
}
// TODO(xiang 2024-12-19): 判断设备上音频是否暂停

// 播放暂停停止设备上的播放器
void XAudioEngin::pause_device(int device_id) {
    auto outdeviceit = outdevices.find(device_id);
    if (outdeviceit == outdevices.end()) {
        LOG_ERROR("不存在设备索引[" + std::to_string(device_id) + "]");
        return;
    }
    auto &outdevice = outdeviceit->second;
    // 暂停此设备上的播放器
    outdeviceit->second->player->pause();
    LOG_INFO("已暂停设备[" + std::to_string(device_id) + ":" +
             outdevice->device_name + "]的播放器");
}
void XAudioEngin::pause_device(const std::string &devicename) {
    auto deviceindexit = outdevice_indicies.find(devicename);
    if (deviceindexit == outdevice_indicies.end()) {
        LOG_ERROR("不存在设备:[" + devicename + "]");
        return;
    }
    pause_device(deviceindexit->second);
}

void XAudioEngin::resume_device(int device_id) {
    auto outdeviceit = outdevices.find(device_id);
    if (outdeviceit == outdevices.end()) {
        LOG_ERROR("不存在设备索引[" + std::to_string(device_id) + "]");
        return;
    }
    // 恢复此设备上的播放器
    outdeviceit->second->player->resume();
}
void XAudioEngin::resume_device(const std::string &devicename) {
    auto outdeviceindexit = outdevice_indicies.find(devicename);
    if (outdeviceindexit == outdevice_indicies.end()) {
        LOG_ERROR("不存在设备[" + devicename + "]");
        return;
    }
    resume_device(outdeviceindexit->second);
}

void XAudioEngin::stop_player(int device_id) {
    auto outdeviceit = outdevices.find(device_id);
    if (outdeviceit == outdevices.end()) {
        LOG_ERROR("不存在设备索引[" + std::to_string(device_id) + "]");
        return;
    }
    if (!outdeviceit->second->player) {
        LOG_WARN("此设备上还没有初始化播放器");
        return;
    }
    auto &outdevice = outdeviceit->second;
    // 停止此设备上的播放器
    outdeviceit->second->player->stop();
    LOG_INFO("已停止设备[" + std::to_string(device_id) + ":" +
             outdevice->device_name + "]的播放器");
}
void XAudioEngin::stop_player(const std::string &devicename) {
    auto outdeviceindexit = outdevice_indicies.find(devicename);
    if (outdeviceindexit == outdevice_indicies.end()) {
        LOG_ERROR("不存在设备[" + devicename + "]");
        return;
    }
    stop_player(outdeviceindexit->second);
}
