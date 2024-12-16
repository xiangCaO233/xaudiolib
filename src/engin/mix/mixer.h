#ifndef X_AUDIO_MIXER_H
#define X_AUDIO_MIXER_H

#include <list>

class XSound;

class XAuidoMixer {
    // 全部音轨
    std::list<XSound> audio_orbits;

   public:
    // 构造XAuidoMixer
    XAuidoMixer();
    // 析构XAuidoMixer
    virtual ~XAuidoMixer();
};

#endif  // X_AUDIO_MIXER_H
