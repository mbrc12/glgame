#pragma once

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <array>
#include <vector>
#include <map>

namespace Engine {

class Shader {

  private:
    std::string vertex_shader_source;
    std::string fragment_shader_source;
    GLuint shader_program;
    bool is_built = false;
    std::map<std::string, GLfloat> float_uniforms;
    std::map<std::string, std::vector<GLfloat>> float_array_uniforms;

    std::map<std::string, std::array<GLfloat, 3>> vec3_uniforms;
    std::map<std::string, std::vector<GLfloat>> vec3_array_uniforms;

    std::map<std::string, std::array<GLfloat, 4>> vec4_uniforms;
    std::map<std::string, std::vector<GLfloat>> vec4_array_uniforms;

  public:
    Shader();
    ~Shader();
    void setVertexShader(const std::string &vertex_shader_source);
    void setFragmentShader(const std::string &fragment_shader_source);

    void build();
    void use() const;

    void setFloatUniform(const std::string &name, GLfloat value);
    void setFloatArrayUniform(const std::string &name, const float *values, size_t size);

    void setVec3Uniform(const std::string &name, const glm::vec3 value);
    void setVec3ArrayUniform(const std::string &name, const glm::vec3 *values, size_t size);

    void setVec4Uniform(const std::string &name, const glm::vec4 value);
    void setVec4ArrayUniform(const std::string &name, const glm::vec4 *values, size_t size);
};

}; // namespace Engine
