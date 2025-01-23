#include "shader.h"

#include <fstream>
#include <iostream>
#include <sstream>

// 读取文件
Shader::Shader(const std::string& vsfile, const std::string& fsfile) {
  std::ifstream vsfstream, fsfstream;
  vsfstream.open(vsfile);
  fsfstream.open(fsfile);

  std::stringstream vsstream, fsstream;
  vsstream << vsfstream.rdbuf();
  fsstream << fsfstream.rdbuf();

  auto vstring = vsstream.str();
  auto fstring = fsstream.str();

  Shader(vstring.c_str(), fstring.c_str());
}
Shader::Shader(const std::string& vsfile, const std::string& gsfile,
               const std::string& fsfile) {
  std::ifstream vsfstream, gsfstram, fsfstream;
  vsfstream.open(vsfile);
  gsfstram.open(gsfile);
  fsfstream.open(fsfile);

  std::stringstream vsstream, gsstream, fsstream;
  vsstream << vsfstream.rdbuf();
  gsstream << gsfstram.rdbuf();
  fsstream << fsfstream.rdbuf();

  auto vstring = vsstream.str();
  auto gstring = gsstream.str();
  auto fstring = fsstream.str();

  Shader(vstring.c_str(), gstring.c_str(), fstring.c_str());
}

// 创建着色器
Shader::Shader(const char* vssource, const char* fssource) {
  auto vshader = GLCALL(glCreateShader(GL_VERTEX_SHADER));
  auto fshader = GLCALL(glCreateShader(GL_FRAGMENT_SHADER));

  // 注入源代码
  GLCALL(glShaderSource(vshader, 1, &vssource, nullptr));
  GLCALL(glShaderSource(fshader, 1, &fssource, nullptr));

  GLCALL(glCompileShader(vshader));
  // 检查编译错误
  int success;
  char infoLog[512];
  GLCALL(glGetShaderiv(vshader, GL_COMPILE_STATUS, &success));
  if (!success) {
    GLCALL(glGetShaderInfoLog(vshader, 512, NULL, infoLog));
    std::cout << "顶点着色器编译出错:\n" << infoLog << std::endl;
  } else {
    std::cout << "顶点着色器编译成功" << std::endl;
  }

  GLCALL(glCompileShader(fshader));
  // 检查编译错误
  GLCALL(glGetShaderiv(fshader, GL_COMPILE_STATUS, &success));
  if (!success) {
    GLCALL(glGetShaderInfoLog(fshader, 512, NULL, infoLog));
    std::cout << "片段着色器编译出错:\n" << infoLog << std::endl;
  } else {
    std::cout << "片段着色器编译成功" << std::endl;
  }
  // 链接着色器
  program = glCreateProgram();
  GLCALL(glAttachShader(program, vshader));
  GLCALL(glAttachShader(program, fshader));
  GLCALL(glLinkProgram(program));
  // 检查链接错误
  GLCALL(glGetProgramiv(program, GL_LINK_STATUS, &success));
  if (!success) {
    GLCALL(glGetProgramInfoLog(program, 512, NULL, infoLog));
    std::cout << "链接着色器出错:\n" << infoLog << std::endl;
  } else {
    std::cout << "着色器程序链接成功" << std::endl;
  }
  // 释放着色器
  GLCALL(glDeleteShader(vshader));
  GLCALL(glDeleteShader(fshader));
}
Shader::Shader(const char* vssource, const char* gssource,
               const char* fssource) {
  auto vshader = GLCALL(glCreateShader(GL_VERTEX_SHADER));
  auto gshader = GLCALL(glCreateShader(GL_GEOMETRY_SHADER));
  auto fshader = GLCALL(glCreateShader(GL_FRAGMENT_SHADER));

  // 注入源代码
  GLCALL(glShaderSource(vshader, 1, &vssource, nullptr));
  GLCALL(glShaderSource(gshader, 1, &gssource, nullptr));
  GLCALL(glShaderSource(fshader, 1, &fssource, nullptr));

  GLCALL(glCompileShader(vshader));
  // 检查编译错误
  int success;
  char infoLog[512];
  GLCALL(glGetShaderiv(vshader, GL_COMPILE_STATUS, &success));
  if (!success) {
    GLCALL(glGetShaderInfoLog(vshader, 512, NULL, infoLog));
    std::cout << "顶点着色器编译出错:\n" << infoLog << std::endl;
  } else {
    std::cout << "顶点着色器编译成功" << std::endl;
  }

  GLCALL(glCompileShader(gshader));
  // 检查编译错误
  GLCALL(glGetShaderiv(gshader, GL_COMPILE_STATUS, &success));
  if (!success) {
    GLCALL(glGetShaderInfoLog(gshader, 512, NULL, infoLog));
    std::cout << "几何着色器编译出错:\n" << infoLog << std::endl;
  } else {
    std::cout << "几何着色器编译成功" << std::endl;
  }

  GLCALL(glCompileShader(fshader));
  // 检查编译错误
  GLCALL(glGetShaderiv(fshader, GL_COMPILE_STATUS, &success));
  if (!success) {
    GLCALL(glGetShaderInfoLog(fshader, 512, NULL, infoLog));
    std::cout << "片段着色器编译出错:\n" << infoLog << std::endl;
  } else {
    std::cout << "片段着色器编译成功" << std::endl;
  }

  // 链接着色器
  program = glCreateProgram();
  GLCALL(glAttachShader(program, vshader));
  GLCALL(glAttachShader(program, gshader));
  GLCALL(glAttachShader(program, fshader));
  GLCALL(glLinkProgram(program));
  // 检查链接错误
  GLCALL(glGetProgramiv(program, GL_LINK_STATUS, &success));
  if (!success) {
    GLCALL(glGetProgramInfoLog(program, 512, NULL, infoLog));
    std::cout << "链接着色器出错:\n" << infoLog << std::endl;
  } else {
    std::cout << "着色器程序链接成功" << std::endl;
  }

  // 释放着色器
  GLCALL(glDeleteShader(vshader));
  GLCALL(glDeleteShader(gshader));
  GLCALL(glDeleteShader(fshader));
}

