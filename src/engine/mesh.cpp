#include "engine/mesh.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <numeric>
#include <algorithm>
#include "common.hpp"

namespace Engine {

Mesh::Mesh() : type{MeshType::Triangles}, widths{}, store{}, buffer{}, element_buffer{} {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

void Mesh::setElementBuffer(const uint *data, size_t count, MeshType type) {
    this->type = type;
    element_buffer.resize(count);
    std::copy_n(data, count, element_buffer.begin());
}

void Mesh::setElementBuffer(const std::initializer_list<uint> &data, MeshType type) {
    this->type = type;
    element_buffer.resize(data.size());
    std::copy(data.begin(), data.end(), element_buffer.begin());
}

void Mesh::draw() {
    transferToGPU();

    auto primitive_type = static_cast<GLenum>(type);
    glBindVertexArray(vao);

    if (!element_buffer.empty()) {
        glDrawElements(primitive_type, element_buffer.size(), GL_UNSIGNED_INT, 0);
    } else {
        glDrawArrays(primitive_type, 0, vertex_count);
    }

    glBindVertexArray(0);
}

size_t Mesh::getVertexCount() {
    vertex_count = std::numeric_limits<size_t>::max();
    for (size_t field = 0; field < store.size(); field++) {
        size_t size = std::visit([](auto &v) { return v.size(); }, store[field]);
        vertex_count = std::min(vertex_count, size);
    }
    
    if (vertex_count == std::numeric_limits<size_t>::max()) {
        vertex_count = 0;
    }

    return vertex_count;
}

void Mesh::transferToGPU() {
    if (!buffer.empty()) {
        return;
    }

    size_t total_width = std::accumulate(widths.begin(), widths.end(), 0);

    vertex_count = getVertexCount();
    buffer.resize(vertex_count * total_width);

    size_t offset = 0;

    for (size_t i = 0; i < vertex_count; i++) {
        offset = 0;
        for (size_t field = 0; field < store.size(); ++field) {
            std::visit(
                [&](auto &xs) {
                    std::copy_n(glm::value_ptr(xs[i]), widths[field], buffer.begin() + total_width * i + offset);
                },
                store[field]);
            offset += widths[field];
        }
    }

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(float), buffer.data(), GL_STATIC_DRAW);

    if (!element_buffer.empty()) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, element_buffer.size() * sizeof(uint), element_buffer.data(),
                     GL_STATIC_DRAW);
    }

    offset = 0;

    for (size_t field = 0; field < store.size(); ++field) {
        glVertexAttribPointer(field, widths[field], GL_FLOAT, GL_FALSE, total_width * sizeof(float),
                              (void *)(offset * sizeof(float)));
        glEnableVertexAttribArray(field);
        offset += widths[field];
    }

    glBindVertexArray(0);
}

}; // namespace Engine
