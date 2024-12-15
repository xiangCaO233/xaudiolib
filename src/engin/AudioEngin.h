#ifndef X_AUDIO_ENGIN_H
#define X_AUDIO_ENGIN_H

class XAudioEngin {
   public:
    // 创建XAudioEngin 引擎
    XAudioEngin();
    virtual ~XAudioEngin();
    // 初始化引擎
    void init();
    // 关闭引擎
    void shutdown();
};

#endif  // X_AUDIO_ENGIN_H