Shader::~Shader() {
  // 释放着色器
  GLCALL(glDeleteProgram(program));
}

// 使用着色器(主)
void Shader::use() { glUseProgram(program); }
void Shader::unuse() { glUseProgram(0); }

// 获取统一变量location
GLint Shader::uniform_loc(const std::string& name) {
  auto it = uniformLocationCache.find(name);
  if (it != uniformLocationCache.end()) return (int)uniformLocationCache[name];
  auto loc = GLCALL(glGetUniformLocation(program, name.c_str()));
  uniformLocationCache[name] = loc;
  return loc;
}

// 设置uniform变量
void Shader::set_sampler(const std::string& name, int value) const {
  auto pos = GLCALL(glGetUniformLocation(program, name.c_str()))
  GLCALL(glUniform1i(pos, value));
}
void Shader::set_unfm1f(const std::string& name, float value) {
  GLCALL(glUniform1f(uniform_loc(name), value));
}
void Shader::set_unfm1i(const std::string& name, int value) {
  GLCALL(glUniform1i(uniform_loc(name), value));
}
void Shader::set_unfm2f(const std::string& name, float value1, float value2) {
  GLCALL(glUniform2f(uniform_loc(name), value1, value2));
}
void Shader::set_unfm2i(const std::string& name, int value1, int value2) {
  GLCALL(glUniform2i(uniform_loc(name), value1, value2));
}
void Shader::set_unfm3f(const std::string& name, float value1, float value2,
                        float value3) {
  GLCALL(glUniform3f(uniform_loc(name), value1, value2, value3));
}
void Shader::set_unfm3i(const std::string& name, int value1, int value2,
                        int value3) {
  GLCALL(glUniform3i(uniform_loc(name), value1, value2, value3));
}
void Shader::set_unfmat4f(const std::string& name, glm::mat4& mat) {
  GLCALL(glUniformMatrix4fv(uniform_loc(name), 1, GL_FALSE, &mat[0][0]));
}
