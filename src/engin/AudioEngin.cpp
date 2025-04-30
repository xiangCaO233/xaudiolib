#include "AudioEngin.h"

#include <SDL.h>
#include <SDL_audio.h>

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "Sound.h"
#include "config/config.h"
#include "engin/mix/mixer.h"
#include "engin/util/ncm.h"
#include "log/colorful-log.h"
#include "util/utils.h"

// 引擎实现
int XAudioEngin::currentid = 0;

XAudioEngin::XAudioEngin() { XINFO("XAudioEngin初始化"); }
XAudioEngin::~XAudioEngin() { shutdown(); }

std::unique_ptr<XAudioEngin> XAudioEngin::init() {
  XINFO("初始化音频引擎");
  XTRACE("正在获取配置文件");
  std::filesystem::path config_path("./config/latest_config.json");
  if (std::filesystem::exists(config_path.parent_path())) {
    if (std::filesystem::exists(config_path)) {
      XTRACE("配置目录存在");
      // 加载配置文件
      x::Config::load();
    }
  } else {
    XWARN("配置目录不存在");
    if (std::filesystem::create_directory(config_path.parent_path())) {
      XINFO("配置目录创建成功");
    } else {
      XERROR("配置目录创建失败");
    }
  }
  auto e = std::make_unique<XAudioEngin>();
  auto sdl_init_ret = SDL_Init(SDL_INIT_AUDIO);
  if (sdl_init_ret < 0) {
    XERROR("sdl初始化失败");
    return nullptr;
  }
  auto inputs = SDL_GetNumAudioDevices(true);
  if (inputs < 0) {
    XERROR("获取输入设备失败");
    return nullptr;
  }
  for (int i = 0; i < inputs; i++) {
    auto input_device_name = SDL_GetAudioDeviceName(i, true);
    auto device_name_str = std::string(input_device_name);
    auto inputdevice = std::make_shared<XInputDevice>(i, device_name_str);
    XINFO("检测到输入设备" + device_name_str);
    e->inputdevice_indicies.try_emplace(device_name_str, i);
    e->inputdevices.try_emplace(i, inputdevice);
  }
  // 插入一个未知设备
  std::string input_device_name = "unknown input device";
  auto inputdevice = std::make_shared<XInputDevice>(-1, input_device_name);
  e->inputdevice_indicies.try_emplace(input_device_name, -1);
  e->inputdevices.try_emplace(-1, inputdevice);

  auto outputs = SDL_GetNumAudioDevices(false);
  if (outputs < 0) {
    XERROR("获取输出设备失败");
    return nullptr;
  }
  for (int i = 0; i < outputs; i++) {
    auto output_device_name = SDL_GetAudioDeviceName(i, false);
    auto device_name_str = std::string(output_device_name);
    auto outdevice = std::make_shared<XOutputDevice>(i, device_name_str);
    XINFO("检测到输出设备" + device_name_str);
    e->outdevice_indicies.try_emplace(device_name_str, i);
    e->outdevices.try_emplace(i, outdevice);
  }
  // 插入一个未知设备
  std::string output_device_name = "unknown output device";
  auto outdevice = std::make_shared<XOutputDevice>(-1, output_device_name);
  e->outdevice_indicies.try_emplace(output_device_name, -1);
  e->outdevices.try_emplace(-1, outdevice);

  // 插入一个未知音频
  const auto unknown_sound =
      std::make_shared<XSound>(-1, "unknown sound", "unknown path", nullptr);
  e->handles.try_emplace("unknown sound", -1);
  e->audios.try_emplace(-1, unknown_sound);

  return e;
}

void XAudioEngin::shutdown() {
  SDL_Quit();
  x::Config::save();
}

