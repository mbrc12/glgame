#include "engine/texture.hpp"
#include "common.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace Engine {

Texture::Texture() : loaded(false) {}

Texture::Texture(const std::string &path) : loaded(false) { load(path); }

Texture::~Texture() {
    if (loaded) {
        glDeleteTextures(1, &texture);
    }
}

void Texture::load(const std::string &path) {
    int width, height, channels;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (!data) {
        DBG("Failed to load texture: " << path);
        assert(false);
    }

    GLenum format = channels == 3 ? GL_RGB : GL_RGBA;
    DBG("Loaded texture: " << path << " (" << width << "x" << height << ", " << channels << " channels)");

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    auto wrap_ = static_cast<GLenum>(wrap);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    loaded = true;
}

void Texture::setWrap(TextureWrap wrap) {
    this->wrap = wrap;
    if (loaded) {
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLenum>(wrap));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLenum>(wrap));
    }
}

}; // namespace Engine
