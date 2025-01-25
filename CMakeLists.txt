cmake_minimum_required(VERSION 3.28)

project(xaudiolib LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(CODEC_SOURCES src/engin/codec/encoder.cpp src/engin/codec/decoder.cpp)
set(DEVICE_SOURCES src/engin/device/outdevice.cpp src/engin/device/indevice.cpp)
set(MIXER_SOURCES src/gpu/gl/shader/shader.cpp src/engin/mix/mixer.cpp)
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

set(LIB_SOURCES src/log/colorful_log.cpp ${ENGIN_SOURCES})

set(SOURCES src/main.cpp ${LIB_SOURCES})

include_directories(includes src)

# 日志
find_package(spdlog REQUIRED)
# SDL2(Simple DirectMedia Library2)
find_package(SDL2 REQUIRED)
# OpenAL(3d音效api)
find_package(OpenAL REQUIRED)
# OpenCL(GPU加速计算api)
find_package(OpenCL REQUIRED)
# OpenGL(GPU图形渲染管线)
find_package(OpenGL REQUIRED)
# glm(gl数学库)
find_package(glm REQUIRED)
# GLEW(opengl显卡接口)
find_package(GLEW REQUIRED)
# GLFW(opengl窗口api)
find_package(glfw3 REQUIRED)
# (c包管理器)
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

# 可执行程序
add_executable(testxaudiolib ${SOURCES})

# 静态库
add_library(xaudio-dev STATIC ${LIB_SOURCES})

# 动态库
add_library(xaudio SHARED ${LIB_SOURCES})

# Mac平台
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  # macos Core Audio
  find_library(COREAUDIO_LIBRARY CoreAudio)
  find_library(AUDIOTOOLBOX_LIBRARY AudioToolbox)
  find_library(AUDIOUNIT_LIBRARY AudioUnit)

  target_link_libraries(
    testxaudiolib
    PUBLIC ${COREAUDIO_LIBRARY}
           ${AUDIOTOOLBOX_LIBRARY}
           ${AUDIOUNIT_LIBRARY}
           spdlog::spdlog
           OpenAL::OpenAL
           OpenCL::OpenCL
           GLEW::GLEW
           glfw
           OpenGL::GL
           glm::glm
           SDL2::SDL2
           PkgConfig::ffmpeg)
  target_link_libraries(
    xaudio
    PUBLIC ${COREAUDIO_LIBRARY}
           ${AUDIOTOOLBOX_LIBRARY}
           ${AUDIOUNIT_LIBRARY}
           spdlog::spdlog
           OpenAL::OpenAL
           OpenCL::OpenCL
           GLEW::GLEW
           glfw
           OpenGL::GL
           glm::glm
           SDL2::SDL2
           PkgConfig::ffmpeg)
  target_link_libraries(
    xaudio-dev
    PUBLIC ${COREAUDIO_LIBRARY}
           ${AUDIOTOOLBOX_LIBRARY}
           ${AUDIOUNIT_LIBRARY}
           spdlog::spdlog
           OpenAL::OpenAL
           OpenCL::OpenCL
           GLEW::GLEW
           glfw
           OpenGL::GL
           glm::glm
           SDL2::SDL2
           PkgConfig::ffmpeg)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  target_link_libraries(
    testxaudiolib
    PUBLIC spdlog::spdlog
           OpenAL::OpenAL
           OpenCL::OpenCL
           GLEW::GLEW
           SDL2::SDL2
           glfw
           OpenGL::GL
           glm::glm
           PkgConfig::ffmpeg)
  target_link_libraries(
    xaudio
    PUBLIC spdlog::spdlog
           OpenAL::OpenAL
           OpenCL::OpenCL
           SDL2::SDL2
           glfw
           OpenGL::GL
           glm::glm
           GLEW::GLEW
           PkgConfig::ffmpeg)
  target_link_libraries(
    xaudio-dev
    PUBLIC spdlog::spdlog
           OpenAL::OpenAL
           OpenCL::OpenCL
           GLEW::GLEW
           SDL2::SDL2
           OpenGL::GL
           glm::glm
           glfw
           PkgConfig::ffmpeg)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  # windows
  target_link_libraries(
    testxaudiolib
    PUBLIC OpenAL::OpenAL
           OpenCL::OpenCL
           GLEW::glew
           SDL2::SDL2
           spdlog::spdlog
           OpenGL::GL
           glm::glm
           glfw
           PkgConfig::ffmpeg)
  target_link_libraries(
    xaudio
    PUBLIC OpenAL::OpenAL
           OpenCL::OpenCL
           GLEW::glew
           spdlog::spdlog
           OpenGL::GL
           glm::glm
           glfw
           SDL2::SDL2
           PkgConfig::ffmpeg)
  target_link_libraries(
    xaudio-dev
    PUBLIC OpenAL::OpenAL
           OpenCL::OpenCL
           GLEW::glew
           SDL2::SDL2
           OpenGL::GL
           glm::glm
           glfw
           spdlog::spdlog
           PkgConfig::ffmpeg)
else()
  # 其它系统
  message(STATUS "当前系统: ${CMAKE_SYSTEM_NAME}")
endif()
