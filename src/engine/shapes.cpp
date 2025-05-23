#include "engine/shapes.hpp"
#include <random>
#include <array>
#include <ranges>

namespace Engine {

Mesh cuboidMesh(float width, float height, float depth) {
    if (height < 0) {
        height = width;
    }
    if (depth < 0) {
        depth = width;
    }

    Mesh mesh;

    mesh.setVertexPositions<glm::vec3>({
        {-width, -height, -depth},
        {-width, -height, depth},
        {-width, height, -depth},
        {-width, height, depth},
        {width, -height, -depth},
        {width, -height, depth},
        {width, height, -depth},
        {width, height, depth},
    });

    mesh.setAssociatedData<glm::vec2>(1, {
                                             {0.f, 0.f},
                                             {0.f, 1.f},
                                             {1.f, 0.f},
                                             {1.f, 1.f},
                                             {1.f, 0.f},
                                             {1.f, 0.f},
                                             {1.f, 1.f},
                                             {1.f, 0.f},
                                         });

    mesh.setElementBuffer({
        0b000, 0b001, 0b011, 0b000, 0b001, 0b101, 0b000, 0b010, 0b011, 0b000, 0b010, 0b110,
        0b000, 0b100, 0b101, 0b000, 0b100, 0b110, 0b001, 0b011, 0b111, 0b001, 0b101, 0b111,
        0b010, 0b011, 0b111, 0b010, 0b110, 0b111, 0b100, 0b101, 0b111, 0b100, 0b110, 0b111,

    });

    return mesh;
}

Mesh sphereMesh(float radius, size_t splits) {
    Mesh mesh;

    std::vector<glm::vec3> vertices;
    std::vector<std::array<size_t, 3>> indices;

    for (size_t i = 0; i <= splits; ++i) {
        float theta = i * std::numbers::pi / splits;
        for (size_t j = 0; j <= splits; ++j) {
            float phi = j * 2.f * std::numbers::pi / splits;
            float x = radius * sin(theta) * cos(phi);
            float y = radius * sin(theta) * sin(phi);
            float z = radius * cos(theta);
            vertices.push_back({x, y, z});
        }
    }

    for (size_t i = 0; i < splits; ++i) {
        for (size_t j = 0; j < splits; ++j) {
            size_t a = i * (splits + 1) + j;
            size_t b = a + splits + 1;
            size_t c = a + 1;
            size_t d = b + 1;

            indices.push_back({a, b, c});
            indices.push_back({b, d, c});
        }
    }

    mesh.setVertexPositions<glm::vec3>(vertices.data(), vertices.size());
    auto indices_flat = indices | std::views::join | std::ranges::to<std::vector<GLuint>>();
    mesh.setElementBuffer(indices_flat.data(), indices_flat.size());

    return mesh;
}

// Mesh randomShape(size_t splits) {
//     static std::random_device rd;
//     static std::minstd_rand rng(rd());
//
//
//     std::vector<glm::vec3> vertices {
//         {-1.f, 0.f, -1.f},
//         {1.f, 0.f, -1.f},
//         {0.f, 0.f, 1.f},
//     };
//
//     std::vector<std::array<size_t, 3>> indices {
//         {0, 1, 2},
//     };
//
//     for (int i = 0; i < splits; i++) {
//         auto idx = std::uniform_int_distribution(0, (int)indices.size() - 1)(rng);
//         auto a = indices[idx][0];
//         auto b = indices[idx][1];
//         auto c = indices[idx][2];
//         auto v = (vertices[a] + vertices[b] + vertices[c]) / 3.f;
//         v += std::uniform_real_distribution(-1.f, 1.f)(rng) * glm::vec3(0.2f, 0.2f, 0.2f);
//         vertices.push_back(v);
//         auto d = vertices.size() - 1;
//
//         indices.erase(indices.begin() + idx);
//         indices.push_back({a, b, d});
//         indices.push_back({b, c, d});
//         indices.push_back({c, a, d});
//     }
//
//     auto mesh = Mesh();
//     mesh.setVertexPositions<glm::vec3>(vertices.data(), vertices.size());
//
//     auto indices_flat = indices | std::ranges::views::join | std::ranges::to<std::vector<GLuint>>();
//     mesh.setElementBuffer(indices_flat.data(), indices_flat.size());
//
//     return mesh;
// }
//
} // namespace Engine
