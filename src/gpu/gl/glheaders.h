#ifndef XGL3D_GLHEADERS_H
#define XGL3D_GLHEADERS_H

// 用于包装 OpenGL 调用并检查错误
#define GLCALL(func)                                       \
  func;                                                    \
  {                                                        \
    GLenum error = glGetError();                           \
    if (error != GL_NO_ERROR) {                            \
      XERROR("在[" + std::string(#func) +                  \
             "]发生OpenGL错误: " + std::to_string(error)); \
    }                                                      \
  }

// 操作系统判断宏
#ifdef __APPLE__
#define OS_MACOS
#elif defined(_WIN32)
#define OS_WINDOWS
#elif defined(__linux__)
#define OS_LINUX
#endif

#include <GL/glew.h>

#pragma once
#include <GLFW/glfw3.h>
#include <GL/gl.h>

#include <glm/glm.hpp>

#endif  // XGL3D_GLHEADERS_H
