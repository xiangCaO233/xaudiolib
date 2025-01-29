#ifndef X_AUDIO_UTILS_H
#define X_AUDIO_UTILS_H

#include <cstddef>
#include <cstdint>

namespace xutil {
inline int64_t pcmpos2milliseconds(size_t pcmpos, int pcmsamplerate,
                                   int channels) {
  return pcmpos * 1000 / pcmsamplerate / channels;
}
inline size_t milliseconds2pcmpos(int64_t milliseconds, int pcmsamplerate,
                                  int channels) {
  return milliseconds * pcmsamplerate / 1000 * channels;
}
}  // namespace xutil

#endif  // X_AUDIO_UTILS_H
