#pragma once
#include "material.hpp"
#include "mesh.hpp"

class ModelImporter
{
public:
    ModelImporter(std::string const &path, Transferer &transferer);

public:
    std::vector<Mesh> mMeshes;
    std::vector<Material> mMaterials;
};
