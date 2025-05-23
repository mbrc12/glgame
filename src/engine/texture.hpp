#pragma once
#include <glad/glad.h>
#include <string>

namespace Engine {

enum class TextureWrap {
    Repeat = GL_REPEAT,
    MirroredRepeat = GL_MIRRORED_REPEAT,
    ClampToEdge = GL_CLAMP_TO_EDGE,
};

class Texture {
  private:
    GLuint texture;
    bool loaded = false;
    TextureWrap wrap = TextureWrap::ClampToEdge;

  public:
    explicit Texture();
    explicit Texture(const std::string& path);
    ~Texture();

    void load(const std::string& path);
    void setWrap(TextureWrap wrap);
    void bind(GLuint index = 0);
};

}; // namespace Engine
