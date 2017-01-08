#pragma once

#include <assimp/material.h>
#include "VkTools/Image/texture.hpp"

struct Material {
    Material(aiMaterial *mtl, std::string const &globalPath, Transferer &transferer);

    Texture diffuseTexture;
    glm::vec3 color;
    bool useTexture;
};
