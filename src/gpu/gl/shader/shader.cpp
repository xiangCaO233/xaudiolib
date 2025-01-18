#include "shader.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "../../../logger/logger.h"

Shader::Shader(const char *verglslfile, const char *fragglslfile,
               bool is_code) {
  const char *vertex_source;
  const char *fragment_source;
  if (!is_code) {
    std::ifstream vsource_file;
    std::ifstream fsource_file;

    std::stringstream vsstr, fsstr;

    vsource_file.open(verglslfile);
    fsource_file.open(fragglslfile);

    LOG_INFO("读取顶点着色器文件[" + std::string(verglslfile) + "]");
    vsstr << vsource_file.rdbuf();
    LOG_INFO("读取片段着色器文件[" + std::string(fragglslfile) + "]");
    fsstr << fsource_file.rdbuf();

    vsource_file.close();
    fsource_file.close();

    std::string vstr = vsstr.str();
    std::string fstr = fsstr.str();
    vertex_source = vstr.c_str();
    fragment_source = fstr.c_str();
  } else {
    // 直接赋值
    vertex_source = verglslfile;
    fragment_source = fragglslfile;
  }

  GLuint vertex_shader;
  GLuint fragment_shader;

  vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &(vertex_source), nullptr);
  LOG_INFO("开始编译顶点着色器");
  glCompileShader(vertex_shader);
  // check for shader compile errors
  check_error(vertex_shader, SType::SHADER_COMPILE);

  fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &(fragment_source), nullptr);
  LOG_INFO("开始编译片段着色器");
  glCompileShader(fragment_shader);
  // check for shader compile errors
  check_error(fragment_shader, SType::SHADER_COMPILE);

  shader_program = glCreateProgram();
  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);
  glLinkProgram(shader_program);
  // check for linking errors
  check_error(shader_program, SType::PROGRAM_LINK);

  // clear shaders
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
}

Shader::~Shader() {
  // free shader program
  glDeleteProgram(shader_program);
}

void Shader::check_error(GLuint val, SType error_type) {
  int success;
  char infoLog[1024];
  switch (error_type) {
    case SHADER_COMPILE: {
      glGetShaderiv(val, GL_COMPILE_STATUS, &success);
      if (!success) {
        glGetShaderInfoLog(val, 1024, nullptr, infoLog);
        LOG_ERROR("着色器编译失败");
        LOG_ERROR(infoLog);
      }
      break;
    }
    case PROGRAM_LINK: {
      glGetProgramiv(val, GL_LINK_STATUS, &success);
      if (!success) {
        glGetProgramInfoLog(val, 1024, nullptr, infoLog);
        LOG_ERROR("着色器链接失败");
        LOG_ERROR(infoLog);
      }
      break;
    }
  }
}

GLint Shader::uniform_loc(const char *name) {
  auto it = uniform_locs.find(name);
  if (it != uniform_locs.end()) {
    return (int)uniform_locs[name];
  }
  auto loc = glGetUniformLocation(shader_program, name);
  uniform_locs[name] = loc;
  return loc;
}

void Shader::set_sampler(const char *name, int value) const {
  glUniform1i(glGetUniformLocation(shader_program, name), value);
}

void Shader::set_unfm1f(const char *name, float value) {
  glUniform1f(uniform_loc(name), value);
}

void Shader::set_unfm1i(const char *name, int value) {
  glUniform1i(uniform_loc(name), value);
}

void Shader::set_unfm2f(const char *name, float value1, float value2) {
  glUniform2f(uniform_loc(name), value1, value2);
}

void Shader::set_unfm2i(const char *name, int value1, int value2) {
  glUniform2i(uniform_loc(name), value1, value2);
}

void Shader::set_unfm3f(const char *name, float value1, float value2,
                        float value3) {
  glUniform3f(uniform_loc(name), value1, value2, value3);
}

void Shader::set_unfm3i(const char *name, int value1, int value2, int value3) {
  glUniform3i(uniform_loc(name), value1, value2, value3);
}

void Shader::set_unfmat4f(const char *name, glm::mat4 &mat) {
  glUniformMatrix4fv(uniform_loc(name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::use() const { glUseProgram(shader_program); }

void Shader::unuse() const { glUseProgram(0); }
