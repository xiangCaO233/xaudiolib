#ifndef SHADER_H
#define SHADER_H

#include <unordered_map>

#include "../../../../includes/glinclude/glcore.h"

enum SType { SHADER_COMPILE, PROGRAM_LINK };

class Shader {
    GLuint program{};
    std::unordered_map<const char *, GLuint> uniform_locs;

    friend class Xmesh;

    friend class XquadMesh;

   public:
    // 构造Shader
    Shader(const char *verglslfile, const char *fragglslfile, bool is_code);

    // 析构Shader
    virtual ~Shader();

    // 使用此Shader
    void use() const;

    // 取消使用此Shader
    void unuse() const;

    // 设置uniform变量
    void set_sampler(const char *name, int value) const;

    void set_unfm1f(const char *name, float value);

    void set_unfm1i(const char *name, int value);

    void set_unfm2f(const char *name, float value1, float value2);

    void set_unfm2i(const char *name, int value1, int value2);

    void set_unfm3f(const char *name, float value1, float value2, float value3);

    void set_unfm3i(const char *name, int value1, int value2, int value3);

    void set_unfmat4f(const char *name, glm::mat4 &mat);

   private:
    GLuint shader_program{0};

    // 检查着色器及链接器编译或链接错误
    static void check_error(GLuint val, SType error_type);

    // 获取统一变量location
    GLint uniform_loc(const char *name);
};

#endif /* SHADER_H */
