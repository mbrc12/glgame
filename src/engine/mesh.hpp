#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <variant>
#include <vector>

namespace Engine {

enum class MeshType : GLenum {
    Points = GL_POINTS,
    Lines = GL_LINES,
    LineStrip = GL_LINE_STRIP,
    LineLoop = GL_LINE_LOOP,
    Triangles = GL_TRIANGLES,
    TriangleStrip = GL_TRIANGLE_STRIP,
    TriangleFan = GL_TRIANGLE_FAN,
};

// Assumes that the first element is the vertex position
class Mesh {
  public:
    explicit Mesh();
    ~Mesh();

    MeshType type;

    template <typename T> void setVertexPositions(const T *data, size_t count) {
        static_assert(std::is_same_v<T, glm::vec3> || std::is_same_v<T, glm::vec4>, "T must be glm::vec3 or glm::vec4");

        if (store.size() < 1) {
            store.resize(1);
            widths.resize(1);
        }

        widths[0] = sizeof(T) / sizeof(float);

        store[0] = std::vector<T>(data, data + count);

        buffer.clear();
    }

    template <typename T> void setVertexPositions(const std::initializer_list<T> &data) {
        setVertexPositions(data.begin(), data.size()); 
    }

    template <typename T> void setAssociatedData(size_t index, const T *data, size_t count) {
        static_assert(std::is_same_v<T, glm::vec2> || std::is_same_v<T, glm::vec3> || std::is_same_v<T, glm::vec4>,
                      "T must be glm::vec2 or glm::vec3 or glm::vec4");

        if (store.size() < index + 1) {
            store.resize(index + 1);
            widths.resize(index + 1);
        }

        widths[index] = sizeof(T) / sizeof(float);

        store[index] = std::vector<T>(data, data + count);

        buffer.clear();
    }

    template <typename T> void setAssociatedData(size_t index, const std::initializer_list<T> &data) {
        setAssociatedData<T>(index, data.begin(), data.size());
    }

    size_t getVertexCount();

    void setElementBuffer(const uint *data, size_t count, MeshType type = MeshType::Triangles);

    void setElementBuffer(const std::initializer_list<uint> &data, MeshType type = MeshType::Triangles);

    void draw();

  private:
    GLuint vao, vbo, ebo; // vertex array object, vertex buffer object, element buffer object

    std::vector<size_t> widths;
    size_t vertex_count;

    std::vector<std::variant<std::vector<glm::vec2>, std::vector<glm::vec3>, std::vector<glm::vec4>>> store;

    std::vector<uint> element_buffer;
    std::vector<float> buffer;

    void transferToGPU();
};

}; // namespace Engine
