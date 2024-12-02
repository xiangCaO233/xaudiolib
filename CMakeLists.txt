cmake_minimum_required(VERSION 3.31)

project(xaudiolib)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(LIB_SOURCES)
set(SOURCES src/main.cpp)

add_executable(testxaudiolib ${SOURCES})
# add_library(libxaudio STATIC ${LIB_SOURCES})

# add_library(xaudio SHARED ${LIB_SOURCES})

# target_link_libraries(testxaudiolib )

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  # macos 查找 Core Audio 框架
  find_library(COREAUDIO_LIBRARY CoreAudio)
  find_library(AUDIOTOOLBOX_LIBRARY AudioToolbox)
  find_library(AUDIOUNIT_LIBRARY AudioUnit)

  # 链接 Core Audio 相关库
  target_link_libraries(
    testxaudiolib PRIVATE ${COREAUDIO_LIBRARY} ${AUDIOTOOLBOX_LIBRARY}
                          ${AUDIOUNIT_LIBRARY})
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  # linux
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  # windows
else()
  # 其他系统
  message(STATUS "当前系统: ${CMAKE_SYSTEM_NAME}")
endif()
