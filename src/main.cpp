#include <iostream>
#if defined(__APPLE__)
#include <AudioToolbox/AudioToolbox.h>
#include <CoreAudio/CoreAudio.h>
#elif defined(__linux__)
#elif defined(_WIN32)
#elif defined(_WIN64)
#elif defined(__ANDROID__)
#else
#endif

int main(int argc, char *argv[]) {
    std::cout << "xaudios!" << std::endl;
    AudioStreamBasicDescription audioFormat;
    memset(&audioFormat, 0, sizeof(audioFormat));
    audioFormat.mSampleRate = 44100;
    audioFormat.mFormatID = kAudioFormatLinearPCM;
    audioFormat.mFormatFlags = kAudioFormatFlagIsSignedInteger;
    audioFormat.mBitsPerChannel = 16;
    audioFormat.mChannelsPerFrame = 2;
    audioFormat.mBytesPerPacket = 4;
    audioFormat.mFramesPerPacket = 1;
    return 0;
}
