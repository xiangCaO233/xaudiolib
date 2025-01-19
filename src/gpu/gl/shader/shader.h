#ifndef XGL3D_SHADER_H
#define XGL3D_SHADER_H

#include <string>
#include <unordered_map>

#include "../glheaders.h"

class Shader {
  // shader程序程序
  GLuint program;

  // uniform变量location缓存
  std::unordered_map<std::string, GLint> uniformLocationCache;

 public:
  // 构造Shader
  // vertexshader/fragmentshader
  Shader(const std::string& vsfile, const std::string& fsfile);
  Shader(const char* vssource, const char* fssource);
  // vertexshader/geometryshader/fragmentshader
  Shader(const std::string& vsfile, const std::string& gsfile,
         const std::string& fsfile);
  Shader(const char* vssource, const char* gssource, const char* fssource);

  // 析构Shader
  virtual ~Shader();

  // 使用着色器
  void use();
  void unuse();

  // 获取统一变量location
  GLint uniform_loc(const std::string& name);

  // 设置uniform变量
  void set_sampler(const std::string& name, int value) const;
  void set_unfm1f(const std::string& name, float value);
  void set_unfm1i(const std::string& name, int value);
  void set_unfm2f(const std::string& name, float value1, float value2);
  void set_unfm2i(const std::string& name, int value1, int value2);
  void set_unfm3f(const std::string& name, float value1, float value2,
                  float value3);
  void set_unfm3i(const std::string& name, int value1, int value2, int value3);
  void set_unfmat4f(const std::string& name, glm::mat4& mat);
};

#endif  // XGL3D_SHADER_H
