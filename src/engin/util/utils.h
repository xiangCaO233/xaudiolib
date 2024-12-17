#ifndef X_AUDIO_UTILS_H
#define X_AUDIO_UTILS_H

#include <cstddef>
#include <cstdint>
namespace xutil {
int64_t pcmpos2milliseconds(size_t pcmpos, int pcmsamplerate);
size_t milliseconds2pcmpos(int64_t milliseconds, int pcmsamplerate) {}
}  // namespace xutil

#endif  // X_AUDIO_UTILS_H
