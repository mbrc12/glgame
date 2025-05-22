#include "engine/shapes.hpp"

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

    mesh.setElementBuffer({
        0b000, 0b001, 0b011,
        0b000, 0b001, 0b101,
        0b000, 0b010, 0b011,
        0b000, 0b010, 0b110,
        0b000, 0b100, 0b101,
        0b000, 0b100, 0b110,
        0b001, 0b011, 0b111,
        0b001, 0b101, 0b111,
        0b010, 0b011, 0b111,
        0b010, 0b110, 0b111,
        0b100, 0b101, 0b111,
        0b100, 0b110, 0b111,
    });

    return mesh;
}

}
