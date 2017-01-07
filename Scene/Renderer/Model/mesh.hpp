#pragma once

#include <vector>
#include "vulkan/vulkan.hpp"
#include <assimp/mesh.h>

struct Mesh {
    Mesh(aiMesh const *mesh);

    unsigned materialIndex;
    unsigned numberVertices;
    unsigned numberIndices;
    std::vector<Vertex> vertices;
    std::vector<unsigned> indices;
};
