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
  auto vshader = glCreateShader(GL_VERTEX_SHADER);
  auto fshader = glCreateShader(GL_FRAGMENT_SHADER);

  // 注入源代码
  glShaderSource(vshader, 1, &vssource, nullptr);
  glShaderSource(fshader, 1, &fssource, nullptr);

  glCompileShader(vshader);
  // 检查编译错误
  int success;
  char infoLog[512];
  glGetShaderiv(vshader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vshader, 512, NULL, infoLog);
    std::cout << "顶点着色器编译出错:\n" << infoLog << std::endl;
  }

  glCompileShader(fshader);
  // 检查编译错误
  glGetShaderiv(fshader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fshader, 512, NULL, infoLog);
    std::cout << "片段着色器编译出错:\n" << infoLog << std::endl;
  }
  // 链接着色器
  program = glCreateProgram();
  glAttachShader(program, vshader);
  glAttachShader(program, fshader);
  glLinkProgram(program);
  // 检查链接错误
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(program, 512, NULL, infoLog);
    std::cout << "链接着色器出错:\n" << infoLog << std::endl;
  }
  // 释放着色器
  glDeleteShader(vshader);
  glDeleteShader(fshader);
}
Shader::Shader(const char* vssource, const char* gssource,
               const char* fssource) {
  auto vshader = glCreateShader(GL_VERTEX_SHADER);
  auto gshader = glCreateShader(GL_GEOMETRY_SHADER);
  auto fshader = glCreateShader(GL_FRAGMENT_SHADER);

  // 注入源代码
  glShaderSource(vshader, 1, &vssource, nullptr);
  glShaderSource(gshader, 1, &gssource, nullptr);
  glShaderSource(fshader, 1, &fssource, nullptr);

  glCompileShader(vshader);
  // 检查编译错误
  int success;
  char infoLog[512];
  glGetShaderiv(vshader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vshader, 512, NULL, infoLog);
    std::cout << "顶点着色器编译出错:\n" << infoLog << std::endl;
  }

  glCompileShader(gshader);
  // 检查编译错误
  glGetShaderiv(gshader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(gshader, 512, NULL, infoLog);
    std::cout << "几何着色器编译出错:\n" << infoLog << std::endl;
  }

  glCompileShader(fshader);
  // 检查编译错误
  glGetShaderiv(fshader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fshader, 512, NULL, infoLog);
    std::cout << "片段着色器编译出错:\n" << infoLog << std::endl;
  }
  // 链接着色器
  program = glCreateProgram();
  glAttachShader(program, vshader);
  glAttachShader(program, gshader);
  glAttachShader(program, fshader);
  glLinkProgram(program);
  // 检查链接错误
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(program, 512, NULL, infoLog);
    std::cout << "链接着色器出错:\n" << infoLog << std::endl;
  }
  // 释放着色器
  glDeleteShader(vshader);
  glDeleteShader(fshader);
}

Shader::~Shader() {
  // 释放着色器
  glDeleteProgram(program);
}

// 使用着色器(主)
void Shader::use() { glUseProgram(program); }
void Shader::unuse() { glUseProgram(0); }

// 获取统一变量location
GLint Shader::uniform_loc(const std::string& name) {
  auto it = uniformLocationCache.find(name);
  if (it != uniformLocationCache.end()) return (int)uniformLocationCache[name];
  auto loc = glGetUniformLocation(program, name.c_str());
  uniformLocationCache[name] = loc;
  return loc;
}

// 设置uniform变量
void Shader::set_sampler(const std::string& name, int value) const {
  glUniform1i(glGetUniformLocation(program, name.c_str()), value);
}
void Shader::set_unfm1f(const std::string& name, float value) {
  glUniform1f(uniform_loc(name), value);
}
void Shader::set_unfm1i(const std::string& name, int value) {
  glUniform1i(uniform_loc(name), value);
}
void Shader::set_unfm2f(const std::string& name, float value1, float value2) {
  glUniform2f(uniform_loc(name), value1, value2);
}
void Shader::set_unfm2i(const std::string& name, int value1, int value2) {
  glUniform2i(uniform_loc(name), value1, value2);
}
void Shader::set_unfm3f(const std::string& name, float value1, float value2,
                        float value3) {
  glUniform3f(uniform_loc(name), value1, value2, value3);
}
void Shader::set_unfm3i(const std::string& name, int value1, int value2,
                        int value3) {
  glUniform3i(uniform_loc(name), value1, value2, value3);
}
void Shader::set_unfmat4f(const std::string& name, glm::mat4& mat) {
  glUniformMatrix4fv(uniform_loc(name), 1, GL_FALSE, &mat[0][0]);
}
