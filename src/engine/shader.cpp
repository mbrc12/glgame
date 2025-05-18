#include "engine/shader.hpp"
#include "glad/glad.h"
#include "common.hpp"
#include <cassert>
#include <ranges>

namespace Engine {

constexpr int MAX_INFO_LOG_SIZE = 1024;

constexpr const char *DEFAULT_VERTEX_SHADER = R"(
#version 330 core
layout(location = 0) in vec3 aPos;

void main() {
    gl_Position = vec4(aPos, 1.0);
}
)";

constexpr const char *DEFAULT_FRAGMENT_SHADER = R"(
#version 330 core
out vec4 FragColor;
uniform vec4 ourColor;

void main() {
    FragColor = ourColor;
})";

// Helper functions
GLint getAttributeLocation(GLuint program, const std::string &name) {
    GLint location = glGetAttribLocation(program, name.data());
    if (location == -1) {
        DBG("attribute " << name << " not found in shader program, or is invalid attribute name");
        assert(false);
    }
    return location;
}

GLint getUniformLocation(GLuint program, const std::string &name) {
    GLint location = glGetUniformLocation(program, name.data());
    if (location == -1) {
        DBG("uniform " << name << " not found in shader program, or is invalid uniform name");
        assert(false);
    }
    return location;
}

// Shader implementation
Shader::Shader()
    : vertex_shader_source{DEFAULT_VERTEX_SHADER}, fragment_shader_source{DEFAULT_FRAGMENT_SHADER}, is_built{false} {}

Shader::~Shader() { glDeleteProgram(shader_program); }

void Shader::setVertexShader(const std::string &vertex_shader_source) {
    this->vertex_shader_source = std::string(vertex_shader_source);
}

void Shader::setFragmentShader(const std::string &fragment_shader_source) {
    this->fragment_shader_source = std::string(fragment_shader_source);
}

void Shader::build() {
    // Vertex shader
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);

    auto vertex_shader_cstr = vertex_shader_source.c_str();
    glShaderSource(vertex_shader, 1, &vertex_shader_cstr, NULL);
    glCompileShader(vertex_shader);

    GLint success;
    char infoLog[MAX_INFO_LOG_SIZE];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
        DBG("vertex shader compilation failed: \n" << infoLog);
        assert(false);
    }

    // Fragment shader
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    auto fragment_shader_cstr = fragment_shader_source.c_str();
    glShaderSource(fragment_shader, 1, &fragment_shader_cstr, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
        DBG("fragment shader compilation failed: \n" << infoLog);
        assert(false);
    }

    // Linking shaders
    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program, 512, NULL, infoLog);
        DBG("shader program linking failed: \n" << infoLog);
        assert(false);
    }

    // Cleanup
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    is_built = true;
}

void Shader::setFloatUniform(const std::string &name, float value) { float_uniforms[name] = value; }

void Shader::setFloatArrayUniform(const std::string &name, const float *values, size_t size) {
    float_array_uniforms[name] = std::vector<GLfloat>(values, values + size);
}

void Shader::setVec3Uniform(const std::string &name, const glm::vec3 value) {
    vec3_uniforms[name] = {value.x, value.y, value.z};
}

void Shader::setVec3ArrayUniform(const std::string &name, const glm::vec3 *values, size_t size) {
    auto vec = std::span{values, size} |
                std::views::transform([](const glm::vec3 &v) { return std::array<GLfloat, 3>{v.x, v.y, v.z}; }) |
                std::views::join |
                std::ranges::to<std::vector<GLfloat>>();
    vec3_array_uniforms[name] = vec;
}

void Shader::setVec4Uniform(const std::string &name, const glm::vec4 value) {
    vec4_uniforms[name] = {value.x, value.y, value.z, value.w};
}

void Shader::setVec4ArrayUniform(const std::string &name, const glm::vec4 *values, size_t size) {
    auto vec = std::span{values, size} |
                std::views::transform([](const glm::vec4 &v) { return std::array<GLfloat, 4>{v.x, v.y, v.z, v.w}; }) |
                std::views::join |
                std::ranges::to<std::vector<GLfloat>>();
    vec4_array_uniforms[name] = vec;
}

void Shader::use() const {
    assert(is_built);

    glUseProgram(shader_program);

    for (const auto &[name, value] : float_uniforms) {
        GLint location = getUniformLocation(shader_program, name);
        glUniform1f(location, value);
    }

    for (const auto &[name, values] : float_array_uniforms) {
        GLint location = getUniformLocation(shader_program, name);
        glUniform1fv(location, static_cast<GLsizei>(values.size()), values.data());
    }

    for (const auto &[name, values] : vec3_uniforms) {
        GLint location = getUniformLocation(shader_program, name);
        glUniform3f(location, values[0], values[1], values[2]);
    }

    for (const auto &[name, values] : vec3_array_uniforms) {
        GLint location = getUniformLocation(shader_program, name);
        glUniform3fv(location, static_cast<GLsizei>(values.size()), values.data());
    }

    for (const auto &[name, values] : vec4_uniforms) {
        GLint location = getUniformLocation(shader_program, name);
        glUniform4f(location, values[0], values[1], values[2], values[3]);
    }

    for (const auto &[name, values] : vec4_array_uniforms) {
        GLint location = getUniformLocation(shader_program, name);
        glUniform4fv(location, static_cast<GLsizei>(values.size()), values.data());
    }
}

}; // namespace Engine
