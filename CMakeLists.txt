cmake_minimum_required(VERSION 3.28)

project(xaudiolib LANGUAGES CXX)

if(MSVC)
  add_compile_options(/utf-8 /EHsc /wd4005)
  add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
  add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
else()
  # 覆盖所有非 MSVC 编译器（包括 GCC/Clang/Intel 等）
  add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

add_definitions(-DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_VISIBILITY_PRESET default)
set(CMAKE_VISIBILITY_INLINES_HIDDEN OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CODEC_SOURCES src/engin/codec/encoder.cpp src/engin/codec/decoder.cpp)
set(DEVICE_SOURCES src/engin/device/outdevice.cpp src/engin/device/indevice.cpp)
set(MIXER_SOURCES src/gpu/gl/shader/shader.cpp src/engin/mix/mixer.cpp
  src/engin/mix/AudioOrbit.cpp)
set(CONFIG_SOURCES src/config/config.cpp)
set(SDL_SOURCES src/engin/sdl/rbuffer.cpp src/engin/sdl/xmicrophone.cpp
  src/engin/sdl/xplayer.cpp)

set(ENGIN_SOURCES
    src/engin/AudioEngin.cpp
    src/engin/AudioManager.cpp
    src/engin/Sound.cpp
    src/engin/util/ncm.h
    src/engin/util/utils.h
    ${CONFIG_SOURCES}
    ${CODEC_SOURCES}
    ${DEVICE_SOURCES}
    ${MIXER_SOURCES}
    ${SDL_SOURCES})

set(LIB_SOURCES src/log/colorful_log.cpp ${ENGIN_SOURCES})

set(SOURCES src/main.cpp ${LIB_SOURCES})

include_directories(includes src)

# 日志
find_package(spdlog CONFIG REQUIRED)

# json库
find_package(nlohmann_json REQUIRED)

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

if(WIN32)
  find_package(FFMPEG REQUIRED)
  find_package(PkgConfig REQUIRED)

  # rubberband库
  pkg_check_modules(RUBBERBAND REQUIRED rubberband)
else()
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

  # rubberband库
  pkg_check_modules(RUBBERBAND REQUIRED rubberband)
endif(WIN32)

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
    /opt/homebrew/Cellar/rubberband/4.0.0/lib/librubberband.3.dylib
    nlohmann_json::nlohmann_json
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
    /opt/homebrew/Cellar/rubberband/4.0.0/lib/librubberband.3.dylib
    nlohmann_json::nlohmann_json
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
    ${RUBBERBAND_LIBRARIES}
    nlohmann_json::nlohmann_json
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
  # rubberband库
  pkg_check_modules(RUBBERBAND REQUIRED rubberband)
  target_link_libraries(
    testxaudiolib
    PUBLIC nlohmann_json::nlohmann_json
    spdlog::spdlog
    ${RUBBERBAND_LIBRARIES}
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
    PUBLIC nlohmann_json::nlohmann_json
    spdlog::spdlog
    ${RUBBERBAND_LIBRARIES}
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
    PUBLIC nlohmann_json::nlohmann_json
    spdlog::spdlog
    ${RUBBERBAND_LIBRARIES}
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
    nlohmann_json::nlohmann_json
    OpenGL::GL
    glm::glm
    glfw
    E:\\Software\\vcpkg\\installed\\x64-windows\\lib\\rubberband.lib
    ${FFMPEG_LIBRARIES})
  target_link_libraries(
    xaudio
    PUBLIC OpenAL::OpenAL
    OpenCL::OpenCL
    GLEW::glew
    spdlog::spdlog
    nlohmann_json::nlohmann_json
    OpenGL::GL
    glm::glm
    glfw
    SDL2::SDL2
    E:\\Software\\vcpkg\\installed\\x64-windows\\lib\\rubberband.lib
    ${FFMPEG_LIBRARIES})
  target_link_libraries(
    xaudio-dev
    PUBLIC OpenAL::OpenAL
    OpenCL::OpenCL
    GLEW::glew
    nlohmann_json::nlohmann_json
    SDL2::SDL2
    OpenGL::GL
    glm::glm
    glfw
    spdlog::spdlog
    E:\\Software\\vcpkg\\installed\\x64-windows\\lib\\rubberband.lib
    ${FFMPEG_LIBRARIES})
else()
  # 其它系统
  message(STATUS "当前系统: ${CMAKE_SYSTEM_NAME}")
endif()
