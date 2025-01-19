cmake_minimum_required(VERSION 3.28)

project(xaudiolib LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(CODEC_SOURCES src/engin/codec/encoder.cpp src/engin/codec/decoder.cpp)
set(DEVICE_SOURCES src/engin/device/outdevice.cpp src/engin/device/indevice.cpp)
set(MIXER_SOURCES # src/gpu/gl/shader/shader.cpp
    src/engin/mix/mixer.cpp)
set(CONFIG_SOURCES src/config/config.cpp)
set(SDL_SOURCES src/engin/sdl/rbuffer.cpp src/engin/sdl/xmicrophone.cpp
                src/engin/sdl/xplayer.cpp)

set(ENGIN_SOURCES
    src/engin/AudioEngin.cpp
    src/engin/AudioManager.cpp
    src/engin/Sound.cpp
    ${CONFIG_SOURCES}
    ${CODEC_SOURCES}
    ${DEVICE_SOURCES}
    ${MIXER_SOURCES}
    ${SDL_SOURCES})

set(API_SOURCES src/api/cpp/XAudioManager.cpp)

set(LIB_SOURCES ${API_SOURCES} ${ENGIN_SOURCES})

set(SOURCES src/main.cpp ${LIB_SOURCES})

include_directories(src)

# SDL2(Simple DirectMedia Library2)
find_package(SDL2 REQUIRED)
# OpenAL(播放api)
find_package(OpenAL REQUIRED)
# OpenCL(GPU加速混音计算)
find_package(OpenCL REQUIRED)
# OpenGL(GPU使用gl渲染管线加速)
find_package(OpenGL REQUIRED)
# GLEW(opengl驱动接口)
find_package(GLEW REQUIRED)
# 包管理器
find_package(PkgConfig REQUIRED)
# ffmpeg
pkg_check_modules(
  ffmpeg
  REQUIRED
  IMPORTED_TARGET
  libavutil
  libswresample
  libavcodec
  libavformat)

# 可执行
add_executable(testxaudiolib ${SOURCES})

# 静态库
add_library(xaudio-dev STATIC ${LIB_SOURCES})

# 动态库
add_library(xaudio SHARED ${LIB_SOURCES})

# 链接其他库
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  # macos macos 查找 Core Audio 框架
  find_library(COREAUDIO_LIBRARY CoreAudio)
  find_library(AUDIOTOOLBOX_LIBRARY AudioToolbox)
  find_library(AUDIOUNIT_LIBRARY AudioUnit)

  target_link_libraries(
    testxaudiolib
    PUBLIC ${COREAUDIO_LIBRARY}
           ${AUDIOTOOLBOX_LIBRARY}
           ${AUDIOUNIT_LIBRARY}
           OpenAL::OpenAL
           OpenCL::OpenCL
           GLEW::GLEW
           OpenGL::GL
           SDL2::SDL2
           PkgConfig::ffmpeg)
  target_link_libraries(
    xaudio
    PUBLIC ${COREAUDIO_LIBRARY}
           ${AUDIOTOOLBOX_LIBRARY}
           ${AUDIOUNIT_LIBRARY}
           OpenAL::OpenAL
           OpenCL::OpenCL
           GLEW::GLEW
           OpenGL::GL
           SDL2::SDL2
           PkgConfig::ffmpeg)
  target_link_libraries(
    xaudio-dev
    PUBLIC ${COREAUDIO_LIBRARY}
           ${AUDIOTOOLBOX_LIBRARY}
           ${AUDIOUNIT_LIBRARY}
           OpenAL::OpenAL
           OpenCL::OpenCL
           GLEW::GLEW
           OpenGL::GL
           SDL2::SDL2
           PkgConfig::ffmpeg)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  # linux
  target_link_libraries(
    testxaudiolib PUBLIC OpenAL::OpenAL OpenCL::OpenCL GLEW::GLEW SDL2::SDL2
                         PkgConfig::ffmpeg)
  target_link_libraries(xaudio PUBLIC OpenAL::OpenAL OpenCL::OpenCL SDL2::SDL2
                                      GLEW::GLEW PkgConfig::ffmpeg)
  target_link_libraries(
    xaudio-dev PUBLIC OpenAL::OpenAL OpenCL::OpenCL GLEW::GLEW SDL2::SDL2
                      PkgConfig::ffmpeg)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  # windows
  target_link_libraries(
    testxaudiolib PUBLIC OpenAL::OpenAL OpenCL::OpenCL GLEW::glew SDL2::SDL2
                         PkgConfig::ffmpeg)
  target_link_libraries(xaudio PUBLIC OpenAL::OpenAL OpenCL::OpenCL GLEW::glew
                                      SDL2::SDL2 PkgConfig::ffmpeg)
  target_link_libraries(
    xaudio-dev PUBLIC OpenAL::OpenAL OpenCL::OpenCL GLEW::glew SDL2::SDL2
                      PkgConfig::ffmpeg)
else()
  # 其他系统
  message(STATUS "当前系统: ${CMAKE_SYSTEM_NAME}")
endif()
