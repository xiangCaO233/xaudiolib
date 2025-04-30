#ifndef X_AUDIO_UTILS_H
#define X_AUDIO_UTILS_H

#include <cstddef>
#include <cstdint>

namespace xutil {
inline int64_t plannerpcmpos2milliseconds(size_t plannerpcmpos,
                                          int pcmsamplerate) {
  return plannerpcmpos * 1000 / pcmsamplerate;
}
inline size_t milliseconds2plannerpcmpos(int64_t milliseconds,
                                         int pcmsamplerate) {
  return milliseconds * pcmsamplerate / 1000;
}
}  // namespace xutil

#endif  // X_AUDIO_UTILS_H