int XAudioEngin::load(const std::string &audio,
                      std::string &loaded_audio_name) {
  std::filesystem::path path(audio);
  auto extension = path.extension().string();
  if (extension == ".ncm") {
    // 调用ncmdump
    std::string desdirpath;
    xutil::convert_music(audio, desdirpath);
    // 取最后一个音频输出
    for (const auto &entry : std::filesystem::directory_iterator(desdirpath)) {
      // 更新路径
      path = entry.path();
      extension = path.extension().string();
    }
  }
  // 规范化路径
  auto p = std::filesystem::weakly_canonical(std::filesystem::absolute(path))
               .string();
  auto name = path.filename().string();
  loaded_audio_name = name;
  auto handelit = handles.find(p);
  if (handelit != handles.end()) {
    XWARN("载入音频[" + audio + "]失败,音频已载入过,句柄[" +
          std::to_string(handelit->second) + "]");
    return handelit->second;
  } else {
    XINFO("正在打开音频[" + audio + "]");
    // 添加句柄
    AVFormatContext *format = nullptr;
    if (avformat_open_input(&format, path.string().c_str(), nullptr, nullptr) >=
        0) {
      XINFO("打开[" + audio + "]成功,句柄[" + std::to_string(currentid) + "]");
      // 初始化
      auto audioformat = format;

      auto sound = std::make_shared<XSound>(currentid, name, p, audioformat);
      audios.try_emplace(currentid, sound);
      handles[p] = currentid;
      // 获取流信息
      if (avformat_find_stream_info(format, nullptr) < 0) {
        XWARN("获取流信息失败");
        // 清理前面塞入的数据
        handelit = handles.find(p);
        handles.erase(handelit);
        auto audioit = audios.find(currentid);
        audios.erase(audioit);
        return -1;
      }
      // 获取编解码器
      auto codecit = audio_codecs.find(extension);
      int streamindex = -1;
      if (codecit == audio_codecs.end()) {
        XWARN("未找到[" + extension + "]编解码器");
        streamindex = av_find_best_stream(format, AVMEDIA_TYPE_AUDIO, -1, -1,
                                          nullptr, -1);
        auto decoder = std::make_shared<XAudioDecoder>(
            format->streams[streamindex]->codecpar->codec_id);
        auto encoder = std::make_shared<XAudioEncoder>(
            format->streams[streamindex]->codecpar->codec_id);
        // 直接在表中分配
        audio_codecs.try_emplace(extension, decoder, encoder);
      } else {
        XINFO("找到解码器:[" + extension + "]");
      }
      if (streamindex == -1)
        streamindex = av_find_best_stream(format, AVMEDIA_TYPE_AUDIO, -1, -1,
                                          nullptr, -1);
      codecit = audio_codecs.find(extension);
      // 解码数据(直接填充到表所处内存中)
      if (codecit->second.first->decode_audio_planner(format, streamindex,
                                                      sound->pcm) >= 0) {
        XINFO("解码[" + sound->name + "]成功");
        XINFO("音频数据大小:[" +
              std::to_string(sound->get_pcm_data_size() * 4) + "]");
      } else {
        XERROR("解码出现问题");
        handelit = handles.find(p);
        handles.erase(handelit);
        auto audioit = audios.find(currentid);
        audios.erase(audioit);
        audio_codecs.erase(codecit);
        return -1;
      }
    } else {
      XERROR("打开[" + audio + "]失败,请检查文件");
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
    XERROR("未加载过音频[" + audio + "]");
  }
}

void XAudioEngin::unload(int id) {
  // 使用id卸载音频
  auto audioit = audios.find(id);
  if (audioit != audios.end()) {
    // 删除句柄
    auto handelit = handles.find(audioit->second->path);
    handles.erase(handelit);
    // TODO(xiang 2024-12-19): 还需要删除所有设备中的此音轨
    for (const auto &[did, device] : outdevices) {
      if (device->player) {
        device->player->mixer->remove_orbit(audios[id]);
      }
    }
    XINFO("已卸载[" + audioit->second->name + "],句柄:[" + std::to_string(id) +
          "]");
    // 删除音频
    audios.erase(audioit);
  } else {
    XWARN("音频句柄[" + std::to_string(id) + "]不存在");
  }
}

// 检查音频是否加载
bool XAudioEngin::is_audio_loaded(const std::string &audioname, int &handle) {
  auto handleit = handles.find(audioname);
  if (handleit == handles.end()) {
    XWARN("不存在音频[" + audioname + "]");
    handle = -1;
    return false;
  }
  handle = handleit->second;
  return true;
}
bool XAudioEngin::is_audio_loaded(int audio_handle,
                                  std::shared_ptr<XSound> &res) {
  auto audioit = audios.find(audio_handle);
  if (audio_handle == -1 || audioit == audios.end()) {
    XWARN("不存在音频句柄[" + std::to_string(audio_handle) + "]");
    res = audios.at(-1);
    return false;
  }
  res = audioit->second;
  return true;
}

// 检查设备是否存在
bool XAudioEngin::is_indevice_exist(const std::string &devicename,
                                    int &device_index) {
  auto deviceindexit = inputdevice_indicies.find(devicename);
  if (deviceindexit == inputdevice_indicies.end()) {
    XWARN("输入设备[" + devicename + "]不存在");
    device_index = -1;
    return false;
  }
  device_index = deviceindexit->second;
  return true;
}
bool XAudioEngin::is_indevice_exist(int device_index,
                                    std::shared_ptr<XInputDevice> &res) {
  auto deviceit = inputdevices.find(device_index);
  if (device_index == -1 || deviceit == inputdevices.end()) {
    XWARN("输入设备索引[" + std::to_string(device_index) + "]不存在");
    res = inputdevices.at(-1);
    return false;
  }
  res = deviceit->second;
  return true;
}

bool XAudioEngin::is_outdevice_exist(const std::string &devicename,
                                     int &device_index) {
  auto deviceindexit = outdevice_indicies.find(devicename);
  if (deviceindexit == outdevice_indicies.end()) {
    XWARN("输出设备[" + devicename + "]不存在");
    device_index = -1;
    return false;
  }
  device_index = deviceindexit->second;
  return true;
}
bool XAudioEngin::is_outdevice_exist(int device_index,
                                     std::shared_ptr<XOutputDevice> &res) {
  auto deviceit = outdevices.find(device_index);
  if (device_index == -1 || deviceit == outdevices.end()) {
    XWARN("输出设备索引[" + std::to_string(device_index) + "]不存在");
    res = outdevices.at(-1);
    return false;
  }
  res = deviceit->second;
  return true;
}

// 获取音频名
const std::string &XAudioEngin::audio_name(int id) {
  std::shared_ptr<XSound> res;
  is_audio_loaded(id, res);
  return res->name;
}

// 获取音频路径
const std::string &XAudioEngin::audio_path(int id) {
  std::shared_ptr<XSound> res;
  is_audio_loaded(id, res);
  return res->path;
}

// 获取设备名
const std::string &XAudioEngin::input_device_name(int id) {
  std::shared_ptr<XInputDevice> res;
  is_indevice_exist(id, res);
  return res->device_name;
}
const std::string &XAudioEngin::output_device_name(int id) {
  std::shared_ptr<XOutputDevice> res;
  is_outdevice_exist(id, res);
  return res->device_name;
}

// 获得音频句柄
int XAudioEngin::audio_handle(const std::string &name) {
  int res;
  is_audio_loaded(name, res);
  return res;
}
// 获得设备索引
int XAudioEngin::input_device_index(const std::string &name) {
  int res;
  is_indevice_exist(name, res);
  return res;
}
int XAudioEngin::output_device_index(const std::string &name) {
  int res;
  is_outdevice_exist(name, res);
  return res;
}

// 获取音频音量
float XAudioEngin::volume(int deviceid, int id) {
  std::shared_ptr<XOutputDevice> outdevice;
  if (!is_outdevice_exist(deviceid, outdevice)) {
    return -1.0f;
  }
  std::shared_ptr<XSound> audio;
  if (!is_audio_loaded(id, audio)) {
    return -1.0f;
  }
  // 检查播放器
  const auto &player = outdevice->player;
  if (!player) {
    XWARN("设备[" + std::to_string(deviceid) + ":" + outdevice->device_name +
          "]还没有初始化播放器");
    return -1.0f;
  }
  // 检查混音器
  const auto &mixer = player->mixer;
  // auto orbitit = mixer->audio_orbits.find(id);
  // if (orbitit == mixer->audio_orbits.end()) {
  //   XWARN("此设备上不存在音轨句柄[" + std::to_string(id) + "]");
  //   return -1.0f;
  // }
  // return orbitit->second->volume;
  return 1.0f;
};

// 设置音频音量
void XAudioEngin::setVolume(int deviceid, int id, float v) {
  std::shared_ptr<XOutputDevice> outdevice;
  if (!is_outdevice_exist(deviceid, outdevice)) {
    return;
  }
  std::shared_ptr<XSound> audio;
  if (!is_audio_loaded(id, audio)) {
    return;
  }
  // 检查播放器
  const auto &player = outdevice->player;
  if (!player) {
    XWARN("设备[" + std::to_string(deviceid) + ":" + outdevice->device_name +
          "]还没有初始化播放器");
    return;
  }
  const auto &mixer = player->mixer;
  // 检查混音器音轨
  // auto orbitit = mixer->audio_orbits.find(id);
  // if (orbitit == mixer->audio_orbits.end()) {
  //   XWARN("此设备上不存在音轨句柄[" + std::to_string(id) + "]");
  //   return;
  // }
  // if (v >= 0 && v <= 1.0f) {
  //   orbitit->second->volume = v;
  // } else {
  //   XWARN("设置音量失败,请检查音量值[" + std::to_string(v) + "]");
  // }
}

// 设置全局音量
void XAudioEngin::setGlobalVolume(float volume) {
  if (volume >= 0 && volume <= 1.0f)
    gVolume = volume;
  else
    XWARN("取消设置,音量只能介于[0.0]-[1.0]之间,当前设置[" +
          std::to_string(volume) + "]");
}

// 设置音频当前播放到的位置
void XAudioEngin::pos(int deviceid, int id, int64_t time) {
  std::shared_ptr<XOutputDevice> outdevice;
  if (!is_outdevice_exist(deviceid, outdevice)) {
    return;
  }
  std::shared_ptr<XSound> audio;
  if (!is_audio_loaded(id, audio)) {
    return;
  }
  // 检查播放器
  const auto &player = outdevice->player;
  if (!player) {
    XWARN("设备[" + std::to_string(deviceid) + ":" + outdevice->device_name +
          "]还没有初始化播放器");
    return;
  }
  const auto &mixer = player->mixer;
  // 检查混音器音轨
  // auto orbitit = mixer->audio_orbits.find(id);
  // if (orbitit == mixer->audio_orbits.end()) {
  //   XWARN("设备[" + std::to_string(deviceid) + ":" + outdevice->device_name +
  //         "]上不存在音轨句柄[" + std::to_string(id) + "]");
  //   return;
  // }
  // auto pos = xutil::milliseconds2plannerpcmpos(
  //     time, static_cast<int>(x::Config::samplerate));
  // orbitit->second->playpos = pos;
  // XINFO("跳转[" + std::to_string(deviceid) + ":" + outdevice->device_name +
  //       "]设备上音频句柄[" + std::to_string(id) + "]到位置:[" +
  //       std::to_string(pos) + "]");
}

// 播放句柄
void XAudioEngin::play(int device_index, int audio_id, bool loop) {
  std::shared_ptr<XOutputDevice> outdevice;
  if (!is_outdevice_exist(device_index, outdevice)) {
    return;
  }
  std::shared_ptr<XSound> audio;
  if (!is_audio_loaded(audio_id, audio)) {
    return;
  }
  // 检查播放器
  const auto &player = outdevice->player;
  if (!player) {
    // 不存在此设备的播放器
    XWARN("未找到输出设备[" + std::to_string(device_index) + ":" +
          outdevice->device_name + "]上的播放器");
    XINFO("正在创建播放器");
    // 初始化播放器并加入播放器表
    if (outdevice->creat_player()) {
      XINFO("成功创建输出设备[" + std::to_string(device_index) + ":" +
            outdevice->device_name + "]的播放器");
    } else {
      XERROR("创建播放器出错");
      return;
    }
    // 设置全局音量
    player->set_player_volume(gVolume);
    // 启动播放器
    player->start();
  } else {
    XINFO("已找到输出设备[" + std::to_string(device_index) + ":" +
          outdevice->device_name + "]上的播放器");
  }
  if (!player->running) {
    player->start();
    XWARN("播放器为关闭状态,已重新启动");
  }
  // 找播放器绑定的混音器中是否存在此音频
  const auto &mixer = player->mixer;
  auto orbitsit = mixer->audio_orbits.find(audios[audio_id]);
  if (orbitsit == mixer->audio_orbits.end()) {
    XWARN("[" + std::to_string(device_index) + ":" + outdevice->device_name +
          "]设备音轨中不存在音源[" + std::to_string(audio_id) + "]");
    // 不存在
    // 加入此音频
    auto orbit = std::make_shared<XAudioOrbit>(audio);
    mixer->add_orbit(orbit);
    orbitsit = mixer->audio_orbits.find(audios[audio_id]);
    XINFO("已添加播放音频句柄[" + std::to_string(audio_id) + ":" + audio->name +
          "]到[" + std::to_string(device_index) + ":" + outdevice->device_name +
          "]音轨");
  } else {
    for (const auto &orbit : orbitsit->second) {
      if (orbit->paused) {
        // 存在
        XWARN("检测到音频暂停,继续播放句柄[" + std::to_string(audio_id) + "]");
        // 更新音频暂停标识
        orbit->paused = false;
      } else {
        XWARN("轨道已正在播放中");
      }
    }
  }
  if (player->paused) {
    XWARN("检测到[" + std::to_string(device_index) + ":" +
          outdevice->device_name + "]设备播放器暂停,继续播放");
    player->resume();
  }

  for (const auto &orbit : orbitsit->second) {
    // 更新循环标识
    orbit->loop = loop;
  }
}

// 暂停音频句柄
void XAudioEngin::pause(int device_index, int audio_id) {
  std::shared_ptr<XOutputDevice> outdevice;
  if (!is_outdevice_exist(device_index, outdevice)) {
    return;
  }
  std::shared_ptr<XSound> audio;
  if (!is_audio_loaded(audio_id, audio)) {
    return;
  }
  // 检查播放器
  const auto &player = outdevice->player;
  if (!player) {
    XWARN("设备[" + std::to_string(device_index) + ":" +
          outdevice->device_name + "]还没有初始化播放器");
    return;
  }
  const auto &mixer = player->mixer;
  auto orbitsit = mixer->audio_orbits.find(audios[audio_id]);
  if (orbitsit == mixer->audio_orbits.end()) {
    XWARN("暂停失败,设备[" + std::to_string(device_index) + "]不存在音源[" +
          orbitsit->first->name + "]");
    return;
  }
  // 暂停对应的音源
  for (auto &orbit : orbitsit->second) {
    orbit->paused = true;
  }
  XINFO("已暂停设备[" + std::to_string(device_index) + ":" +
        outdevice->device_name + "]上的音源[" + orbitsit->first->name + "]");
}

// 恢复
void XAudioEngin::resume(int device_index, int audio_id) {
  std::shared_ptr<XOutputDevice> outdevice;
  if (!is_outdevice_exist(device_index, outdevice)) {
    return;
  }
  std::shared_ptr<XSound> audio;
  if (!is_audio_loaded(audio_id, audio)) {
    return;
  }
  // 检查播放器
  const auto &player = outdevice->player;
  if (!player) {
    XWARN("设备[" + std::to_string(device_index) + ":" +
          outdevice->device_name + "]还没有初始化播放器");
    return;
  }
  const auto &mixer = player->mixer;
  auto orbitsit = mixer->audio_orbits.find(audios[audio_id]);
  if (orbitsit == mixer->audio_orbits.end()) {
    XWARN("暂停失败,设备[" + std::to_string(device_index) + "]不存在音源[" +
          orbitsit->first->name + "]");
    return;
  }
  // 恢复对应的音频
  for (auto &orbit : orbitsit->second) {
    orbit->paused = false;
  }
  // 检查播放器状态
  if (!player->running) {
    player->start();
    XWARN("[" + std::to_string(device_index) + ":" + outdevice->device_name +
          "]上的播放器为关闭状态,已重新启动");
  }
  if (player->paused) {
    XINFO("已恢复[" + std::to_string(device_index) + ":" +
          outdevice->device_name + "]上的播放器");
    player->resume();
  }
  XINFO("已恢复音频句柄[" + std::to_string(audio_id) + "]");
}

// 播放或暂停全部此设备
void XAudioEngin::resume(int device_id) {}
void XAudioEngin::pause(int device_id) {}

// 终止音频
void XAudioEngin::stop(int device_index, int audio_id) {
  std::shared_ptr<XOutputDevice> outdevice;
  if (!is_outdevice_exist(device_index, outdevice)) {
    return;
  }
  std::shared_ptr<XSound> audio;
  if (!is_audio_loaded(audio_id, audio)) {
    return;
  }
  // 检查播放器
  const auto &player = outdevice->player;
  if (!player) {
    XWARN("设备[" + std::to_string(device_index) + ":" +
          outdevice->device_name + "]还没有初始化播放器");
    return;
  }
  const auto &mixer = player->mixer;
  // 移除此音轨
  if (!mixer->remove_orbit(audio)) {
    XERROR("移除音轨失败");
    return;
  }
  XINFO("已移除[" + std::to_string(device_index) + ":" +
        outdevice->device_name + "]设备上的音频句柄[" +
        std::to_string(audio_id) + "]");
}

// 获取设备播放器状态
bool XAudioEngin::is_pause(int device_id) {
  std::shared_ptr<XOutputDevice> outdevice;
  if (!is_outdevice_exist(device_id, outdevice)) {
    return false;
  }
  // 检查播放器
  const auto &player = outdevice->player;
  if (!player) {
    XWARN("设备[" + std::to_string(device_id) + ":" + outdevice->device_name +
          "]还没有初始化播放器");
    return false;
  }
  return player->paused;
}

// 播放暂停停止设备上的播放器
void XAudioEngin::pause_device(int device_id) {
  std::shared_ptr<XOutputDevice> outdevice;
  if (!is_outdevice_exist(device_id, outdevice)) {
    return;
  }
  // 检查播放器
  const auto &player = outdevice->player;
  if (!player) {
    XWARN("设备[" + std::to_string(device_id) + ":" + outdevice->device_name +
          "]还没有初始化播放器");
    return;
  }
  // 暂停此设备上的播放器
  player->pause();
  XINFO("已暂停设备[" + std::to_string(device_id) + ":" +
        outdevice->device_name + "]的播放器");
}

void XAudioEngin::resume_device(int device_id) {
  std::shared_ptr<XOutputDevice> outdevice;
  if (!is_outdevice_exist(device_id, outdevice)) {
    return;
  }
  // 检查播放器
  const auto &player = outdevice->player;
  if (!player) {
    XWARN("设备[" + std::to_string(device_id) + ":" + outdevice->device_name +
          "]还没有初始化播放器");
    return;
  }
  // 恢复此设备上的播放器
  player->resume();
  XINFO("已恢复设备[" + std::to_string(device_id) + ":" +
        outdevice->device_name + "]的播放器");
}

void XAudioEngin::stop_player(int device_id) {
  std::shared_ptr<XOutputDevice> outdevice;
  if (!is_outdevice_exist(device_id, outdevice)) {
    return;
  }
  // 检查播放器
  const auto &player = outdevice->player;
  if (!player) {
    XWARN("设备[" + std::to_string(device_id) + ":" + outdevice->device_name +
          "]还没有初始化播放器");
    return;
  }
  // 停止此设备上的播放器
  player->stop();
  XINFO("已停止设备[" + std::to_string(device_id) + ":" +
        outdevice->device_name + "]的播放器");
}
