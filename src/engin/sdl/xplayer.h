#ifndef X_AUDIO_PLAYER_H
#define X_AUDIO_PLAYER_H

#include <thread>
class XPlayer {
    // sdl播放线程
    std::thread sdl_playthread;

   public:
    // 构造XPlayer
    XPlayer();
    // 析构XPlayer
    virtual ~XPlayer();
};

#endif  // X_AUDIO_PLAYER_H
