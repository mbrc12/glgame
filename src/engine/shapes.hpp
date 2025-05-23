#pragma once
#include "engine/mesh.hpp"

namespace Engine {

Mesh cuboidMesh(float width, float height = -1, float depth = -1);
Mesh sphereMesh(float radius, size_t splits = 10);

// Mesh randomShape(size_t splits = 10);

} // namespace Engine
